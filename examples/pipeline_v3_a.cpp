#include <iostream>
#include <functional>
#include <thread>
#include <atomic>
#include <mutex>
#include <unistd.h>
#include <queue>
#include <condition_variable>
#include <chrono>

struct timeSince {
    struct duration {
        typedef std::ratio<1l, 1000000000l> nano;
        typedef std::chrono::duration<unsigned long long,         std::nano> nanoseconds;
        typedef std::chrono::duration<unsigned long long,        std::micro> microseconds;
        typedef std::chrono::duration<unsigned long long,        std::milli> milliseconds;
        typedef std::chrono::duration<unsigned long long                   > seconds;
        typedef std::chrono::duration<     unsigned long, std::ratio<  60> > minutes;
        typedef std::chrono::duration<     unsigned long, std::ratio<3600> > hours;
    };
    
    std::string time;

    std::chrono::time_point<std::chrono::high_resolution_clock> start;
    
    const timeSince & elapse() {

        duration::nanoseconds nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - start);

        duration::microseconds microseconds = std::chrono::duration_cast<duration::microseconds>(nanoseconds);
        nanoseconds -= std::chrono::duration_cast<duration::nanoseconds>(microseconds);

        duration::milliseconds milliseconds = std::chrono::duration_cast<duration::milliseconds>(microseconds);
        microseconds -= std::chrono::duration_cast<duration::milliseconds>(microseconds);

        duration::seconds seconds = std::chrono::duration_cast<duration::seconds>(milliseconds);
        milliseconds -= std::chrono::duration_cast<duration::milliseconds>(seconds);

        seconds -= std::chrono::duration_cast<duration::seconds>(std::chrono::duration_cast<duration::minutes>(seconds));
    
        auto s = seconds.count();
        time = "";
        if (s < 10) time += "0";
        time += std::to_string(s);
        time += ":";
        auto mil = milliseconds.count();
        if (mil < 10) time += "00";
        else if (mil < 100) time += "0";
        time += std::to_string(mil);
        time += ":";
        auto micr = microseconds.count();
        if (micr < 10) time += "00";
        else if (micr < 100) time += "0";
        time += std::to_string(micr);
        time += ":";
        auto n = nanoseconds.count();
        if (n < 10) time += "00";
        else if (n < 100) time += "0";
        time += std::to_string(n);
        return *this;
    }
    
    void mark() {
        start = std::chrono::high_resolution_clock::now();
    }
    
    friend std::ostream &operator<<(std::ostream & output, const timeSince & D) {
        output << const_cast<timeSince &>(D).elapse().time;
        return output;
    }
};

#define ELPP_THREAD_SAFE
#define ELPP_FORCE_USE_STD_THREAD
#define ELPP_STL_LOGGING

#include "easyloggingpp/src/easylogging++.h"
#include "easyloggingpp/src/easylogging++.cc"

INITIALIZE_EASYLOGGINGPP

#include <iostream>
#include <functional>
#include <deque>
#include <cassert>

#define PipelineLambda(val, index, pipeline) [] (auto && val, int index, auto * pipeline)

timeSince program_start;

template <typename T>
struct Pipeline {
    el::Logger* pipelineLogger = nullptr;
    el::Configurations config;

    static std::string getTime(const el::LogMessage* message) {
        return program_start.elapse().time;
    }
    
    Pipeline () {
        pipelineLogger = el::Loggers::getLogger("pipeline");
        el::Helpers::installCustomFormatSpecifier(el::CustomFormatSpecifier("%TIMESINCESTART", getTime));
        config.setToDefault();
    }
    
    // if this is false the pipeline will execute in producer-consumer mode
    // this is very similar to how an actual pipelined cpu operates
    bool sequential = false;
    bool manual_increment = false;
    
    // move the input instead of copying it
    // this allows for input mutation
    typedef std::function<void(T && val, int index, Pipeline<T> * pipeline)> Task;
    
    struct Stage {
        Task pre = nullptr;
        Task run = nullptr;
        Task post = nullptr;
    };
    
    std::deque<Stage> stages;

    #define PipelineLambdaArguments Pipeline * pipeline, const Stage & stage, int index, std::deque<T> * input, std::deque<T> * output, std::atomic<bool> * haltC, std::atomic<bool> * haltN, std::condition_variable * cvP, std::condition_variable * cvC, std::condition_variable * cvN, std::condition_variable * cvF, std::mutex * mC, std::mutex * mN
    
    typedef std::function<void(PipelineLambdaArguments)> TaskCallback;
    
