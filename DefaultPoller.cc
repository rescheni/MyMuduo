#include "Poller.h"
#include "EPollPoller.h"


#include <cstdlib>
#include <stdlib.h>


// 该文件 和 基类处于 同一层 
// 派生类 可以引用  不能引用派生类

namespace mymuduo {
	

	// EventLoop 可以 通过该接口 获取 默认 的 IO 复用的 具体实现
	Poller * Poller::newDefaultPoller(EventLoop *loop)
	{
		if(getenv("MUDUO_USE_POLL"))
		{
			return nullptr;
		}else {
			return new EPollPoller(loop);
		}
	}
	
}
