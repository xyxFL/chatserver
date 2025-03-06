#include "chatserver.hpp"
#include <iostream>
#include <functional>
#include <string>
#include "json.hpp"
#include "chatservice.hpp"

using namespace std;
using json = nlohmann::json;

ChatServer::ChatServer(EventLoop *loop,
                       const InetAddress &listenAddr,
                       const string &nameArg)
    : _server(loop, listenAddr, nameArg), _loop(loop)
{
    // 注册有关连接的回调函数
    _server.setConnectionCallback(bind(&ChatServer::onConnection, this, placeholders::_1));

    // 注册有关消息的回调函数
    _server.setMessageCallback(bind(&ChatServer::onMessage, this, placeholders::_1, placeholders::_2, placeholders::_3));

    // 设置服务器端的线程数量：1个I/O线程，3个worker线程
    _server.setThreadNum(4);
}

// 启动服务
void ChatServer::start()
{
    _server.start();
}

// 上报连接相关信息的回调函数
void ChatServer::onConnection(const TcpConnectionPtr &conn)
{
    // 客户端断开连接
    if(!conn->connected())
    {
        ChatService::instance()->clientCloseException(conn);
        conn->shutdown();
    }
}

// 上报读写事件相关信息的回调函数
void ChatServer::onMessage(const TcpConnectionPtr &conn, // 连接
                           Buffer *buffer,               // 缓冲区
                           Timestamp time)               // 接收到数据的时间信息
{
    string buf = buffer->retrieveAllAsString();
    // 数据的反序列化
    json js = json::parse(buf);
    // 目的：完全解耦网络模块和业务模块的代码
    // 通过js["msgid"]获取业务处理器handler
    auto msgHandler = ChatService::instance()->getHandler(js["msgid"].get<int>());
    // 利用回调消息绑定好的事件处理器，来执行相应的业务逻辑
    msgHandler(conn, js, time);
}