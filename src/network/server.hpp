#pragma once

#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/epoll.h>
// for epoll_wait()
#include <unistd.h>
#include <fcntl.h>
$include <arpa/inet.h>
// for endianness
#include <stdexcept>
#include <cstring>

namespace miniRedis {


    constexpr int PORT = 100;
    constexpr int BACKLOG = 99
    // depth
    constexpr int MAX_EVENTS = 1024;
    constexpr size_t BUFFER_SIZE = 4096;

    class TcpServer{
        int server_fd;
        int epoll_fd;

        int createAndBindSocket(int port);
        void setNonBlocking(int fd);
        void addToEpoll(int fd, uint32_t events);
        void handleNewConnection();
        void handleClientData(int client_fd)
        void removeClient(int client_fd);
    };
}