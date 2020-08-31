#include <iostream>
#include <functional>
#include <thread>
#include <atomic>
#include <mutex>
#include <unistd.h>
#include <queue>
#include <condition_variable>
#include <chrono>

struct TimeSince {
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
    
    const TimeSince & elapse() {

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
    
    friend std::ostream &operator<<(std::ostream & output, const TimeSince & D) {
        output << const_cast<TimeSince &>(D).elapse().time;
        return output;
    }
};

#define ELPP_THREAD_SAFE
#define ELPP_FORCE_USE_STD_THREAD
#define ELPP_STL_LOGGING

#include "easyloggingpp/src/easylogging++.h"
#include "easyloggingpp/src/easylogging++.cc"

INITIALIZE_EASYLOGGINGPP

#define PipelinePrintValue(value) #value << ": " << value
#define PipelinePrintBool(boolean) #boolean << ": " << (boolean ? "true" : "false")
#define PipelinePrint CLOG(INFO, "pipeline")
#define PipelineFPrint(type) CLOG(type, "pipeline")
#define PipelinePrintStage(stage) PipelinePrint << "[Stage " << stage << "] "
#define PipelineFPrintStage(type, stage) PipelineFPrint(type) << "[Stage " << stage << "] "

#include <iostream>
#include <functional>
#include <deque>
#include <cassert>

#define PipelineLambda(val, index, pipeline) [] (auto && val, int index, auto * pipeline, auto * input, auto * output)

#define PipelineCycleLambda(pipeline) [] (auto * pipeline)

TimeSince program_start;

struct Electronics {
    template <typename T, T INITIALIZER>
    struct SignalEdgeDetector {
        T signalStored = INITIALIZER;
        
        bool is_rise(T signal) {
            bool ret = false;
            if (signalStored == 0 && signal == 1) ret = true;
            signalStored = signal;
            return ret;
        }

        bool is_fall(T signal) {
            return !is_rise(signal);
        }
    };
    
    // this is a Flip-Flop, specifically a Type D ("Data" or "Delay") Flip-Flop
    
    template <typename T, T INITIALIZER>
    struct Flop {
        T input = INITIALIZER;
        T output = INITIALIZER;
        bool hasOutput = false;
        bool hasInput = false;
        void push_input(T && in) {
            input = std::move(in);
            hasInput = true;
        }
        void push_output(T && out) {
            output = std::move(out);
            hasOutput = true;
        }
        T pull_input() {
            T in = std::move(input);
            hasInput = false;
            return std::move(in);
        }
        T pull_output() {
            T out = std::move(output);
            hasOutput = false;
            return std::move(out);
        }
        void exec() {
            push_output(std::move(pull_input()));
        }
    };
};

struct PipelineStageTypes {
    static const int Undefined = -1;
    static const int Stage = 0;
    static const int Flop = 1;
};

template <typename T, T INITIALIZER>
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
    typedef std::function<void(T && val, int index, Pipeline<T, INITIALIZER> * pipeline, std::deque<T> * input, std::deque<T> * output)> Task;
    
    typedef std::function<void(Pipeline * pipeline)> Cycle;
    
    Cycle cycleFuncNoop = [] (Pipeline * pipeline) {};
    Cycle cycleFunc = cycleFuncNoop;

    struct Stage {
        int type = PipelineStageTypes::Undefined;

        std::deque<T> output;

        Task pre = nullptr;
        Task run = nullptr;
        Task post = nullptr;
        struct Electronics::Flop<T, INITIALIZER> flop;
    };
    
    std::deque<Stage> stages;
    
    #define PipelineLambdaArguments Pipeline * pipeline, const Stage & stage, int index, std::deque<T> * input, std::deque<T> * output, std::atomic<bool> * haltC, std::atomic<bool> * haltN, std::condition_variable * cvP, std::condition_variable * cvC, std::condition_variable * cvN, std::condition_variable * cvF, std::mutex * mC, std::mutex * mN, Cycle cycle
    
    typedef std::function<void(PipelineLambdaArguments)> TaskCallback;
    
    #define PipelineLambdaCallback [] (PipelineLambdaArguments)
    
    struct Functions {
        Pipeline<T, INITIALIZER> * pipeline = nullptr;
        
        size_t instruction_length = 0;
        
