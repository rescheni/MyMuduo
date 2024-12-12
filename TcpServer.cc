#include "TcpServer.h"
#include "logger.h"




namespace mymuduo {
        EventLoop * CheckLoopNotNull(EventLoop * loop)
        {
            
            if(loop == nullptr)
            {
                LOG_FATAL("%s:%s:%d main Loop is null \n",__FILE__,__FUNCTION__,__LINE__);
            }
            return loop;
        }



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

        }
		// 设置底层subloop 的个数
		void TcpServer::setThreadNum (int 	numThread)
        {
        } 
		// 开启服务器 监听
		void TcpServer::start()
        {
            threadPool_->start(threadInitCallback_ );           // 启动底层的线程池
            loop_->runInLoop(std::bind(&Acceptor::listen,acceptor_.get()));
        }
        void TcpServer::newConnection(int sockfd,const InetAddress & peerAddress)
        {
        }
		void TcpServer::removeConnection(const TcpConnectionPtr & conn)
        {
        }
		void TcpServer::removeConnectionInLoop(const TcpConnection & conn)
        {
        }

}

