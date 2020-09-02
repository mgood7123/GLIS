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

#define PipelineLambda(val, index) [] (auto && val, int index)

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
    
    // move the input instead of copying it
    // this allows for input mutation
    typedef std::function<void(T && val, int index)> Task;
    
    struct Stage {
        Task pre = nullptr;
        Task run = nullptr;
        Task post = nullptr;
    };
    
    std::deque<Stage> stages;

    #define PipelineLambdaArguments const Stage & stage, int index, std::queue<T> * input, std::queue<T> * output, std::atomic<bool> * haltC, std::atomic<bool> * haltN, std::condition_variable * cvP, std::condition_variable * cvC, std::condition_variable * cvN, std::condition_variable * cvF, std::mutex * mC, std::mutex * mN
    
    typedef std::function<void(PipelineLambdaArguments)> TaskCallback;
    
    #define PipelineLambdaCallback [] (PipelineLambdaArguments)

    TaskCallback callbackA = PipelineLambdaCallback {
        while(!input->empty()) {
            auto val = std::move(input->front());
            input->pop();
            
            stage.run(std::move(val), index);

            if (output != nullptr) {
                std::unique_lock<std::mutex> lkN(*mN);
                cvC->wait(lkN, [output]{ return output->size() < 1;});
                output->push(val);
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
            std::unique_lock<std::mutex> lkC(*mC);
            cvC->wait(lkC, [input, haltC]{return !input->empty() || (haltC->load() && input->empty());});
            if (haltC->load() && input->empty()) {
                std::unique_lock<std::mutex> lkN(*mN);
                haltN->store(true);
                cvN->notify_one();
                lkN.unlock();
                break;
            }
            auto val = std::move(input->front());
            input->pop();
            cvP->notify_one();
            lkC.unlock();
            
            stage.run(std::move(val), index);
            
            if (output != nullptr) {
                std::unique_lock<std::mutex> lkN(*mN);
                cvC->wait(lkN, [output]{ return output->size() < 1;});
                output->push(val);
                cvN->notify_one();
                lkN.unlock();
            }
        }
    };

    std::deque<std::thread> pool;
    std::deque<std::queue<T>> queues;
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
    
    Pipeline & run(std::deque<T> * input) {
        std::string fmt = "[%TIMESINCESTART] [%logger:";
        fmt += sequential ? "sequential" : "pipelined";
        fmt += ":%level] %msg";
        config.setGlobally(el::ConfigurationType::Format, fmt);
        el::Loggers::reconfigureLogger("pipeline", config);
        
        program_start.mark();

        auto s = input->size();
        queues.push_back(std::queue<T>());
        
        for (int i = 0; i < s; i++)
            queues.front().push(std::move(input->at(i)));
        
        auto ss = stages.size();
        
        if (!sequential) {
            conditions.push_back(new std::condition_variable);
            conditions.push_back(new std::condition_variable);
            mutexes.push_back(new std::mutex);
            halts.push_back(new std::atomic<bool>{false});
            for (int i = 0; i < ss; i++) {
                queues.push_back(std::queue<T>());
                conditions.push_back(new std::condition_variable);
                mutexes.push_back(new std::mutex);
                halts.push_back(new std::atomic<bool>{false});
                pool.push_back(
                    std::thread(
                        i == 0 ? callbackA : callbackB, stages[i], i+1,
                        &queues[i], i+1 == ss ? nullptr : &queues[i+1],
                        halts[i], halts[i+1],
                        i == 0 ? nullptr : conditions[i-1], conditions[i], conditions[i+1], conditions[i+2],
                        mutexes[i], mutexes[i+1]
                    )
                );
            }
        } else {
            while(!queues[0].empty()) {
                for (int i = 0; i < ss; i++) {
                    queues.push_back(std::queue<T>());
                    auto val = std::move(queues[i].front());
                    queues[i].pop();
                    stages[i].run(std::move(val), i);
                    if (i+1 != ss) queues[i+1].push(val);
                }
            }
        }
        return *this;
    }
    
    Pipeline & run(std::deque<T> input) {
        return run(&input);
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

int main() {
    Pipeline<int> pipeline;
    pipeline.add(PipelineLambda(val, i) {
        CLOG(INFO, "pipeline") << "T" << i << ": fetch BEFORE";
        usleep(1000000);
        CLOG(INFO, "pipeline") << "T" << i << ": fetch AFTER";
    });
    pipeline.add(PipelineLambda(val, i) {
        CLOG(INFO, "pipeline") << "T" << i << ": decode BEFORE";
        usleep(1000000);
        CLOG(INFO, "pipeline") << "T" << i << ": decode AFTER";
    });
    pipeline.add(PipelineLambda(val, i) {
        CLOG(INFO, "pipeline") << "T" << i << ": execute BEFORE";
        usleep(1000000);
        CLOG(INFO, "pipeline") << "T" << i << ": execute AFTER";
        val = 95*i/3;
    });
    pipeline.add(PipelineLambda(val, i) {
        CLOG(INFO, "pipeline") << "T" << i << ": write-back BEFORE";
        usleep(1000000);
        CLOG(INFO, "pipeline") << "T" << i << ": write-back AFTER (" << val << ")";
    });
    pipeline.run({0,1,2,3,4}).join();
    return 0;
}
