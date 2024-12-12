#include "Socket.h"
#include "logger.h"

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <cstring>
#include <netinet/tcp.h>

namespace mymuduo
{

    Socket::~Socket()
    {
        close(sockfd_);
    }
    void Socket::bindAddress(const InetAddress & localaddr)
    {
        if(bind(sockfd_,(sockaddr*)localaddr.getSockAddr(),sizeof(sockaddr_in)) != 0)
        {
            LOG_FATAL("band Socketfd%d fail \n",sockfd_);
        }
    }
    void Socket::listen()
    {
        if(::listen (sockfd_,1024)!=0){
            LOG_FATAL("band Socketfd%d fail \n",sockfd_);
        }   
    }
    int  Socket::accept(InetAddress * peeraddr)
    {
        sockaddr_in addr ;
        socklen_t len;
        bzero(& addr,sizeof addr);
        int connfd = ::accept(sockfd_,(sockaddr*)&addr,&len);
        if(connfd >= 0)
        {
            peeraddr->setSockAddr(addr);
        }
        return connfd;
    }
    void Socket::shutdownWrite()
    {
        if(shutdown(sockfd_ ,SHUT_WR) < 0)
        {
            LOG_ERROR("Shutdown Write Error");
        }
    }
    void Socket::setTcpNoDelay(bool on)
    {
        int optValue = on?1:0;
        setsockopt(sockfd_,IPPROTO_TCP,TCP_NODELAY,&optValue,sizeof optValue);
    }

    void Socket::setReuseAddr(bool on)
    {
        int optValue = on?1:0;
        setsockopt(sockfd_,SOL_SOCKET,SO_REUSEADDR,&optValue,sizeof optValue);
    }

    void Socket::setReusePort(bool on)
    {
        int optValue = on?1:0;
        setsockopt(sockfd_,SOL_SOCKET,SO_REUSEPORT,&optValue,sizeof optValue);
    }

    void Socket::setKeepAlive(bool on)
    {
        int optValue = on?1:0;
        setsockopt(sockfd_,SOL_SOCKET,SO_KEEPALIVE,&optValue,sizeof optValue);
    }

    
} // namespace mymuduo
