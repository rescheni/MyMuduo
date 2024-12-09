#pragma once

#include <sys/syscall.h>      /* Definition of SYS_* constants */
#include <unistd.h>


namespace mymuduo{

namespace CurrentThread{

    // 引入外部变量 
    extern __thread int t_cachedTid;
    

    void cacheTid(); // 用于缓存当前线程的线程ID。


    inline int tid()
    {
        // coplit __builtin_expect(t_cachedTid == 0, 0) 的意思是告诉编译器
        // 你期望 t_cachedTid == 0 的情况很少发生   编译器自动优化
        if(__builtin_expect(t_cachedTid == 0,0))
        {
            cacheTid();
        }
        return t_cachedTid;
    }
    
} // namespace CurrentThread

}// namespace mymuduo