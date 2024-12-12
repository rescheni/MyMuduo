#pragma once

#include "noncopyable.h"
#include "InetAddress.h"



// 封装 socket

namespace mymuduo
{
    class Socket : noncopyable
    {
        public:
            explicit Socket(int sockfd)
                :sockfd_(sockfd)
            {}
            ~Socket();


            int fd() const {return sockfd_;}
            void bindAddress(const InetAddress & localaddr);
            void listen();
            int accept(InetAddress * peeraddr);

            void shutdownWrite();

            void setTcpNoDelay(bool on);
            void setReuseAddr(bool on);
            void setReusePort(bool on);
            void setKeepAlive(bool on);
            
        private:
            const int sockfd_;

    };

    
    
} // namespace mymuduo
