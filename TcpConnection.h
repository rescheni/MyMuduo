#pragma once


#include <algorithm>
#include <memory>
#include <string>
#include <atomic>


#include "Timestamp.h"
#include "Callbacks.h"
#include "InetAddress.h"
#include "noncopyable.h"
#include "Buffer.h"



// 



namespace mymuduo
{

	class Channel;
	class EventLoop;
	class Socket;

    class TcpConnection : noncopyable,public std::enable_shared_from_this<TcpConnection>
    {
        public:
            TcpConnection(
							EventLoop * loop,
							const std::string & name,
							int sockfd,
							const InetAddress & localAddr,
							const InetAddress & peerAddr
						);

            ~TcpConnection();

			EventLoop * getLoop() const {return loop_;}
			const std::string & name () const {return name_;}
			const InetAddress & localAddress () const {return localAddr_;}
			const InetAddress & peerAddress() const {return peerAddr_;}

			bool connected() const {return state_ == kConnected; }

			void setConnectionCallback( const ConnectionCallback & cb)
			{connectionCallback_ = cb;}
			void setMessageCallback(const MessageCallback & cb)
			{messageCallback_ = cb;}
			void setWriteCompleteCallback(const WriteComplateCallback & cb)
			{writeComplateCallback_ = cb;}
			void setHightWaterMarkCallbac(const HightWateMarkCallback & cb,size_t hightWaterMark)
			{hightWateMarkCallback_ = cb; highWaterMark_ = hightWaterMark;}

			void setCloseCallback(const CloseCallback & cb)
			{closeCallback_ = cb;}


			// 建立连接
			void connectEstablelished();
			// 发送数据
			void send(const std::string & buffer);	
			// 销毁连接
			void connectDestroyed();
			// 关闭连接
			void shutdown();


		private:
			enum StateE {
				kDisconnected,	//  已断开连接
				kConnecting,	//  正在连接
				kConnected,		//  已连接
				kDisconnecting  // 正在断开连接
			};

		private:

			void handleWrite();
			void handleRead(Timestamp reveiveTime);
			void handleClose();
			void handleError();

			void sendInLoop(const void * message,size_t len);
			void shutdownInLoop();



        private:
			

			void setState(StateE state){ state_ = state;}


			EventLoop * loop_;  	 // TcpConnection  是 在subLoop 中 管理的
			const std::string name_; //
			std::atomic_int state_;
			bool reading_; 

			std::unique_ptr<Socket> socket_;
			std::unique_ptr<Channel> channel_;

			const InetAddress localAddr_;	// 当前主机
			const InetAddress peerAddr_;	// 对端主机

			ConnectionCallback connectionCallback_;
			MessageCallback messageCallback_;
			WriteComplateCallback writeComplateCallback_;
			HightWateMarkCallback hightWateMarkCallback_;
			CloseCallback closeCallback_;

			int highWaterMark_;

			Buffer inputbuffer_;
			Buffer outputbuffer_;



    };

    

} // namespace mymuduo
