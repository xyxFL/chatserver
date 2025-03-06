# chatserver
可以工作在nginx tcp负载均衡环境中的集群聊天服务器和客户端源码，基于muduo网络库、mysql数据库和redis消息队列中间件实现。

编译方式
cd build
rm -rf *
cmake    # 生成makefile文件
make     # 编译makefile文件
