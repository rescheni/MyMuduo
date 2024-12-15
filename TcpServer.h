#pragma once 


// 包含用户常用 TCPServer 常用的头文件 (给用户用)
#include "EventLoop.h"
#include "Acceptor.h"
#include "InetAddress.h"
#include "noncopyable.h"
#include "EventLoopThreadPool.h"
#include "Callbacks.h"

#include <functional>
#include <string>
#include <memory>
#include <atomic>
#include <unordered_map>


namespace mymuduo {

	// 对外部 TCP 服务器 编程 提供的 类
	class TcpServer :noncopyable
	{
		public:
			using ThreadInitCallback = std::function<void(EventLoop*)>;  // 线程 初始化 回调

			enum Option
			{
				kNoReusePort,
				kReusePort
			};
			

			TcpServer(EventLoop *loop,
					const InetAddress & listenAddr,
					const std::string nameArg,
					Option option = kNoReusePort);
			~TcpServer();


			void setThreadInitcallback(const ThreadInitCallback & cb){threadInitCallback_ = cb;}
			void setConnectioncallback(const ConnectionCallback & cb){connectionCallback_ = cb;}
			void setMessagecallback(const MessageCallback & cb){messageCallback_ = cb;}
			void setWriteComplatecallback(const WriteComplateCallback & cb){writeComplateCallback_ = cb;}
			// 设置底层subloop 的个数
			void setThreadNum (int 	numThread); 

			// 开启服务器 监听
			void start();


		private:

			void newConnection(int sockfd,const InetAddress & peerAddress);
			void removeConnection(const TcpConnectionPtr & conn);
			void removeConnectionInLoop(const TcpConnectionPtr & conn);	


		private:
			using  ConnectionMap = std::unordered_map<std::string,TcpConnectionPtr>;


			EventLoop * loop_;			//	baseloop 用户定义的loop
			const std::string ipPort_;
			const std::string name_;
			std::unique_ptr<Acceptor> acceptor_;				// in mainloop 监听新的连接事件 
			std::shared_ptr<EventLoopThreadPool> threadPool_;  //	one per Thread

			ConnectionCallback connectionCallback_;			// 有新连接时的 回调
			MessageCallback messageCallback_;				// 有读写消息时的回调
			WriteComplateCallback writeComplateCallback_;	// 消息写完后的 回调

			ThreadInitCallback threadInitCallback_; 		// 线程初始化后的回调

			std::atomic_int  started_;

			int nextConnId_;

			ConnectionMap connects_ ; 	//	保存所有的连接


	};
}
