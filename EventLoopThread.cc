#include "EventLoopThread.h"
#include "logger.h"


namespace mymuduo
{

    EventLoopThread::EventLoopThread(const ThreadInitCallback & cb,
                    const std::string  &name)
                    : loop_(nullptr)
                    , exiting_(false)
                    , thread_(std::bind(&EventLoopThread::threadFunc, this),name)
                    , mutex_()
                    , cond_()
                    , callback_(cb)
    {
    }

    EventLoopThread::~EventLoopThread()
    {
        exiting_ = true;
        if(loop_ != nullptr)
        {
            loop_->quit();
            thread_.join();
        }
    }
    EventLoop* EventLoopThread::startLoop()
    {
        thread_.start();        // 启动底层的线程
        LOG_DEBUG(" EventLoopThread::startLoop()  底层线程池启动成功");
        EventLoop *loop = nullptr;
        {
            std::unique_lock<std::mutex> lock(mutex_);
            while(loop_ == nullptr)
            {
                LOG_DEBUG("cond_.wait(lock)ing \n");
                cond_.wait(lock);
                LOG_DEBUG("cond_.wait(lock)ing OK!!! \n");
            }
            loop = loop_;
        }   

        return loop;

    }
    // 下面的 in new  thread
    void EventLoopThread::threadFunc()
    {
        EventLoop loop;     // 创建一个 独立的线程 和上面的是 一一对应的，One  loop per Thread

        if(callback_)
        {
            callback_(&loop);
        }

        {
            std::unique_lock<std::mutex> lock(mutex_);
            loop_ = &loop;
            cond_.notify_one();
        }
        loop.loop();        // EventLoop loop => Poller.poll
        std::unique_lock<std::mutex> lock(mutex_);
        loop_ = nullptr;
    }

    
} // namespace mymuduo