    #define PipelineLambdaCallback [] (PipelineLambdaArguments)

    TaskCallback callbackA = PipelineLambdaCallback {
        auto qs = input->size();
        while(pipeline->PC < qs) {
            auto val = std::move(input->at(pipeline->PC));
            
//             if (output != nullptr) {
//                 std::unique_lock<std::mutex> lkN(*mN);
//                 cvC->wait(lkN, [output]{ return output->size() < 1;});
//             }
            
            stage.run(std::move(val), index, pipeline);
            
            if (output != nullptr) {
                std::unique_lock<std::mutex> lkN(*mN);
                cvC->wait(lkN, [output]{ return output->size() < 1;});
                output->push_back(val);
                cvN->notify_one();
                lkN.unlock();
            }
        }
        std::unique_lock<std::mutex> lkN(*mN);
        haltN->store(true);
        cvN->notify_one();
        lkN.unlock();
    };
    
    TaskCallback callbackB = PipelineLambdaCallback {
        while(true) {
//             if (output != nullptr) {
//                 std::unique_lock<std::mutex> lkN(*mN);
//                 cvC->wait(lkN, [output]{ return output->size() < 1;});
//             }
            std::unique_lock<std::mutex> lkC(*mC);
            cvC->wait(lkC, [input, haltC]{
                // block until the following conditions are met:
                return
                    // input is not empty
                    !input->empty()
                    // or we are told to halt, and the input is empty
                    // note that because we check if the input is not empty,
                    // before we check this, this will not reached,
                    // if we are told to halt, but input is not empty
                    || (haltC->load() && input->empty());
            });
            if (haltC->load() && input->empty()) {
                std::unique_lock<std::mutex> lkN(*mN);
                haltN->store(true);
                cvN->notify_one();
                lkN.unlock();
                break;
            }
            auto val = std::move(input->front());
            input->pop_front();
            cvP->notify_one();
            lkC.unlock();
            
            stage.run(std::move(val), index, pipeline);
            
            if (output != nullptr) {
                std::unique_lock<std::mutex> lkN(*mN);
                cvC->wait(lkN, [output]{
                    // block until the following conditions are met:
                    return
                        // the output size is less than one
                        // that is, if output contains 1 or more items,
                        // then we wait until output has processed enough of its items
                        // such that it has 0 items
                        // note that because of this, output can never contain 2 or more items
                        // because we only add an item if output contains 0 items
                        // eg if output has 0 items, we push, output now has 1 item
                        // if output has 1 item, we wait for it to process that item,
                        //   before pushing another item
                        // output processes the item, output now has 0 items
                        output->size() < 1;
                });
                output->push_back(val);
                cvN->notify_one();
                lkN.unlock();
            }
        }
    };

    std::deque<std::thread> pool;
    std::deque<std::deque<T>> queues;
    std::deque<std::condition_variable*> conditions;
    std::deque<std::mutex*> mutexes;
    std::deque<std::atomic<bool>*> halts;

    void add(Task task) {
        Stage anomynous_stage;
        anomynous_stage.run = task;
        stages.push_back(anomynous_stage);
    }

    void add(Stage stage) {
        stages.push_back(stage);
    }
    
    // in order to support address increment we will need to support two things:
    // 1. obtaining the current value in lambda from the current stage
    // 2. 
    
    std::deque<T> memory;
    int cycle = 0;
    int PC = 0;
    T * CIR = 0;
    int MAR = 0;
    T * MDR = 0;
    int ACC = 0;
    
