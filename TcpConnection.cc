#include "TcpConnection.h"
#include "logger.h"
#include "Util.h"
#include "Channel.h"
#include "Socket.h"
#include "Buffer.h"
#include "EventLoop.h"

#include <functional>
#include <cerrno>

namespace mymuduo
{
    TcpConnection::TcpConnection(
                    EventLoop * loop,
                    const std::string & nameArg,
                    int sockfd,
                    const InetAddress & localAddr,
                    const InetAddress & peerAddr
                )
                : loop_(CheckLoopNotNull(loop))
                , name_(nameArg)
                , state_(kConnecting)
                , reading_(true)
                , socket_(new Socket(sockfd))
                , channel_(new Channel(loop,sockfd))
                , localAddr_(localAddr)
                , peerAddr_(peerAddr)
                , highWaterMark_(64 * 1024 * 1024)  // 64 M
    {

        // 给 channel 设置相应的回调函数 
        // poller 给 channel 通知感兴趣的 事件
        // 事件发生 channel 会 回调相应的操作函数

        channel_->setReadCallback(
            std::bind(&TcpConnection::handleRead,this,std::placeholders::_1)
        );
        channel_->setWriteCallback(
            std::bind(&TcpConnection::handleWrite,this)
        );
        channel_->setCloseCallback(
            std::bind(&TcpConnection::handleClose,this)
        );
        channel_->setErrorCallback(
            std::bind(&TcpConnection::handleError,this)
        );

        LOG_INFO("TcpConnection::ctor[%s] at fd = %d \n",name_.c_str(),sockfd);
        socket_->setKeepAlive(true);    //  TCP 的 保活机制     
    }

    TcpConnection::~TcpConnection()
    {
        LOG_INFO("TcpConnection::dtor[%s] at fd = %d state = %d \n",\
        name_.c_str(),channel_->fd() , (int)state_);
    }



    void TcpConnection::handleRead(Timestamp receiveTime)
    {
        int savedErrno = 0;
        ssize_t n = inputbuffer_.readFd(channel_->fd(), &savedErrno);

        if(n  > 0)
        {
            // 已经建立连接 的 用户有可读事件发生了 
            messageCallback_(shared_from_this(), &inputbuffer_,receiveTime);
        }
        else if(n == 0)
        {
            handleClose();
        }else{
            errno = savedErrno;
            LOG_ERROR("TcpConnection::handleRead");
            handleError();

        }

    }


    void TcpConnection::handleWrite()
    {
        LOG_DEBUG("TcpConnection::handleWrite() ===========> IN");
        if(channel_->isWriting())
        {
            int savedErrno = 0;
            ssize_t n = inputbuffer_.readFd(channel_->fd(),&savedErrno);
            if(n > 0)
            {
                outputbuffer_.retrieve(n);
                if(outputbuffer_.readableBytes() == 0) // 继续发送
                {
                    channel_->disableWriting();
                    if(writeComplateCallback_)
                    {
                        loop_->queeueInLoop(
                            std::bind(writeComplateCallback_,shared_from_this())
                        );
                    }
                    if(state_ == kDisconnecting)
                    {
                        shutdownInLoop();
                    }
                }
            }
            else {
                LOG_ERROR("Tcp Connection ::handleWrite");
            }
        }else{
            LOG_ERROR("Tcp Connection fd=%d is down ,no more warining \n",channel_->fd());
        }
        LOG_DEBUG("TcpConnection::handleWrite() ===========> OUT");
    }


    // Poller => channel::closeCallback => TcpConnection::handleClose

    void TcpConnection::handleClose()
    {
        LOG_INFO("fd=%d, state=%d \n",channel_->fd(),(int)state_);
        setState(kDisconnected);
        channel_->disableAll();

        TcpConnectionPtr connptr(shared_from_this());
        connectionCallback_(connptr);       // 关闭执行连接的回调
        closeCallback_(connptr);            // 关闭连接的回调
    }

    void TcpConnection::handleError()
    {
        int optValue;
        socklen_t optlen = sizeof optValue;
        int err = 0;

        if(::getsockopt(channel_->fd(),SOL_SOCKET,SO_ERROR,&optValue,&optlen) < 0)
        {
            err = errno;
        }else {
            err = optValue;
        }
        LOG_ERROR("TcpConnection::handleError name:%s - So errno:%d \n",name_.c_str(),err);
    }


