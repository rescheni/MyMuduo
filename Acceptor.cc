#include "Acceptor.h"
#include "logger.h"
#include "InetAddress.h"


#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h> 
#include <errno.h>


namespace mymuduo
{


    static int createSocket()
    {
        int sockfd = ::socket(AF_INET,SOCK_STREAM|SOCK_NONBLOCK|SOCK_CLOEXEC,0);
        // int sockfd = ::socket(AF_INET,SOCK_STREAM|SOCK_NONBLOCK|SOCK_CLOEXEC,IPPROTO_TCP);
        if(sockfd  < 0)
        {
            LOG_FATAL("%s:%s:%d|listen Socket  create err : %d  \n",__FILE__,__FUNCTION__,__LINE__,errno);
        }

        LOG_DEBUG("Acceptor::createSocket =========> OK !");

        return sockfd;
    }


    Acceptor::Acceptor(EventLoop * loop,const InetAddress &listenaddr,bool resueport)
        : loop_(loop)
        , acceptSocket_(createSocket())
        , acceptChannel_(loop,acceptSocket_.fd())
        , listenning_(false)
    {
        // 
        acceptSocket_.setReuseAddr(true);
        acceptSocket_.setReusePort(true);
        acceptSocket_.bindAddress(listenaddr); // bind

        acceptChannel_.setReadCallback(std::bind(&Acceptor::handleRead,this));
    }

    Acceptor::~Acceptor()
    {
        acceptChannel_.disableAll();
        acceptChannel_.remove();

    }
    void Acceptor::listen()
    {
        listenning_ = true;
        LOG_DEBUG("Listen Start ==============>  OK!");
        acceptSocket_.listen();             // listen
        acceptChannel_.enableReading();      // 注册 acceptChannel =>Poller
    }


    // listen 有事件发生了   ，， 就是有新用户连接了
    void Acceptor::handleRead()
    {
        InetAddress peerAddr;
        int connfd = acceptSocket_.accept(&peerAddr);
        if(connfd >= 0)
        {
            if(newConnectionCallback_)
            {
                newConnectionCallback_(connfd,peerAddr);
            }   
            else{
                ::close(connfd);    
            }
        }
        else
        {

            LOG_ERROR("%s:%s:%d| Accept  err : %d  \n",__FILE__,__FUNCTION__,__LINE__,errno);
            if(errno == EMFILE)
            {
                LOG_ERROR("%s:%s:%d| Accept  err :|||| {errno = EMFILE} \n",__FILE__,__FUNCTION__,__LINE__);
            }
        } 
    }

    
} // namespace mymuduo
