#include "UdpServer.h"
//#include <bin_to_hex.h>

UdpServer::UdpServer(const UdpServerSettings settings, SessionManager &sessionManager, CdrWriter &cdrWriter) :
    settings(settings), sessionManager(sessionManager), cdrWriter(cdrWriter) {
        serverLogger = spdlog::get("serverLogger");
        if (!serverLogger) {
            throw std::logic_error("Global serverLogger is not initialized");
        }
    }
 
void UdpServer::start() {
    if (isRunning.load()) {
        serverLogger->warn("UdpServer already running. Ignoring start().");
        return;
    }
    isRunning = true;
    thread = std::make_unique<std::thread>(&UdpServer::run, this);
    
}


void UdpServer::stop() {
    isRunning = false;
    if (epoll_fd != -1) close(epoll_fd);
    if (socket_fd != -1) close(socket_fd);
    if (thread && thread->joinable()) {
        thread->join();
    }
}

void UdpServer::run() {
    socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (socket_fd < 0) {
        serverLogger->error("Failed to create UDP socket");
        return;
    }

    int flags = fcntl(socket_fd, F_GETFL, 0);
    fcntl(socket_fd, F_SETFL, flags | O_NONBLOCK);

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(settings.port);
    server_addr.sin_addr.s_addr = inet_addr(settings.ip.c_str());

    if (bind(socket_fd, (sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        serverLogger->error("Failed to bind UDP socket: {}", strerror(errno));
        return;
    }

    epoll_fd = epoll_create1(0);
    if (epoll_fd < 0) {
        serverLogger->error("epoll_create1 failed: {}", strerror(errno));
        return;
    }

    epoll_event ev{};
    ev.events = EPOLLIN;
    ev.data.fd = socket_fd;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, socket_fd, &ev);

    serverLogger->info("UDP server started on {}:{}", settings.ip, settings.port);

    char buffer[1024];
    epoll_event events[10];

    while (isRunning) {
        int nfds = epoll_wait(epoll_fd, events, 10, 1000);
        for (int i = 0; i < nfds; ++i) {
            if (events[i].data.fd == socket_fd) {
                sockaddr_in client{};
                socklen_t len = sizeof(client);
                ssize_t recv_len = recvfrom(socket_fd, buffer, sizeof(buffer), 0,
                                            (sockaddr*)&client, &len);
                if (recv_len > 0) {
                    std::string bcd_imsi(buffer, recv_len);
                    handleImsi(bcd_imsi, client);
                }
            }
        }
    }
}

void UdpServer::handleImsi(const std::string& bcd_imsi, sockaddr_in& client_addr) {
    std::string imsi = decodeBcd(bcd_imsi);

    if (imsi.empty()) {
        serverLogger->warn("Decoded IMSI is empty. Ignoring request.");
        return;
    }

    std::string response;
    if (!sessionManager.initSession(imsi)) {
        response = "rejected";
        serverLogger->info("IMSI {} rejected from {}", imsi, inet_ntoa(client_addr.sin_addr));
        cdrWriter.write(imsi, CdrWriter::Action::Reject);
    } else {
        response = "created";
        serverLogger->info("Session created for IMSI {}", imsi);
        cdrWriter.write(imsi, CdrWriter::Action::Create);
    }

    sendto(socket_fd, response.c_str(), response.size(), 0,
           (sockaddr*)&client_addr, sizeof(client_addr));
}


std::string UdpServer::decodeBcd(const std::string& data) {
    std::string result;
    for (unsigned char byte : data) {
        int low = byte & 0x0F;
        int high = (byte & 0xF0) >> 4;
        if (low <= 9) result += std::to_string(low);
        if (high <= 9) result += std::to_string(high);
    }
    return result;
}