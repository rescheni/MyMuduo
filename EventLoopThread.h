#pragma once 



#include <functional>
#include <mutex>
#include <condition_variable>
#include <string>

#include "EventLoop.h"
#include "noncopyable.h"
#include "Thread.h"





namespace mymuduo
{

    class EventLoop;

    class EventLoopThread : noncopyable
    {
        public:
            using ThreadInitCallback = std::function<void(EventLoop*)>;
        public:
            EventLoopThread(const ThreadInitCallback & cb = ThreadInitCallback(),
                            const std::string  &name = std::string());
            ~EventLoopThread();

            EventLoop* startLoop();
        private:
            void threadFunc();

            EventLoop * loop_;
            bool exiting_;
            Thread thread_;
            std::mutex mutex_;
            std::condition_variable cond_;
            ThreadInitCallback callback_;
    };

    
} // namespace mymuduo