        int index_of_prev_stage = 0;
        int index_of_this_stage = 0;
        int index_of_next_stage = 0;
        
        Stage * prev_stage = nullptr;
        Stage * this_stage = nullptr;
        Stage * next_stage = nullptr;
        
        Electronics::Flop<T, INITIALIZER> * prev_flip_flop = nullptr;
        Electronics::Flop<T, INITIALIZER> * this_flip_flop = nullptr;
        Electronics::Flop<T, INITIALIZER> * next_flip_flop = nullptr;
        
        bool prev_stage_is_flip_flop = false;
        bool this_stage_is_flip_flop = false;
        bool next_stage_is_flip_flop = false;
        
        Functions(Pipeline<T, INITIALIZER> * pipeline) {
            this->pipeline = pipeline;
        }

        Functions(Pipeline<T, INITIALIZER> * pipeline, const int & index) {
            this->pipeline = pipeline;
            aquire_indexes_stages_and_flip_flops(index);
        }
        
        void aquire_indexes_stages_and_flip_flops(const int & index) {
            index_of_prev_stage = index-1;
            index_of_this_stage = index;
            index_of_next_stage = index+1;
            
            if (index_of_prev_stage != -1) {
                prev_stage = &pipeline->stages[index_of_prev_stage];
                prev_flip_flop = &prev_stage->flop;
                prev_stage_is_flip_flop = prev_stage->type == PipelineStageTypes::Flop;
            };
            
            this_stage = &pipeline->stages[index_of_this_stage];
            this_flip_flop = &this_stage->flop;
            this_stage_is_flip_flop = this_stage->type == PipelineStageTypes::Flop;
            
            if (index_of_next_stage < pipeline->stages.size()) {
                next_stage = &pipeline->stages[index_of_next_stage];
                next_flip_flop = &next_stage->flop;
                next_stage_is_flip_flop = next_stage->type == PipelineStageTypes::Flop;
            }
        }
        
        void store_instruction_length(std::deque<T> * input) {
            instruction_length = input->size();
        }
        
        bool program_counter_is_greater_than_instruction_length() {
            return pipeline->PC[0] > (instruction_length-1);
        }
        
        std::string prev_stage_as_string() {
            return std::move(std::string("Stage ") + std::to_string(index_of_prev_stage));
        }

        std::string this_stage_as_string() {
            return std::move(std::string("Stage ") + std::to_string(index_of_this_stage));
        }
        
        std::string next_stage_as_string() {
            return std::move(std::string("Stage ") + std::to_string(index_of_next_stage));
        }
        
        
        void lock(std::unique_lock<std::mutex> & unique_lock) {
            using namespace std::chrono_literals;
            auto start = std::chrono::high_resolution_clock::now();
            auto end = start + 1s;
            do {
                try {
                    unique_lock.lock();
                    return;
                } catch (const std::system_error& e) {
                    PipelinePrintStage(index_of_this_stage)
                        << "Failed to lock: " << e.what();
                    std::this_thread::sleep_for(250ms);
                }
            } while(std::chrono::high_resolution_clock::now() < end);
            PipelineFPrintStage(FATAL, index_of_this_stage) << std::endl << std::endl
                << "Failed to aquire a lock within 1 second." << std::endl
                << "There is likely a hang somewhere." << std::endl
                << "For example, a conditional wait not being satisfied" << std::endl;
        }
    };
    
