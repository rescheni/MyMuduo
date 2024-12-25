#pragma once


#include <functional>
#include <string>
#include <vector>
#include <memory>


#include "noncopyable.h"

namespace mymuduo
{

    class EventLoop;
    class EventLoopThread;

    class EventLoopThreadPool
    {

        public:
            using ThreadInitCallback = std::function<void(EventLoop*)>;
        public:
            EventLoopThreadPool(EventLoop * baseLoop, const std::string &nameArgs);
            ~EventLoopThreadPool();


            void setThreadNum(int numberThreads){numThreads_ = numberThreads;}

            void start(const ThreadInitCallback & cb = ThreadInitCallback());
            

            //  如果 工作在多线程中 baseloop 可以用轮询的方式 分配给 subloop
            EventLoop* getNextloop();

            std::vector<EventLoop*> getAllLoops();

            bool started () const {return started_;}

            const std::string name() const {return name_;}

        private:
            EventLoop * baseLoop_;   // EventLoop loop;   一个线程
            std::string name_;
            bool started_;
            int numThreads_;
            int next_;
            std::vector<std::unique_ptr<EventLoopThread>> thread_;      // 包含所有创建事件的线程
            std::vector<EventLoop*> loops_;                             // 包含所有创建事件线程的指针
    };



} // namespace mymuduo