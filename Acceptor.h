#pragma once

#include "noncopyable.h"
#include "Socket.h"
#include "Channel.h"
// 非指针引用变量 包含头文件

#include <functional>


namespace mymuduo
{

    class EventLoop;
    class InetAddress;

    class Acceptor : noncopyable
    {

        public:
            using NewConnectionCallback = std::function<void(int sockfd,const InetAddress&)>;

        public:
            Acceptor(EventLoop * loop,const InetAddress &listenaddr,bool resueport);         
            ~Acceptor();

            void setNewConnectCallback(const NewConnectionCallback & cb){
                // newConnectionCallback_ = std::move(cb);
                newConnectionCallback_ = cb;

            }

            bool listening () {return listenning_;}
            void listen();

        private:
            void handleRead();


        private:
            // Acceptor 使用的就是用户定义的 baseloop  也是 mainloop
            EventLoop *loop_;
            Socket acceptSocket_;
            Channel acceptChannel_;
            NewConnectionCallback newConnectionCallback_;
            bool listenning_;
    };
} // namespace mymuduo
