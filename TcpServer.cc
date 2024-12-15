#include "TcpServer.h"
#include "logger.h"
#include "Util.h"
#include "TcpConnection.h"
#include <cstring>


namespace mymuduo {



    	TcpServer::TcpServer(EventLoop *loop,
			const InetAddress & listenAddr,
			const std::string nameArg,
			Option option)
            : loop_(CheckLoopNotNull(loop))
            , ipPort_(listenAddr.toIpPort())
            , name_(nameArg)
            , acceptor_(new Acceptor(loop,listenAddr,option == kReusePort))
            , threadPool_(new EventLoopThreadPool(loop,name_))
            , connectionCallback_()
            , messageCallback_()
            , nextConnId_(1)
        {
            // 当有新用户链接时  会执行 TcpServer::newConnectionCallback 
            acceptor_->setNewConnectCallback(std::bind(&TcpServer::newConnection,this,std::placeholders::_1,std::placeholders::_2));
        }
		
        TcpServer::~TcpServer()
        {
            for(auto & item : connects_)
            {
                // 这个局部的share 的 ptr 的只能指针 出右括号 可以自动释放
                TcpConnectionPtr conn(item.second);
                item.second.reset();

                conn->getLoop()->runInLoop(
                    std::bind(&TcpConnection::connectDestroyed,conn)
                );
            }
        }
		// 设置底层subloop 的个数
		void TcpServer::setThreadNum (int 	numThread)
        {
        } 
		// 开启服务器 监听
		void TcpServer::start()
        {
            threadPool_->start(threadInitCallback_);           // 启动底层的线程池
            loop_->runInLoop(std::bind(&Acceptor::listen,acceptor_.get()));
        }
        void TcpServer::newConnection(int sockfd,const InetAddress & peerAddress)
        {
            // 轮询算法 选择 subloop 来管理 channel
            EventLoop * ioloop = threadPool_->getNextloop();
            char buffer[64] = {0};
            snprintf(buffer, sizeof buffer, "-%s#%d",ipPort_.c_str(),nextConnId_);
            ++nextConnId_;
            std::string connName = name_+buffer;

            LOG_INFO("TcpServer::newConnection[%s] - new connection [%s] fropm %s \n",\
                name_.c_str(),connName.c_str(),peerAddress.toIpPort().c_str()
            );
            // 通过 socketfd 获取其 绑定的本机的ip地址和 端口信息
            sockaddr_in local;
            ::bzero(& local,sizeof local);

            socklen_t addrlen = sizeof local;
            if(::getsockname(sockfd,(sockaddr*)&local,&addrlen) < 0)
            {
                LOG_ERROR("Socket::getLocalAddr \n");
            }

            InetAddress localAddr(local);

            TcpConnectionPtr conn(new TcpConnection(
                                    ioloop,
                                    connName,
                                    sockfd,   // Socket Channel
                                    localAddr,
                                    peerAddress));

            connects_[connName] = conn;
            // 下面的回调都是用户 设置的
            conn->setConnectionCallback(connectionCallback_);
            conn->setMessageCallback(messageCallback_);
            conn->setWriteCompleteCallback(writeComplateCallback_);

            // 设置如何关闭连接的 回调
            conn->setCloseCallback(
                std::bind(&TcpServer::removeConnection,this,std::placeholders::_1)
            );
            ioloop->runInLoop(std::bind(&TcpConnection::connectEstablelished,conn));
            
        }
		void TcpServer::removeConnection(const TcpConnectionPtr & conn)
        {
            loop_->runInLoop(
                std::bind(&TcpServer::removeConnectionInLoop,this,conn)
            );
        }
		void TcpServer::removeConnectionInLoop(const TcpConnectionPtr & conn)
        {
            LOG_INFO(
                "TcpServer::removeConnectionInLoop[%s] - connection %s \n",
                name_.c_str(),conn->name().c_str()
            );
            connects_.erase(conn->name());
            EventLoop * ioloop = conn->getLoop();
            ioloop->queeueInLoop(
                std::bind(&TcpConnection::connectDestroyed,conn)
            );
        }

}

