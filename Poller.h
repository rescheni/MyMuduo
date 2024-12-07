#pragma once

#include "noncopyable.h"
#include "Timestamp.h"

#include <vector>
#include <unordered_map>




namespace mymuduo {
	class Channel;
	class EventLoop;
	// muduo 库 中的 多路 事件 分发器
	class Poller:noncopyable
	{
		public:
			using ChannelList = std::vector<Channel*>;

			Poller(EventLoop * loop);

			virtual ~Poller() = default;	// 使用 虚析构 函数 
									    	// 允许你通过基类指针或引用删除派生类对象
										  	// 从而支持多态删除
		   								    // 可以确保在删除派生类对象时，正确调用派生类的析构函数


			// 给所有的IO复用保留统一的接口
			virtual Timestamp poll(int timeoutMs,ChannelList * activeChannels ) = 0;   // 纯虚函数  子类 必须重写
			virtual void updateChannel(Channel * channel) = 0;
			virtual void removeChannel(Channel * channel) = 0;

			// 判断 参数 channel 是否在当前 Poller 中
			bool hasChannel(Channel * channel) const;

			// EventLoop 可以 通过该接口 获取 默认 的 IO 复用的 具体实现
			static Poller * newDefaultPoller(EventLoop * loop);



		protected:
			// 	Map 的 Key  sockfd value ：sockfd 所属的 channel 通道类型
			// 	通过 sockfd  快速 查找 sockfd  所属的 通道
			using ChannelMap = std::unordered_map<int, Channel *>;
			ChannelMap channels_;

		private:
			EventLoop * ownerLoop_; //  定义 Poller 所属的 事件循环
	};
}
