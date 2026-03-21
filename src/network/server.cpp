#include "server.hpp"

#include <iostream>
#include <cerrno>

namespace miniRedis {
    TcpServer::TcpServer(int port) {
        server_fd_ = createAndBindSocket(port);
        epoll_fd_ = epoll_create1(0);

        if(epoll_fd_ < 0)throw std::runtime_error("epoll_create1 failed");
        addToEpoll(server_fd_, EPOLLIN);
        std::cout << "[mini-redis] Listening on port " << port << "\n";
    }

    TcpServer::~TcpServer() {
        if (server_fd_ >= 0) ::close(server_fd_);
        if (epoll_fd_ >= 0) ::close(epoll_fd_);
    }

    int TcpServer::createAndBindSocket(int port){
        // AF)INET = IPv4 Address family
        // SOCK_STREAM = TCP (reliable, ordered, stream based);
        // 0 = auto select the protocol. default TCP for stream

        int fd = socket(AF_INET, SOCK_STREAM, 0);
        if(fd < 0) throw std::runtime_error("socket() failed");

        // SO_REUSEADDR: lets rebind this instanntly after restart
        // without this, os holds the port for +-2min 
        int opt = 1;
        setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

        sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        //big-endian (network byte order)
        addr.sin_addr.s_addr = INADDR_ANY; // bind to all interfaces. 0.0.0.0

        if(bind(fd, (sockaddr*)&addr, sizeof(addr)) < 0) {
            throw std::runtime_error("bind() failed, port is being used?");
        }

        if(listen(fd, BACKLOG) < 0) {
            throw std::runtime_error("listen() failed");
        }

        setNonBlocking(fd);
        return fd;
    }

    void TcpServer::addToEpoll(int fd, uint32_t events) {
        epoll_event ev{};
        ev.events = events;
        ev.data.fd = fd;
        epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, fd, &ev);
    }

    void TcpServer::setNonBlocking(int fd) {
        // O_NONBLOCK: read or write return immediately instead of blocking
        // so that single thread = multiple clients
        int flags= fcntl(fd, F_GETFL, 0);
        fcntl(fd, F_SETFL, flags | O_NONBLOCK);
    }

    void TcpServer::run() {
        epoll_event events[MAX_EVENTS];
        while(true) {
            // epoll_wait : block until atleast 1 fd ready
            // -1 timeout = wait forever

            int n = epoll_wait(epoll_fd_, events, MAX_EVENTS, -1);
            for(int i = 0; i < n; i++) {
                if(events[i].data.fd == server_fd_){
                    handleNewConnection();
                }
                else {
                    handleClientData(events[i].data.fd);
                }
            }
        }
    }
    void TcpServer::handleNewConnection() {
        sockaddr_in client_addr{};
        socklen_t len = sizeof(client_addr);
        int client_fd = accept(server_fd_, (sockaddr*)&client_addr, &len);
        if(client_fd < 0)return; // no more blocking
        setNonBlocking(client_fd);
        addToEpoll(client_fd, EPOLLIN | EPOLLET); //epollet is edge triggered

        std::cout<< "[+] client " << client_fd << " connected\n";
    }

    void TcpServer::handleClientData(int client_fd) {
        char buf[BUFFER_SIZE];
        ssize_t n = read(client_fd, buf, sizeof(buf));
        if (n <= 0) {
            removeClient(client_fd);
            return;
        }
        // TODO: feed buf[0..n) into RespParser and dispatch commands
        (void)n;
    }

    void TcpServer::removeClient(int client_fd) {
        epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, client_fd, nullptr);
        close(client_fd);
        std::cout << "[-] client " << client_fd << " disconnected\n";
    }
}