    Pipeline & run() {
        std::string fmt = "[%TIMESINCESTART] [%logger:";
        fmt += sequential ? "sequential" : "pipelined";
        fmt += ":%level] %msg";
        config.setGlobally(el::ConfigurationType::Format, fmt);
        el::Loggers::reconfigureLogger("pipeline", config);
        
        program_start.mark();

        auto s = instruction_memory->size();
        queues.push_back(std::deque<T>());
        
        for (int i = 0; i < s; i++)
            queues.front().push_back(std::move(instruction_memory->at(i)));
        CLOG(INFO, "pipeline") << queues[0];
        auto ss = stages.size();
        
        if (!sequential) {
            conditions.push_back(new std::condition_variable);
            conditions.push_back(new std::condition_variable);
            mutexes.push_back(new std::mutex);
            halts.push_back(new std::atomic<bool>{false});
            for (int i = 0; i < ss; i++) {
                queues.push_back(std::deque<T>());
                conditions.push_back(new std::condition_variable);
                mutexes.push_back(new std::mutex);
                halts.push_back(new std::atomic<bool>{false});
                pool.push_back(
                    std::thread(
                        i == 0 ? callbackA : callbackB, this, stages[i], i+1,
                        &queues[i], i+1 == ss ? nullptr : &queues[i+1],
                        halts[i], halts[i+1],
                        i == 0 ? nullptr : conditions[i-1], conditions[i], conditions[i+1], conditions[i+2],
                        mutexes[i], mutexes[i+1]
                    )
                );
            }
        } else {
            auto qs = queues[0].size();
            while(PC < qs) {
                for (int i = 0; i < ss; i++) {
                    queues.push_back(std::deque<T>());
                    if (i == 0) {
                        auto val = std::move(queues[0].at(PC));
                        stages[0].run(std::move(val), 0, this);
                        queues[1].push_back(val);
                    } else {
                        auto val = std::move(queues[i].front());
                        queues[i].pop_front();
                        stages[i].run(std::move(val), i, this);
                        if (i+1 != ss) queues[i+1].push_back(val);
                    }
                }
            }
        }
        return *this;
    }
    
    Pipeline & run(std::deque<T> * input) {
        instruction_memory = input;
        return run();
        }
    
    Pipeline & run(std::deque<T> input) {
        instruction_memory = &input;
        return run();
    }
    
    Pipeline & clear() {
        pool.clear();
        queues.clear();
        for (auto condition : conditions) delete condition;
        conditions.clear();
        for (auto mutex : mutexes) delete mutex;
        mutexes.clear();
        for (auto halt : halts) delete halt;
        halts.clear();
        return *this;
    }
    
    Pipeline & join() {
        if (!sequential) for (auto && thread : pool) thread.join();
        return clear();
    }
};

struct instructions {
    static const int load = 1;
    static const int add = 2;
    static const int store = 3;
    
    static const char * toString(int val) {
        #define returncase(val) case val : return #val
        switch(val) {
            returncase(load);
            returncase(add);
            returncase(store);
            default : return "unknown";
        };
        #undef returncase
    }
};

int main() {
    
    Pipeline<int> pipeline;
    pipeline.add(PipelineLambda(val, i, p) {
        CLOG(INFO, "pipeline") << "T" << i << ": fetch BEGIN " << val << " ";
        p->MAR = p->PC;
        p->MDR = &p->queues[0].at(p->MAR);
        p->CIR = p->MDR;
        // since we are not operating in binary, our layout is INSTR,ARG,INSTR,ARG,...
        // increase PC by 2 to reach the next instruction
        p->PC += 2;
        CLOG(INFO, "pipeline") << "T" << i << ": fetch END, " << val << " " << *p->CIR;
    });
    pipeline.add(PipelineLambda(val, i, p) {
        CLOG(INFO, "pipeline") << "T" << i << ": decode BEGIN " << val << " ";
        p->MAR = p->queues[0].at(p->MAR+1);
        p->MDR = &p->memory.at(p->MAR);
        CLOG(INFO, "pipeline") << "T" << i << ": decode END " << val << " ";
    });
    pipeline.add(PipelineLambda(val, i, p) {
        CLOG(INFO, "pipeline") << "T" << i << ": execute BEGIN, " << val << " "
            << "pipeline memory: " << p->memory << ", ACC: " << p->ACC;
        switch(*p->CIR) {
            case instructions::load: {
                p->ACC = *p->MDR;
                break;
            }
            case instructions::add: {
                // add instruction is passed to ALU
                // contents of accumulator are moved to another place ready to be worked with
                int tmp = p->ACC;
                p->ACC = *p->MDR;
                
                // ALU add ACC and tmp together and store it in ACC
                p->ACC += tmp;
                break;
            }
            case instructions::store: {
                *p->MDR = p->ACC;
                break;
            }
            default: break;
        }
        CLOG(INFO, "pipeline") << "T" << i << ": execute END,   " << val << " "
            << "pipeline memory: " << p->memory << ", ACC: " << p->ACC;
    });
    
    pipeline.sequential = false;
    pipeline.manual_increment = true;
    
    pipeline.instruction_memory = {
        // load the contents of memory location of 0 into the accumulator
        instructions::load, 0,
        // add the contents of memory location 1 to what ever is in the accumulator
        instructions::add, 1,
        // store what ever is in the accumulator back back into location 2
        instructions::store, 2
    };
    
    pipeline.data_memory = {
        1,
        2,
        0
    };
    
    pipeline.run().join();
    return 0;
}