    void TcpConnection::send(const std::string & buffer)
    {

        LOG_DEBUG("state_ == kConnected ? = %s",state_ == kConnected?"Yes":"No");
        if(state_ == kConnected)
        {
            if(loop_->isInLoopThread())
            {
                LOG_DEBUG("Runing ======runInLoop============> OK");
                sendInLoop(buffer.c_str(),buffer.size());
            }
            else
            {
                loop_->runInLoop(std::bind(
                    &TcpConnection::sendInLoop,
                    this,
                    buffer.c_str(),
                    buffer.size()
                ));

            }
        }
    }
    // 发送数据 应用  写的块 而内核数据 发送数据慢 将 发送数据写入缓冲区，并且设置了  水位回调
    void TcpConnection::sendInLoop(const void * message,size_t len)
    {


        LOG_DEBUG("IN TcpConnection::sendInLoop ============> OK!!");

        size_t nwrote = 0;
        size_t remaining = len;
        bool faultError = false;

        // 之前调用过  connection  的 shutdown    不能再次发送了
        if(state_ == kDisconnected)
        {
            LOG_ERROR("disconnected give up writing!");
            return;
        }

        LOG_DEBUG("channel_->isWriting() = %s | readableBytes = %ld",channel_->isReading()?"Yes":"NO",outputbuffer_.readableBytes());

        //  表示第一次开始写数据 并且缓冲区没有待发送的数据
        if(! channel_->isWriting() && outputbuffer_.readableBytes() == 0)
        {
            nwrote = ::write(channel_->fd(),message,len);
            if(nwrote >= 0)
            {
                remaining = len - nwrote;
                if(remaining == 0 && writeComplateCallback_)  // 刚好发送完成
                {
                    loop_->queeueInLoop(
                        std::bind(writeComplateCallback_,shared_from_this())
                    );
                    LOG_DEBUG("数据发送完毕===========> OK");

                }
                LOG_DEBUG("sendOK===============> nwrote=%d , message=%s",(int)nwrote,(char *)message);
            }
            else{ // nwrote < 0 
                nwrote = 0;
                if(errno != EWOULDBLOCK)
                {
                    LOG_ERROR("TcpConnection::sendInLoop");

                    if(errno == EPIPE || errno == ECONNRESET) // SIGPIPEv  REST 连接重置
                    {
                        faultError = true;
                    }
                }
            }

            // 当前 这一次write 并没有把数据全部都发出去  剩余的数据需要全部保存到缓冲区中 然后给channel 
            // LT 模式poller 会不断的发送信息
            // 注册 epollerout 事件  poller 会发送tcp 的 缓冲区 空余 会通知相应的socket->channel 调用 writeCallback 回调方法
            // 调用 TcpConnection：：handWrite 方法 把缓冲区的数据全部发送完成
            if(!faultError && remaining > 0)
            {
                //  目前缓冲区发送的数据 剩余待发送的数据长度
                size_t oldlen = outputbuffer_.readableBytes();
                if(oldlen + remaining >= highWaterMark_
                    && oldlen < highWaterMark_
                    && hightWateMarkCallback_
                    )
                {
                        loop_->queeueInLoop(
                            std::bind(hightWateMarkCallback_, shared_from_this(), oldlen+remaining)
                        );
                }
                outputbuffer_.append((char*)message + nwrote,remaining);

                if(!channel_->isWriting())
                {
                    channel_->enableWriting(); // 注册channel 的写事件  否则 poller 不会给 channel 通知 epollout
                }
            }
        }
    }


    // 建立连接
	void TcpConnection::connectEstablelished()
    {
        setState(kConnected);
        channel_->tie(shared_from_this());

        channel_->enableReading();  // 向poller注册 channel的epollin事件
        // 新连接建立 

        connectionCallback_(shared_from_this());

    }
	
	// 销毁连接
	void TcpConnection::connectDestroyed()
    {
        if(state_ == kConnected)
        {
            setState(kDisconnected);
            channel_->disableAll();;   // 将 poller 的所有事件从poller 中del 掉
            connectionCallback_(shared_from_this());
        }
        channel_->remove();
    }

    void TcpConnection::shutdown()
    {
        if(state_ == kConnected)
        {
            setState(kDisconnecting);
            loop_->runInLoop(
                std::bind(&TcpConnection::shutdownInLoop,this)
            );
        }
    }


    void TcpConnection::shutdownInLoop()
    {
        if(!channel_->isWriting()) // 说明 outputbuffer   全部发送完成
        {
            socket_->shutdownWrite();
        }
    }


        
} // namespace mymuduo
