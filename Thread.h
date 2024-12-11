#pragma once 

#include <functional>
#include <thread>
#include <memory>
#include <unistd.h>
#include <string>
#include <atomic>

#include "noncopyable.h"

namespace mymuduo
{

    class Thread : noncopyable
    {  
        public:

            using ThreadFunc = std::function<void()>;

        public:
            explicit Thread(ThreadFunc,const std::string &name = std::string());
            ~Thread();

            void start();
            void join();

            bool started() const { return started_; }
            pid_t tid() const { return tid_; }
            const std::string & name() const { return name_; }
            
            static int numCreated() { return numCreated_; }

        private:
            void setDefaultName();

            bool started_;
            bool joined_;
            std::shared_ptr<std::thread> thread_;   // 
            pid_t tid_;                              // 线程id
            ThreadFunc func_;                       // 线程函数
            std::string name_;                       // 线程名称
            static std::atomic_int numCreated_;
    };

    
} // namespace mymuduo
