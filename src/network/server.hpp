#pragma once

#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/epoll.h>
// for epoll_wait()
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
// for endianness
#include <stdexcept>
#include <cstring>
#include <memory>
#include<unordered_map>
#include "../decorators/istore.hpp"
#include "../protocol/resp_parser.hpp"
#include "../core/command_handler.hpp"

namespace miniRedis {


    constexpr int PORT = 6380;
    constexpr int BACKLOG = 128;
    // depth
    constexpr int MAX_EVENTS = 1024;
    constexpr size_t BUFFER_SIZE = 4096;

    class TcpServer{
    public:
        explicit TcpServer(int port = PORT);
        ~TcpServer();
        void setStore(std::shared_ptr<IStore> store);
        void run();

    private:
        int server_fd_{-1};
        int epoll_fd_{-1};

        std::shared_ptr<IStore> store_;
        std::unique_ptr<CommandHandler> handler_;
        std::unordered_map<int, RespParser> parsers_; // 1 per client fd

        int createAndBindSocket(int port);
        void setNonBlocking(int fd);
        void addToEpoll(int fd, uint32_t events);
        void handleNewConnection();
        void handleClientData(int client_fd);
        void removeClient(int client_fd);
    };
}
