#pragma once

#include <functional>
#include <atomic>
#include <memory>
#include <vector>
#include <mutex>

#include "noncopyable.h"
#include "Timestamp.h"
#include "CurrentThread.h"

namespace mymuduo {


	class Channel;
	class Poller;


	// 事件循环 类  主要 包含 Channel  Poller 类   epoll 的 抽象
	class EventLoop : noncopyable
	{
		public:
			using Functor = std::function<void()>;

		public:
			EventLoop();
			~EventLoop();


			//开启事件循环
			void loop();
			// 退出事件循环
			void quit();			


			Timestamp pollReturnTime() const {return pollerReturnTime_;}


			// 在当前loop 中 执行 cb
			void runInLoop(Functor cb);
			// 把cb 放到 队列中 唤醒 loop 所在的线程 执行cb
			void queeueInLoop(Functor cb);



			// 换幸福loop 所在的线程
			void wakeup();


			// Event Loop => Poller 的 方法
			void updateChannel(Channel * channel);
			void removeChannel(Channel * channel);
			bool hasChannel(Channel * channel);


			// 如果它们相等，则表示当前线程是事件循环所属的线程，函数返回 true；否则，返回 false
			// 判断EventLoop 对象是否在自己线程里面
			bool isInLoopThread() const {return thread_Id_ == CurrentThread::tid();}
		
		private:
			void handleRead();				// 唤醒weakup
			void doPendingFunctors();		// 执行回调函数



		private:
			using ChannelList = std::vector<Channel*>;
		
			std::atomic_bool looping_;		// 原子操作 
			std::atomic_bool quit_;			// 退出loop循环
			std::atomic_bool callingPendingFunctions_;		// 标识当前loop是否有需要执行的 回调函数

			const pid_t thread_Id_; //  记录 当前loop所在线程的id
			Timestamp  pollerReturnTime_;

			std::unique_ptr<Poller> poller_;  	// EventLoop 管理的 poller

			int wakeupFd_;			// 当mainLoop 获取一个新用户的 channel 通过轮询算法选择subloop处理channel

			std::unique_ptr<Channel> wakeupChannel_;

			ChannelList activeChannels_;
			Channel * currentActiveChannel_;

			std::atomic_bool callingPendingFunctors_;  // 标识 当前loop 是否有需要执行的回调操作
			std::vector<Functor> pendingFunctions_;		// 存储 loop 需要执行的所有的 回调操作
			std::mutex mutex_;							// 互斥锁 保护 vector de的线程安全操作


	};
}
