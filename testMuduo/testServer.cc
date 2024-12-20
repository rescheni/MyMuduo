#include <mymuduo/TcpServer.h>
#include <iostream>
#include <string>
#include <cstring>
#include <functional>

using namespace mymuduo;


class EchoServer
{
public:
    EchoServer(
        EventLoop * loop,
        const InetAddress & addr,
        const std::string & name)
        : server_(loop,addr,name)
        , loop_(loop)
    {
        // 注册 回调函数
        server_.setConnectioncallback(
            std::bind(&EchoServer::onConnection,this,std::placeholders::_1)
        );

        server_.setMessagecallback(
            std::bind(&EchoServer::onMessage,
                        this,
                        std::placeholders::_1,
                        std::placeholders::_2,
                        std::placeholders::_3
                        )
        );

        // 设置 合适的 loop 线程数量

        server_.setThreadNum(3);

    }

    
    void start()
    {
        server_.start();
    }



private:

    void onConnection(const TcpConnectionPtr & conn)
    {
        if(conn->connected())
        {
            LOG_INFO("connection up :  %s======================OK%%" , conn ->peerAddress().toIpPort().c_str());
        }else
        {
            LOG_INFO("connection Down : %s =================>OK%%" , conn ->peerAddress().toIpPort().c_str());
        }
        
    }

    void onMessage(const TcpConnectionPtr & conn,
                    Buffer * buf,
                    Timestamp time
                )
    {
        std::string msg = buf->retrieveAllasString();
        std::cout << msg << std::endl;
        conn->send(msg);
        conn->shutdown(); //    关闭写端  Epollhub =>  closeCallback_

        LOG_DEBUG("Call =====================> OK");
    }

private:
    EventLoop * loop_;
    TcpServer server_;    
};



int main()
{

    EventLoop loop;         // main loop
    InetAddress addr(8000);

    EchoServer server(&loop,addr , "EchoServer-01");

    LOG_DEBUG("#########################DEBUG MOD###################\n");
    
    server.start();     // listen loopthread listenfd=>acceptChannel => mainloop

    LOG_DEBUG("##################### start Loop ########################### \n");
    loop.loop();        // 启动mainloop 底层的poller


    return 0;
}


