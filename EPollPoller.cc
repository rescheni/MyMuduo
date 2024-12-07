#include "EPollPoller.h"
#include "logger.h"



#include <cerrno>
#include <unistd.h>


namespace mymuduo {
	// status key
	const int kNew = -1;		// channel 的成员 index = -1  knew 代表channel 的 成员 没有添加到poller 中
	const int kAdded = 1;		
	const int kDeleted = 2;

	EPollPoller::EPollPoller(EventLoop * loop)
		:Poller(loop),
		epollfd_(epoll_create1(EPOLL_CLOEXEC)),
		events_(kInitEventListSize) // on EPollPoller.h default = 16
	{
		if(epollfd_ < 0)
		{
			LOG_FATAL("EPoll_create error: %d \n",errno);
		}
	}
	EPollPoller::~EPollPoller()
	{
		close(epollfd_);
	}

	// 重写 基类抽象方法 override
	Timestamp EPollPoller::poll(int timeoutMs,ChannelList * activeChannels)
	{

	}
	
	// Channel update remove  =>  EventLoop updateChannel removeChannel => Poller updateChannel removeChannel
	void EPollPoller::updateChannel(Channel * channel) 
	{

	}
	
	void EPollPoller::removeChannel(Channel * channel) 
	{

	}

	// 填写 活跃的 连接
	void EPollPoller::fillActiveChannels(int numEvents,ChannelList * activeChannels) const 
	{

	}

	// 更新 channel  
	void EPollPoller::update(int operation,Channel * channel)
	{

	}

}
