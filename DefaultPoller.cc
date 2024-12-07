#include "Poller.h"


#include <cstdlib>
#include <stdlib.h>


// 该文件 和 基类处于 同一层 
// 派生类 可以引用  不能引用派生类

namespace mymuduo {
	
	Poller * Poller::newDefaultPoller(EventLoop *loop)
	{
		if(getenv("MUDUO_USE_POLL"))
		{
			return nullptr;
		}else {
		
			return nullptr;
		}
	}
	
}
