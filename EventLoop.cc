#include <sys/eventfd.h>
#include <fcntl.h>
#include <unistd.h>
#include <cerrno>
#include <memory>


#include "EventLoop.h"
#include "Poller.h"
#include "Channel.h"
#include "logger.h"



namespace mymuduo{


    // 用于 防止 一个线程 创建 多个 EventLoop  thread_local
    __thread EventLoop * t_loopInThisThread = nullptr;


    // 定义了 默认的 poller 的 IO复用 的超时事件
    const int kPollTimeMs = 10000;


    // 创建 wakeupfd  用来 notify 唤醒subReactor 处理新来的 channel
    int createEventfd()
    {
        int evtfd = eventfd(0,EFD_NONBLOCK|EFD_CLOEXEC);

        if(evtfd < 0)
        {
            LOG_FATAL("Eventfd Error : %d \n",errno);
        }

        return evtfd;
    }


    EventLoop::EventLoop() 
        : looping_(false)
        , quit_(false)
        , callingPendingFunctors_(false)
        , thread_Id_(CurrentThread::tid())
        , poller_(Poller::newDefaultPoller(this))
        , wakeupFd_(createEventfd())
        , wakeupChannel_(new Channel(this,wakeupFd_)) 
        , currentActiveChannel_(nullptr)
    {
        LOG_DEBUG("EventLoop create %p in thread %d \n", this,thread_Id_);
        if(t_loopInThisThread)
        {
            LOG_FATAL("Another EventLoop %p exists in this thread %d \n",t_loopInThisThread,thread_Id_);
        }else{
            t_loopInThisThread = this;
        }

        // 设置 wakeupfd 的 事件类型 和 发生后的 回调操作
        wakeupChannel_->setReadCallback(std::bind(&EventLoop::handleRead,this));
        wakeupChannel_->enableReading();

    }
	EventLoop::~EventLoop(){ 
        wakeupChannel_->disableAll();
        wakeupChannel_->remove();
        close(wakeupFd_);
        t_loopInThisThread = nullptr;


    }






    // 开启事件循环
    void EventLoop::loop()
    {
        looping_ = true;
        quit_ = false;

        LOG_INFO("EventLoop %p start looping \n",this);
        while(!quit_)
        {
            activeChannels_.clear();
            // 监听两个fd 一种 client 的 fd 一种 wakeupfd
            pollerReturnTime_ = poller_->poll(kPollTimeMs,&activeChannels_);

            for(Channel * channel: activeChannels_)
            {
                // Poller 监听 哪些 channel 事件发生了，然后上报给 EventLoop，通知 channel 相应的事件
                channel->handleEvent(pollerReturnTime_);
            }
            // 执行当前EventLoop事件循环需要处理的回调操作
            doPendingFunctors();


        }

        LOG_INFO("EventLoop %p stop looping \n",this);
        looping_ = false ;
    }

    // 退出事件循环
    // loop 在 自己的线程中调用 quit 
    // loop 在 其他的线程中调用 quit
    void EventLoop::quit()
    {
        quit_ = true;

        if(!isInLoopThread())
        {
            wakeup();
        }
  
        quit_ = false;
    }


    // 在当前loop 中 执行 cb
    void EventLoop::runInLoop(Functor cb)
    {
        // 在当前的 loop 所在的线程中 执行callback
        if(isInLoopThread())
        {
            cb();
        }
        else{       // 在非当前线程 执行 cb | 需要 唤醒loop所在的线程 执行cb
            queeueInLoop(cb);
        }
    }
    // 把cb 放到 队列中 唤醒 loop 所在的线程 执行cb
    void EventLoop::queeueInLoop(Functor cb)
    {
        {
            std::unique_lock<std::mutex> lock(mutex_);
            pendingFunctions_.emplace_back(cb);     // 直接构造cb
        }

        // 唤醒 相应的， 需要执行上面 回调操作的 loop的线程
        if(isInLoopThread() || callingPendingFunctions_)   // 代表 当前的 Loop 正在执行 回调 但是 又有新的回调
        {
            wakeup();       // 唤醒 loop 所在的线程
        }

        
    }


    void EventLoop::handleRead()
    {
        uint16_t one = 1;
        ssize_t n = read(wakeupFd_,&one,sizeof one);
        if(n != sizeof one)
        {
            LOG_ERROR("EventLoop::handleRead() reads %d bytes instead of 8 ",n);
        }

    }



    // 唤醒loop 所在的线程
    void EventLoop::wakeup()
    {
        uint64_t one = 1;
        ssize_t n = write(wakeupFd_,&one,sizeof one);

        if(n != sizeof one)
        {
            LOG_ERROR ("EventLoop :: Wakup() Writes %lu bytes instead of 8",n);
        }
    }


    // Event Loop => Poller 的 方法
    void EventLoop::updateChannel(Channel * channel)
    {
        poller_->updateChannel(channel);
    }

    void EventLoop::removeChannel(Channel * channel)
    {
        poller_->removeChannel(channel);
    }
    
    bool EventLoop::hasChannel(Channel * channel)
    {
        poller_->hasChannel(channel);
    }

    void EventLoop::doPendingFunctors()		// 执行回调函数
    {
        std::vector<Functor> functors;
        callingPendingFunctions_ = true;

        {
            std::unique_lock<std::mutex> lock(mutex_);
            functors.swap(pendingFunctions_);            
        }

        for(const Functor& functor : functors)
        {
            functor();      // 执行  当前 loop 的 回调函数
        }

        callingPendingFunctions_ = false;
    }








    

}