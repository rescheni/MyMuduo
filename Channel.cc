#include <sys/epoll.h>

#include "Channel.h"
#include "EventLoop.h"
#include "logger.h"



namespace mymuduo {
	
	const int Channel::kNoneEvent = 0;
	const int Channel::kReadEvent = EPOLLIN|EPOLLPRI;
	const int Channel::kWriteEvent = EPOLLOUT;


	Channel::Channel(EventLoop * loop,int fd)
		:loop_(loop)
		,fd_(fd)
		,events_(0)
		,revents_(0)
		,index_(-1)
		,tied_(false)
	{

	}
	
	Channel::~Channel()
	{

	}

	void Channel::tie(const std::shared_ptr<void> &obj)
	{
		tie_ = obj;
		tied_ = true; // 绑定成功
	}

	
	// 当改变channel 所表示的 fd 的 events 事件后 update 负责 在 poller 里面更改相应的事件 epoll_ctl
	// EventLoop => channelList  poller
	void Channel::update()
	{
		// 通过 channel 所属的eventsLoop,调用相应的  方法  注册fd 的 events 事件
		
		// ADD code ...

		loop_->updateChannel(this);

	}

	// 在 channel 所属 的 event loop 中 删除当前的 channel
	void Channel::remove()
	{
		// ADD code ...
		loop_->removeChannel(this);
	}

	// fd_   得到 poller 通知以后 处理 事件 的
	void Channel::handleEvent(Timestamp receiveTime)
	{
		if(tied_)
		{
			std::shared_ptr<void> guard = tie_.lock(); // 智能指针的 提升级、
			
			if(guard)
			{
				handleEventWithGuard(receiveTime);
			}

		}else {
			handleEventWithGuard(receiveTime);
		}
	}

	// Private:
	void Channel::handleEventWithGuard(Timestamp receiveTime)
	{
		LOG_INFO("channel handleEvent revents:%d\n", revents_);

		if ((revents_ & EPOLLHUP) && !(revents_ & EPOLLIN))
		{
			if (closeCallback_)
			{
				LOG_DEBUG("call close Callback");
				closeCallback_();
			}
		}

		if (revents_ & EPOLLERR)
		{
			if (errorCallback_)
			{
				LOG_DEBUG("call error Callback");
				errorCallback_();
			}
		}

		if (revents_ & (EPOLLIN | EPOLLPRI))
		{
			if (readCallback_)
			{
				LOG_DEBUG("call read Callback");
				readCallback_(receiveTime);
			}
		}

		if (revents_ & EPOLLOUT)
		{
			if (writeCallback_)
			{
				LOG_DEBUG("call Write Callback");
				writeCallback_();
			}
		}
	}
}