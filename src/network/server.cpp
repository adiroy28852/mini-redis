#include "server.hpp"

#include <iostream>
#include <cerrno>

namespace miniRedis {
    TcpServer::TcpServer(int port) {
        server_fd = createAndBindSocket(port);
        epoll_fd_ = epoll_create1(0);

        if(epoll_fd_ < 0)throw std::runtime_error("epoll_create1 failed");
        addToEpoll(server_fd, EPOLLIN);
        std::cout << "[mini-redis] Listening n port " << port << "\n";
    }

    int TcpServer::createAndBindSocket(int port){
        // AF)INET = IPv4 Address family
        // SOCK_STREAM = TCP (reliable, ordered, stream based);
        

        // will continue on this after protocol layer. 
    }
}