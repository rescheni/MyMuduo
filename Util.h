#pragma once 

#include "EventLoop.h"
#include "logger.h"


namespace mymuduo
{
    static EventLoop * CheckLoopNotNull(EventLoop * loop)
    {
        
        if(loop == nullptr)
        {
            LOG_FATAL("%s:%s:%d Loop is null \n",__FILE__,__FUNCTION__,__LINE__);
        }
        return loop;
    }

    
} // namespace mymuduo
