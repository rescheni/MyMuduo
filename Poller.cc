#include "Poller.h"
#include "Channel.h"

namespace mymuduo {


	Poller::Poller(EventLoop *loop)
    	: ownerLoop_(loop)
	{
	}

	// 判断 参数 channel 是否在当前 Poller 中
	bool Poller::hasChannel(Channel * channel) const
	{
		auto it = channels_.find(channel->fd());
		return it != channels_.end() && (it->second == channel);
	}


}
