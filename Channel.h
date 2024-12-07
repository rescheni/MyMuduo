#pragma  once 

#include <functional>
#include <memory>
#include <utility>

#include "noncopyable.h"
#include "Timestamp.h"
// #include "EventLoop.h"

namespace mymuduo {
	// channel  理解为 通道 封装了 sockfd 网络套接字 
	// event事件关心 ： EPOLLIN、 EPOLLOUT
	// poller 返回的 具体事件 
	

	class EventLoop;

	class Channel : noncopyable
	{
		public:
			using EventCallback = std::function<void()>;
			using ReadEventCallback = std::function<void(Timestamp)>;

			Channel(EventLoop * loop,int fd);
			~Channel();

			// fd_   得到 poller 通知以后 处理 事件 的
			void handleEvent(Timestamp receiveTime);


			// 设置  回调函数
			void setReadCallback(ReadEventCallback cb){readCallback_ = std::move(cb);}
			void setWriteCallback(EventCallback cb){writeCallback_ = std::move(cb);}
			void setCloseCallback(EventCallback cb){closeCallback_ = std::move(cb);}
			void setErrorCallback(EventCallback cb){errorCallback_ = std::move(cb);}


			// 弱 智能指针  
			// 防止 当 channel 被 手动 remove 掉 channel 还在进行 回调操作
			void tie(const std::shared_ptr<void>&);


			int fd() const {return fd_;}
			int events() const {return events_;}
			int set_revents(int revt) { return  revents_ =  revt;}

			// 设置 fd  相应的事件状态 
			void enableReading(){ events_ |= kReadEvent; update(); }
			void disableReading(){events_ &= ~kReadEvent;update(); }
			void enableWriting(){events_ |= kWriteEvent; update(); }
			void disableWriting(){events_ &= ~kWriteEvent;update();}
			void disableAll() {events_ = kNoneEvent; update();     }

			int index(){return index_;}
			void setIndex(int idx){index_ = idx;}


			//  返回 fd 当前的 事件 状态
			bool isNoneEvents() const {return events_ == kNoneEvent;}
			bool isWriting() const {return events_ & kWriteEvent;}
			bool isReading() const {return events_ & kReadEvent;}

			//one loop per thread
			EventLoop * ownerLoop() {return loop_;}
			void remove();

		private:
			void update();
			void handleEventWithGuard(Timestamp receiveTime);


		private:
			static const int kNoneEvent;
			static const int kReadEvent;
			static const int kWriteEvent;

			EventLoop * loop_;			// 事件循环
			const int fd_;				// fd Poller 监听的对象
			int events_;				// 注册 fd 返回具体发生的事件
			int revents_;				// poller 返回 及具体发生的事件
			int index_;

			std::weak_ptr<void> tie_;
			
			bool tied_;

			// 	 channel 通道 可以 最先获取 fd 最终 发生的 具体事件 revents 
			// 	 channel 负责调用 具体事件的 回调操作
			ReadEventCallback readCallback_;
			EventCallback writeCallback_;
			EventCallback closeCallback_;
			EventCallback errorCallback_;

			
	};
}
