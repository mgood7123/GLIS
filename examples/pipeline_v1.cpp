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

el::Logger* pipelineLogger = el::Loggers::getLogger("pipeline");

timeSince program_start;

std::string getTime(const el::LogMessage* message) {
    return program_start.elapse().time;
}

int main() {
    std::atomic<bool> halt {false}, ready {false};
    std::condition_variable cvA, cvB;
    std::mutex m, streamLock;

    #define Lambda [] (std::queue<int> * input, std::queue<int> * output, std::atomic<bool> * halt, std::condition_variable * cvA, std::condition_variable * cvB, std::mutex * m, std::atomic<bool> * ready, std::mutex * streamLock)
    
    el::Helpers::installCustomFormatSpecifier(el::CustomFormatSpecifier("%TIMESINCESTART", getTime));
    el::Configurations defaultConf;
    defaultConf.setToDefault();
    defaultConf.setGlobally(el::ConfigurationType::Format, "[%TIMESINCESTART] [%logger:%level] %msg");
    el::Loggers::reconfigureLogger("pipeline", defaultConf);
    
    auto A =
    Lambda {
        while(!input->empty()) {
            auto val = std::move(input->front());
            input->pop();
            
            CLOG(INFO, "pipeline") << "T1: computing";
            
            usleep(1000000);

            CLOG(INFO, "pipeline") << "T1: " << val;

            std::unique_lock<std::mutex> lk(*m);
            CLOG(INFO, "pipeline") << "T1: BEFORE output->size: " << output->size();
            cvA->wait(lk, [output]{ return output->size() < 1;});
            CLOG(INFO, "pipeline") << "T1: AFTER output->size: " << output->size();
            CLOG(INFO, "pipeline") << "T1: pushing";
            output->push(val);
            cvB->notify_one();
            lk.unlock();
        }
        std::unique_lock<std::mutex> lk(*m);
        halt->store(true);
        cvB->notify_one();
        lk.unlock();
    };
    
    auto B =
    Lambda {
        while(true) {
            std::unique_lock<std::mutex> lk(*m);
            cvB->wait(lk, [input, halt]{return !input->empty() || (halt->load() && input->empty());});
            if (halt->load() && input->empty()) break;
            auto val = std::move(input->front());
            CLOG(INFO, "pipeline") << "T2: popping";
            input->pop();
            cvA->notify_one();
            lk.unlock();

            CLOG(INFO, "pipeline") << "T2: computing";
            
            usleep(1000000);
            
            CLOG(INFO, "pipeline") << "T2: " << val;
        }
    };
    
    std::queue<int> in, out;
    for(int i = 0; i < 8; i++) in.push(i);
    
    program_start.mark();
    
    auto T1 = std::thread(A, &in, &out,     &halt, &cvA, &cvB, &m, &ready, &streamLock);
    auto T2 = std::thread(B, &out, nullptr, &halt, &cvA, &cvB, &m, &ready, &streamLock);
    
    T2.join();
    T1.join();
    return 0;
}
