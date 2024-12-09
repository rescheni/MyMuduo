#include "EPollPoller.h"
#include "logger.h"
#include "Channel.h"


#include <cerrno>
#include <unistd.h>
#include <cstring>  

namespace mymuduo {
	// status key
	// channel 未添加到poller 中
	const int kNew = -1;		// channel 的成员 index = -1 knew 
	// channel 已添加到poller中
	const int kAdded = 1;	
	// channel 从 poller 中删除
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
		LOG_INFO("func=%s =>  fd total count:%d \n",__FUNCTION__ ,static_cast<int>( channels_.size()));
		
		int numEvents = epoll_wait(epollfd_,&*events_.begin(),static_cast<int>(events_.size()),timeoutMs);
		int saveError = errno;  // 存储 errrno 变量

		Timestamp now (Timestamp::now());

		if(numEvents > 0)
		{
			LOG_INFO("%d,event happened \n",numEvents);
			fillActiveChannels(numEvents,activeChannels);

			if(numEvents == events_.size())
			{
				events_.resize(events_.size() * 2);

			}
		}	
		else if(numEvents == 0)
 		{
			LOG_DEBUG("%s timeout ! \n",__FUNCTION__);			
		}
		else{
			if(saveError != EINTR)
			{
				errno = saveError;
				LOG_ERROR("EPOLL_POLLER::poll() err !");
			}
		}

		return now;
	}
	
	// Channel update remove  =>  EventLoop updateChannel removeChannel => Poller updateChannel removeChannel
	
	//EPollPoller 
	//	  ChannelList
	//	  Poller
	//	  	channelMap<fd,Channel *>

	void EPollPoller::updateChannel(Channel * channel) 
	{
		const int index = channel->index();
		LOG_INFO("func=%s => fd=%d events=%d index=%d \n",__FUNCTION__ ,channel->fd(),channel->events(),index);

		if(index == kNew || index == kDeleted )
		{
			// channel 未被注册 
			if(index == kNew)
			{
				int fd =  channel->fd();
				channels_[fd] = channel;
			}
			channel->setIndex(kAdded);
			update(EPOLL_CTL_ADD,channel);
		}
		else  	// channel 已经 注册过了
		{
			int fd = channel->fd();
			if(channel->isNoneEvents())  // 对所有事件都不关心
			{
				update(EPOLL_CTL_DEL,channel);
				channel->setIndex(kDeleted);
			}else{
				update(EPOLL_CTL_MOD,channel);
			}
		}

	}
	
	void EPollPoller::removeChannel(Channel * channel) 
	{
		int fd = channel->fd();
		// 从 channelList 中删除
		channels_.erase(fd);  
		LOG_INFO("func=%s => fd=%d \n",__FUNCTION__ ,fd);
		 
		int index = channel->index();
		if(index == kAdded) // 现在的是 在已经添加的状态
		{
			update(EPOLL_CTL_DEL,channel);
		}
		channel->setIndex(kNew);


	}

	// 填写 活跃的 连接
	void EPollPoller::fillActiveChannels(int numEvents,ChannelList * activeChannels) const 
	{
		for(int i = 0;i < numEvents;++i)
		{
			Channel * channel = static_cast<Channel*>(events_[i].data.ptr);
			channel->set_revents(events_[i].events);	// 设置 具体发生的 事件
			activeChannels->push_back(channel);
		}
	}

	// 更新 channel  epoll_ctl  add / mod / del
	void EPollPoller::update(int operation,Channel * channel)
	{
		struct epoll_event event;
		memset(& event,0,sizeof event);
		int fd = channel->fd();

		event.events = channel->events();
		event.data.fd = fd;
		event.data.ptr = channel;

		
		if(epoll_ctl(epollfd_,operation,fd,&event) < 0)
		{
			if(operation == EPOLL_CTL_ADD)
			{
				LOG_ERROR("epoll_ctl del error: %d \n",errno);
			}else{
				LOG_FATAL("epoll_ctl add/mod errnr: %d \n",errno);
			}
		}
	}

}