    TaskCallback callback = PipelineLambdaCallback {
        
        Functions functions(pipeline);
        
        functions.aquire_indexes_stages_and_flip_flops(index);
        
        PipelinePrintStage(functions.index_of_this_stage) << "STARTED";
        
        if (functions.index_of_this_stage == 0)
            functions.store_instruction_length(input);
        
        // dummy var until i get around to
        // removing this parameter from callback
        int val = 0;
        
        bool should_exit = false;
        bool this_input_is_not_empty;
        bool next_input_is_empty;
        
        // create locks
        // pass std::defer_lock to avoid locking during initialization
        //
        // NOTE: unique lock_next is this_unique_lock of the next stage
        //
        std::unique_lock<std::mutex> this_unique_lock(*mC, std::defer_lock);
        std::unique_lock<std::mutex> next_unique_lock(*mN, std::defer_lock);
        
        while(!should_exit) {
            
            // this section handles stage execution
            {
                // if this stage is the very first stage we can immediately execute the stage
                // otherwise we need to wait until we have input
                
                PipelinePrintStage(functions.index_of_this_stage) << "locking this mutex";
                functions.lock(this_unique_lock);
                PipelinePrintStage(functions.index_of_this_stage) << "locked this mutex";
                
                PipelinePrintStage(functions.index_of_this_stage)
                    << "waiting on this_input_is_not_empty";
                cvC->wait(this_unique_lock, [&this_input_is_not_empty, &functions, input] {

                    // NOTE: pred specifies that a value of `false` should be returned
                    //       if we should continue to wait
                    
                    {
                        // check the normal input
                        this_input_is_not_empty = !input->empty();
                    }
                    
                    // return true if the condition true, in order to stop waiting
                    // otherwise, return false to keep waiting
                    return this_input_is_not_empty;
                });
                PipelinePrintStage(functions.index_of_this_stage)
                    << "waited on this_input_is_not_empty";

                PipelinePrintStage(functions.index_of_this_stage) << "unlocking this mutex";
                this_unique_lock.unlock();
                PipelinePrintStage(functions.index_of_this_stage) << "unlocked this mutex";
                
                PipelinePrintStage(functions.index_of_this_stage) << PipelinePrintBool(this_input_is_not_empty);
                
                if (functions.this_stage_is_flip_flop) {
                    // if this stage is a flip flop
                    // we move the input into the flip-flop
                    // and then we move the flip-flop's input into its output
                    
                    PipelinePrintStage(functions.index_of_this_stage) << "pushing flip-flop input";
                    functions.this_flip_flop->push_input(std::move(input->front()));
                    PipelinePrintStage(functions.index_of_this_stage) << "pushed flip-flop input";
                    
                    // consume input
                    PipelinePrintStage(functions.index_of_this_stage) << "consuming normal input";
                    input->pop_front();
                    PipelinePrintStage(functions.index_of_this_stage) << "consumed normal input";
                    
                    // check if we have a previous stage
                    if (functions.prev_stage != nullptr) {
                        // tell our previous stage that next output (this input) is ready
                        cvP->notify_one();
                    }
                    
                    
                    PipelinePrintStage(functions.index_of_this_stage)
                        << "functions.this_flip_flop->input: " << functions.this_flip_flop->input << ", "
                        << "functions.this_flip_flop->output: " << functions.this_flip_flop->output;
                    
                    PipelinePrintStage(functions.index_of_this_stage) << "pushing flip-flop output and consuming flip-flop input";
                    functions.this_flip_flop->exec();
                    PipelinePrintStage(functions.index_of_this_stage) << "pushed flip-flop output and consumed flip-flop input";
                    
                    PipelinePrintStage(functions.index_of_this_stage)
                        << "functions.this_flip_flop->input: " << functions.this_flip_flop->input << ", "
                        << "functions.this_flip_flop->output: " << functions.this_flip_flop->output;
                    
                    // check if we have a previous stage
                    if (functions.prev_stage != nullptr) {
                        // tell our previous stage that next output (this input) is ready
                        cvP->notify_one();
                    }
                } else {
                    // if this stage is not a flip flop, then execute the current stage
                    
                    // if the previous stage is a flip-flop
                    // then we need to pass it into our current input
                    if (functions.prev_stage_is_flip_flop) {
//                         input->push_back(std::move(functions.prev_flip_flop->pull_output()));
                    }
                    
                    // TODO: remove the val parameter of run `std::move(val)`
                    stage.run(std::move(val), index, pipeline, input, output);
                    
                    // invoke a cycle end
                    //
                    // NOTE: the pipeline run function automatically assigns
                    //       a NO-OP function to ALL stages except for the LAST stage
                    //
                    cycle(pipeline);
                    
                    // check if we have a previous stage
                    if (functions.prev_stage != nullptr) {
                        // pop our input so that the previous stage can process again
                        PipelinePrintStage(functions.index_of_this_stage) << "consuming normal input";
                        input->pop_front();
                        PipelinePrintStage(functions.index_of_this_stage) << "consumed normal input";
                        // tell our previous stage that next output (this input) is ready
                        cvP->notify_one();
                    }
                }
                
                if (functions.next_stage != nullptr) {
                    // we have a next stage
                    if (functions.this_stage_is_flip_flop) {
                        // if this stage is a flip-flop
                        // push the flip-flop output to the input of the next stage
                        PipelinePrintStage(functions.index_of_this_stage)
                            << "consuming flip-flop output and pushing normal output";
                        output->push_back(std::move(functions.this_flip_flop->pull_output()));
                        PipelinePrintStage(functions.index_of_this_stage)
                            << "consumed flip-flop output and pushed normal output";
                    }
                    // notify next stage that we have input
                    cvN->notify_one();
                    // and wait for our output to be consumed before we continue
                    
                    PipelinePrintStage(functions.index_of_this_stage) << "locking next mutex";
                    functions.lock(next_unique_lock);
                    PipelinePrintStage(functions.index_of_this_stage) << "locked next mutex";
                    
                    PipelinePrintStage(functions.index_of_this_stage)
                        << "waiting on next_input_is_empty";
                    // NOTE: the next stage's input is this stage's output
                    cvC->wait(next_unique_lock, [&next_input_is_empty, &functions, output, haltC] {
                        if (haltC->load()) return true;

                        // NOTE: pred specifies that a value of `false` should be returned
                        //       if we should continue to wait
                        
                        {
                            // the next stage will consume our normal output
                            // regardless of the stage type
                            PipelinePrintStage(functions.index_of_this_stage)
                                << PipelinePrintBool(output->empty());
                            
                            next_input_is_empty = output->empty();
                        }
                        
                        // return true if the condition true, in order to stop waiting
                        // otherwise, return false to keep waiting
                        return next_input_is_empty;
                    });
                    
                    if (haltC->load()) {
                        PipelinePrintStage(functions.index_of_this_stage) "goto halt";
                        goto halt;
                    }
                    
                    PipelinePrintStage(functions.index_of_this_stage)
                        << "waited on next_input_is_empty";
                    PipelinePrintStage(functions.index_of_this_stage) << PipelinePrintBool(next_input_is_empty);
                    
                    PipelinePrintStage(functions.index_of_this_stage) << "unlocking next mutex";
                    next_unique_lock.unlock();
                    PipelinePrintStage(functions.index_of_this_stage) << "unlocked next mutex";
                }
            }
            
            // this section handles the HALT signal
            {
                halt:
                if (functions.index_of_this_stage == 0) {
                    // if the current stage is the very first stage
                    
                    if (functions.program_counter_is_greater_than_instruction_length()) {
                        // and we have reached the end of our instructions
                        
                        // then simply send the HALT signal to the next stage
                        PipelinePrintStage(functions.index_of_this_stage) << "locking next mutex";
                        functions.lock(next_unique_lock);
                        PipelinePrintStage(functions.index_of_this_stage) << "locked next mutex";
                        PipelinePrintStage(functions.index_of_this_stage) << "HALTING";
                        haltN->store(true);
                        cvN->notify_one();
                        PipelinePrintStage(functions.index_of_this_stage) << "unlocking next mutex";
                        next_unique_lock.unlock();
                        PipelinePrintStage(functions.index_of_this_stage) << "unlocked next mutex";
                        // and then exit the loop
                        should_exit = true;
                    }
                    // otherwise do nothing and loop back to consume more instructions
                } else {
                    // otherwise, the current stage is NOT the very first stage
                    // so we need to probogate the halt signal to the next stage
                    
                    PipelinePrintStage(functions.index_of_this_stage) << "locking this mutex";
                    functions.lock(this_unique_lock);
                    PipelinePrintStage(functions.index_of_this_stage) << "locked this mutex";
                    
                    // we need to progate the HALT signal under these conditions:
                    // condition 1. our input must be empty
                    bool input_is_empty;
                    // condition 2: we have actually recieved a HALT signal
                    bool recieved_halt;
                    if (functions.this_stage_is_flip_flop) {
                        // if this is a flip-flop, then check its input
                        PipelinePrintStage(functions.index_of_this_stage)
                            << PipelinePrintBool(functions.this_flip_flop->hasInput);
                        input_is_empty = functions.this_flip_flop->hasInput;
                        if (!input_is_empty) {
                            // the flip-flop also consumes normal input
                            // and stores it in its input
                            PipelinePrintStage(functions.index_of_this_stage)
                                << PipelinePrintBool(input->empty());
                            input_is_empty = input->empty();
                        }
                    } else {
                        // if this is not a flip-flop, check the normal input
                        PipelinePrintStage(functions.index_of_this_stage)
                            << PipelinePrintBool(input->empty());
                        input_is_empty = input->empty();
                    }
                    recieved_halt = haltC->load();
                    PipelinePrintStage(functions.index_of_this_stage)
                        << PipelinePrintBool(input_is_empty);
                    PipelinePrintStage(functions.index_of_this_stage)
                        << PipelinePrintBool(recieved_halt);
                    
                    PipelinePrintStage(functions.index_of_this_stage) << "unlocking this mutex";
                    this_unique_lock.unlock();
                    PipelinePrintStage(functions.index_of_this_stage) << "unlocked this mutex";
                    
                    PipelinePrintStage(functions.index_of_this_stage) << PipelinePrintBool(recieved_halt);
                    
                    PipelinePrintStage(functions.index_of_this_stage) << PipelinePrintBool(input_is_empty);
                    if (input_is_empty) cvP->notify_one();
                    
                    // check if our halt condition has been satisfied
                    if (recieved_halt && input_is_empty) {
                        // our halt condition has been satisfied
                        // probogate it to the next stage
                        PipelinePrintStage(functions.index_of_this_stage) << "locking next mutex";
                        functions.lock(next_unique_lock);
                        PipelinePrintStage(functions.index_of_this_stage) << "locked next mutex";
                        PipelinePrintStage(functions.index_of_this_stage) << "HALTING";
                        haltN->store(true);
                        cvN->notify_one();
                        PipelinePrintStage(functions.index_of_this_stage) << "unlocking next mutex";
                        next_unique_lock.unlock();
                        PipelinePrintStage(functions.index_of_this_stage) << "unlocked next mutex";
                        
                        // and then break
                        should_exit = true;
                    }
                    // our halt condition has not been satisfied
                    // we either have not halted
                    // or we still have more input to process
                }
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
        anomynous_stage.type = PipelineStageTypes::Stage;
        stages.push_back(anomynous_stage);
    }

    void add(Stage stage) {
        stages.push_back(stage);
    }
    
    void addFlop() {
        Stage anomynous_stage;
        anomynous_stage.type = PipelineStageTypes::Flop;
        stages.push_back(anomynous_stage);
    }

    Electronics::Flop<T, INITIALIZER> & getInputFlopForStage(int stageIndex) {
        auto ss = stages.size();
        for (int i = stageIndex; i < ss && i > -1; i--) {
            if (stages[i].type == PipelineStageTypes::Flop) {
                return stages[i].flop;
            }
        }
    }
    
    std::deque<Electronics::Flop<T, INITIALIZER> *> getInputFlopsForStage(int stageIndex) {
        std::deque<Electronics::Flop<T, INITIALIZER> *> flops;
        auto ss = stages.size();
        for (int i = stageIndex; i < ss && i > -1; i--) {
            if (stages[i].type == PipelineStageTypes::Flop) {
                flops.push_back(&stages[i].flop);
            } else break;
        }
        return flops;
    }

    Electronics::Flop<T, INITIALIZER> & getOutputFlopForStage(int stageIndex) {
        auto ss = stages.size();
        for (int i = stageIndex; i < ss; i++) {
            if (stages[i].type == PipelineStageTypes::Flop) {
                return stages[i].flop;
            }
        }
    }
    
    std::deque<Electronics::Flop<T, INITIALIZER> *> getOutputFlopsForStage(int stageIndex) {
        std::deque<Electronics::Flop<T, INITIALIZER> *> flops;
        auto ss = stages.size();
        for (int i = stageIndex; i < ss; i++) {
            if (stages[i].type == PipelineStageTypes::Flop) {
                flops.push_back(&stages[i].flop);
            } else break;
        }
        return flops;
    }

    // in order to support address increment we will need to support two things:
    // 1. obtaining the current value in lambda from the current stage
    // 2. 
    
    std::deque<T> instruction_memory;
    std::deque<T> data_memory;
    
    void * externalData = nullptr;
    int * PC = nullptr;
    
    Pipeline & run() {
        std::string fmt = "[%TIMESINCESTART] [%logger:";
        fmt += sequential ? "sequential" : "pipelined";
        fmt += ":%level] %msg";
        config.setGlobally(el::ConfigurationType::Format, fmt);
        el::Loggers::reconfigureLogger("pipeline", config);
        
        program_start.mark();

        auto s = instruction_memory.size();
        queues.push_back(std::deque<T>());
        
        for (int i = 0; i < s; i++) queues.front().push_back(std::move(instruction_memory.at(i)));
        PipelinePrint << queues.front();
        
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
                        callback, this, stages[i], i,
                        &queues[i], i+1 == ss ? nullptr : &queues[i+1],
                        halts[i], halts[i+1],
                        i == 0 ? nullptr : conditions[i-1], conditions[i], conditions[i+1], conditions[i+2],
                        mutexes[i], mutexes[i+1], i+1 == ss ? cycleFunc : cycleFuncNoop
                    )
                );
            }
        } else {
            auto qs = queues.front().size();
            while(*PC < qs) {
                for (int i = 0; i < ss; i++) {
                    if (stages[i].type == PipelineStageTypes::Stage) {
                        int val = 0;
                        
                        std::deque<T> _input;
                        std::deque<T> * input = nullptr;
                        
                        if (i != 0) {
                            if (stages[i-1].type == PipelineStageTypes::Flop) {
                                input = &_input;
                                if (stages[i-1].flop.hasOutput) {
                                    input->push_front(stages[i-1].flop.output);
                                }
                            } else {
                                input = &stages[i-1].output;
                            }
                        }
                        
                        std::deque<T> * output = &stages[i].output;
                        
                        stages[i].run(std::move(val), i, this, i == 0 ? nullptr : input, output);
                        
                    } else {
                        PipelinePrint << "Flop encountered, executing due to sequential mode";
                        std::deque<T> * input  = i == 0 ? nullptr : &stages[i-1].output;
                        std::deque<T> * output = &stages[i].output;
                        if (i != 0) {
                            if (stages[i-1].type == PipelineStageTypes::Flop) {
                                stages[i].flop.input = stages[i-1].flop.output;
                            } else {
                                auto val = stages[i-1].output.front();
                                stages[i-1].output.pop_front();
                                stages[i].flop.input = val;
                            }
                        }
                        stages[i].flop.exec();
                    }
                    
                    i+1 == ss ? cycleFunc(this) : cycleFuncNoop(this);
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
        for (auto * condition : conditions) delete condition;
        conditions.clear();
        for (auto * mutex : mutexes) delete mutex;
        mutexes.clear();
        for (auto * halt : halts) delete halt;
        halts.clear();
        return *this;
    }
    
    Pipeline & join() {
        if (!sequential) for (auto && thread : pool) thread.join();
        return clear();
    }
};

struct Instructions {
    static const int load = 13;
    static const int add = 86;
    static const int store = 55;
    
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

void twoStagedPipeline(bool sequential) {
    Pipeline<int, 0> pipeline;
    
    // two-staged pipeline
    
    // during the cycle "stage" the flop is triggered, stores its input into its output, and then the cycle ends, at which point a new fetch begins
    
    struct registers {
        int clocktmp = 0;
        int clock = 0;
        int clock_last = 0;
        int PC = 0;
        int * CIR = 0;
        int * CIRPlusOne = 0;
        int MAR = 0;
        int * MDR = 0;
        int * MDRPlusOne = 0;
        int ACC = 0;
    } a;
    
    pipeline.externalData = &a;
    pipeline.PC = &a.PC;
    
    // in a 5 stage pipeline, PC is stored in a latch before being sent to fetch
    
    pipeline.cycleFunc = PipelineCycleLambda(p) {
        struct registers * reg = static_cast<struct registers*>(p->externalData);
        PipelinePrint << "--- clock: " << reg->clock << ": Cycle END             ---";
//         if ((++reg->clocktmp % 2) == 0) {
//             PipelinePrint << "--- clock: " << reg->clock << ": Cycle Sub-Stage BEGIN ---";
//             for (auto & stage : p->stages) if (stage.type == PipelineStageTypes::Flop) {
//                 PipelinePrint << "--- clock: " << reg->clock << ": Cycle Sub-Stage: processing Flop";
//                 stage.flop.exec();
//                 PipelinePrint << "--- clock: " << reg->clock << ": Cycle Sub-Stage: processed Flop";
//             }
//             PipelinePrint << "--- clock: " << reg->clock << ": Cycle Sub-Stage END   ---";
            reg->clock_last = reg->clock;
            reg->clock++;
//         }
        PipelinePrint << "--- clock: " << reg->clock << ": Cycle BEGIN           ---";
    };
    
//     pipeline.add(PipelineLambda(val, i, p) {
//         struct registers * reg = static_cast<struct registers*>(p->externalData);
//         output->push_back(reg->PC + 2);
//     });
    // TODO: pipeline.addFlop(number_of_flop_inputs, number_of_flop_outputs), PipelineFlopLambda(p, flop) {});
    // TODO: ability to name stages, and locate them by name
    // TODO: ability to manually specity inputs and outputs for next stage inside lambda
//     pipeline.addFlop();
    pipeline.add(PipelineLambda(val, i, p) {
        struct registers * reg = static_cast<struct registers*>(p->externalData);
        PipelinePrint << "Stage " << i << ": clock: " << reg->clock << ": fetch BEGIN, PC: " << reg->PC;
        reg->MAR = reg->PC;
        reg->MDR = &p->instruction_memory.at(reg->MAR);
        reg->MDRPlusOne = &p->instruction_memory.at(reg->MAR+1);
        reg->CIR = reg->MDR;
        reg->CIRPlusOne = reg->MDRPlusOne;
        reg->PC += 2;
        PipelinePrint << "Stage " << i << ": clock: " << reg->clock << ": fetch END";
        PipelinePrint << "Stage " << i << ": clock: " << reg->clock << ": decode BEGIN";
        reg->MAR = *reg->MDRPlusOne;
        reg->MDR = &p->data_memory.at(reg->MAR);
        PipelinePrint << "Stage " << i << ": clock: " << reg->clock << ": decode END: "
            << Instructions::toString(*reg->CIR);
        
        output->push_back(*reg->CIR);
        
        PipelinePrint << "Stage " << i << ": clock: " << reg->clock << ": output:" << *output;
    });
    pipeline.addFlop();
    pipeline.add(PipelineLambda(val, i, p) {
        struct registers * reg = static_cast<struct registers*>(p->externalData);
        PipelinePrint << "Stage " << i << ": clock: " << reg->clock << ": input:" << *input;
        PipelinePrint << "Stage " << i << ": clock: " << reg->clock << ": execute BEGIN, "
            << "pipeline memory: " << p->data_memory << ", ACC: " << reg->ACC;
        int o = input->front();
        switch(o) {
            case Instructions::load: {
                reg->ACC = *reg->MDR;
                PipelinePrintStage(i) << PipelinePrintValue(reg->ACC);
                break;
            }
            case Instructions::add: {
                // add instruction is passed to ALU
                // contents of accumulator are moved to another place ready to be worked with
                int tmp = reg->ACC;
                reg->ACC = *reg->MDR;
                
                // ALU add ACC and tmp together and store it in ACC
                reg->ACC += tmp;
                break;
            }
            case Instructions::store: {
                *reg->MDR = reg->ACC;
                break;
            }
            default: break;
        }
        PipelinePrint << "Stage " << i << ": clock: " << reg->clock << ": execute END,   "
            << "pipeline memory: " << p->data_memory << ", ACC: " << reg->ACC;
    });
//     pipeline.addFlop();
    
    pipeline.sequential = sequential;
    pipeline.manual_increment = true;
    
    pipeline.instruction_memory = {
        // load the contents of memory location of 0 into the accumulator
        Instructions::load, 0,
        // add the contents of memory location 1 to what ever is in the accumulator
//         Instructions::add, 1
        // store what ever is in the accumulator back back into location 2
        Instructions::store, 2
        // load the contents of memory location of 0 into the accumulator
//         instructions::load, 0,
        // add the contents of memory location 1 to what ever is in the accumulator
//         instructions::add, 1,
        // store what ever is in the accumulator back back into location 2
//         instructions::store, 2
    };
    
    pipeline.data_memory = {
        1,
        2,
        0
    };
    
    pipeline.run().join();
}

int main() {
    twoStagedPipeline(false);
    return 0;
}
