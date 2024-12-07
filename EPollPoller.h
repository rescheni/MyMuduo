#pragma  once

#include "EventLoop.h"
#include "Poller.h"
#include "Timestamp.h"

#include <vector>
#include <sys/epoll.h>






// epoll 的 使用 流程 
// epoll_create
// epoll_ctl  add/mod/del
// epoll_wait
//



namespace mymuduo {


	class Channel;

	class EPollPoller : public Poller
	{
		public:

			EPollPoller(EventLoop * loop);
			~EPollPoller() override;

			// 重写 基类抽象方法 override
			Timestamp poll(int timeoutMs,ChannelList * activeChannels) override;
			void updateChannel(Channel * channel) override;
			void removeChannel(Channel * channel) override;

		private:

			static const int kInitEventListSize = 16;
			
			// 填写 活跃的 连接
			void fillActiveChannels(int numEvents,ChannelList * activeChannels) const ;
			// 更新 channel  
			void update(int operation,Channel * channel);



			using EventList = std::vector<epoll_event>;

			int epollfd_;
			EventList events_;
	};

}
