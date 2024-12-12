#include "Thread.h"
#include "CurrentThread.h"

#include <semaphore.h>

namespace mymuduo
{

    static std::atomic_int numCreated_(0);

    Thread::Thread(ThreadFunc func,const std::string &name)
        : started_(false)
        , joined_(false)
        , tid_(0)
        , func_(std::move(func))
        , name_(name)
    {
        setDefaultName();
    }
    Thread::~Thread()
    {
        if(started_ &&  ! joined_)
        {
            thread_->detach();  // thread 类 提供的 设置 分离线程的方法
        }
    }
    void Thread::start()            // 一个 Thread 对象 记录的就是 一个新线程的详细信息
    {
        started_ = true;
        sem_t sem;
        sem_init(&sem,false,0);


        ///////////   子线程   ////////////
        // 开启 线程  one loop poll thread
        thread_ = std::shared_ptr<std::thread>(new std::thread([&](){
            
            // 获取线程的 TID
            tid_ = CurrentThread::tid();
            sem_post(&sem);

            //  开启一个新线程
            func_();  

        }));
        // 等待 获取上面创建新线程的 tid 值
        sem_wait(&sem);

        // started_ = false;    
    }
    void Thread::join()
    {
        joined_ = true;
        thread_->join();
    }
    void Thread::setDefaultName()
    {
        int num = ++numCreated_;
        if(name_.empty())
        {
            char buffer[32] = {0};
            snprintf(buffer,sizeof buffer , "Thread%d ",num);            
            name_ = buffer;
        }
    }


}   // end  namespace  mymuduo