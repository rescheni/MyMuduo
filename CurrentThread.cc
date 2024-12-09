#include "CurrentThread.h"



namespace mymuduo{

namespace CurrentThread{

    // __thread 关键字，t_cachedTid 是线程局部存储的变量，每个线程都有自己独立的 t_cachedTid
    __thread int t_cachedTid = 0;

    // 用于缓存当前线程的线程ID。
    void cacheTid()
    {
        if(t_cachedTid == 0)
        {
            // 通过 系统调用 完成  获取 PID 的 调用 
            t_cachedTid = static_cast<pid_t>(syscall(SYS_getpid));
        }
    }
    
    
} // namespace CurrentThread

}// namespace mymuduo