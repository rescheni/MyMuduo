#include "EventLoopThreadPool.h"
#include "EventLoopThread.h"
#include "logger.h"


namespace mymuduo
{
    EventLoopThreadPool::EventLoopThreadPool(EventLoop * baseLoop, const std::string &nameArgs)
        : baseLoop_(baseLoop)
        , name_(nameArgs)
        , started_(false)
        , numThreads_(0)
        , next_(0)      // 轮巡下标 
    {
    }

    EventLoopThreadPool::~EventLoopThreadPool()
    {
        // 源项目say ：     Don't delete loop it's  stack  variable 
    }


    void EventLoopThreadPool::start(const ThreadInitCallback & cb)
    {
        started_ = true;
        for(int i = 0;i < numThreads_; ++i)
        {
            LOG_DEBUG("DEBUG OUT =>  EventLoopThreadPool::start \n");
            char buffer[name_.size() + 32];
            snprintf(buffer ,sizeof buffer, "%s%d",name_.c_str(),i);        // 线程池的名称 + id 为某个线程的name
            // 创建 Event loop Thread
            EventLoopThread *t  = new EventLoopThread(cb,buffer); 
            thread_.push_back(std::unique_ptr<EventLoopThread>(t));
            LOG_DEBUG("DEBUG OUT => thread_.push_back OK ###### \n");

            loops_.push_back(t->startLoop());           // 底层创建线程  绑定一个新的 EventLoop  返回该 loop 的地址
            LOG_DEBUG("底层创建线程,绑定一个新的 EventLoop  返回该 loop 的地址 ###### OK \n");

        }

            LOG_DEBUG("*** numThreads = %d | cb != null %s ?  *** \n",numThreads_,(cb==nullptr?"Yes":"NO"));

        // 整个服务 只有一个线程  运行者 baseloop
        if(numThreads_ == 0 && cb)
        {
            LOG_DEBUG("整个服务 只有一个线程  运行者 baseloop    \n");
            cb(baseLoop_);
        }

    }

    // 使用轮询的方式 将baseloops  分配给 channel  和 subloop
    EventLoop* EventLoopThreadPool::getNextloop()
    {

        EventLoop * loop = baseLoop_;

        if(!loops_.empty())         // 轮询 分配
        {
            loop = loops_[next_];
            ++next_;
            if(next_ >= loops_.size())
            {
                next_ = 0;
            }
        }
        return loop;
    }

    std::vector<EventLoop*> EventLoopThreadPool::getAllLoops()
    {
        if(loops_.empty())
        {
            return std::vector<EventLoop*>(1,baseLoop_); 
        }else{
            return loops_;
        }
    }

} // namespace mymuduo
