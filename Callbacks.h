#pragma once 


#include <memory>
#include <functional>


namespace mymuduo
{

    class Buffer;
    class TcpConnection;
    class Timestamp;

    using TcpConnectionPtr = std::shared_ptr<TcpConnection>;
    using ConnectionCallback =  std::function<void (const TcpConnectionPtr&)>;
    using CloseCallback = std::function<void(const TcpConnectionPtr&)>;
    using WriteComplateCallback =  std::function<void (const TcpConnectionPtr&)>;


    using MessageCallback = std::function<void (const TcpConnectionPtr& , Buffer * ,Timestamp)>;

    using HightWateMarkCallback = std::function<void (const TcpConnectionPtr&,size_t)>;
    
    

} // namespace mymuduo
