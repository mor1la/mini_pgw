#include "UdpClient.h"

#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <cstring>
#include <iostream>
#include <basic_file_sink.h>

UdpClient::UdpClient(const ClientSettings& clientSettings)
    : clientSettings(clientSettings) {
    initLogging();
    clientLogger = spdlog::get("clientLogger");
    if (!clientLogger) {
        throw std::logic_error("Global clientLogger is not initialized");
    }
}

std::string UdpClient::encode_bcd(const std::string& imsi) {
    std::string result;
    for (size_t i = 0; i < imsi.length(); i += 2) {
        uint8_t digit1 = imsi[i] - '0';
        uint8_t digit2 = (i + 1 < imsi.length()) ? (imsi[i + 1] - '0') : 0x0F; // Padding

        uint8_t byte = (digit2 << 4) | digit1;  // digit1 = младший, digit2 = старший
        result.push_back(byte);
    }
    return result;
}

void UdpClient::initLogging() {
    clientLogger = spdlog::basic_logger_mt("clientLogger", clientSettings.log_file);
    clientLogger->set_pattern("[%Y-%m-%d %H:%M:%S] [%^%l%$] %v");
    
    if (clientSettings.log_level == "DEBUG") {
        clientLogger->set_level(spdlog::level::debug);
    } else if (clientSettings.log_level == "INFO") {
        clientLogger->set_level(spdlog::level::info);
    } else if (clientSettings.log_level == "WARN") {
        clientLogger->set_level(spdlog::level::warn);
    } else if (clientSettings.log_level == "ERROR") {
        clientLogger->set_level(spdlog::level::err);
    } else {
        clientLogger->set_level(spdlog::level::info); 
    }
    clientLogger->flush_on(spdlog::level::info);
    clientLogger->info("---------------------------------------");
    clientLogger->info("ClientLogger initialized. Log file: {}, level: {}", clientSettings.log_file, clientSettings.log_level);
}

bool UdpClient::send_imsi(const std::string& imsi) {
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        clientLogger->error("Failed to create UDP socket: {}", strerror(errno));
        return false;
    }

    // Установка сокета в неблокирующий режим
    int flags = fcntl(sockfd, F_GETFL, 0);
    if (fcntl(sockfd, F_SETFL, flags | O_NONBLOCK) < 0) {
        clientLogger->error("Failed to set socket to non-blocking: {}", strerror(errno));
        close(sockfd);
        return false;
    }

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(clientSettings.server_port);
    if (inet_pton(AF_INET, clientSettings.server_ip.c_str(), &server_addr.sin_addr) <= 0) {
        clientLogger->error("Invalid server IP address: {}", clientSettings.server_ip);
        close(sockfd);
        return false;
    }

    std::string bcd = encode_bcd(imsi);
    clientLogger->info("Sending IMSI: {}", imsi);

    ssize_t sent = sendto(sockfd, bcd.c_str(), bcd.size(), 0,
                          (sockaddr*)&server_addr, sizeof(server_addr));
    if (sent <= 0) {
        clientLogger->error("Failed to send UDP packet: {}", strerror(errno));
        close(sockfd);
        return false;
    }

    // Настройка epoll
    int epoll_fd = epoll_create1(0);
    if (epoll_fd < 0) {
        clientLogger->error("epoll_create1 failed: {}", strerror(errno));
        close(sockfd);
        return false;
    }

    epoll_event ev{};
    ev.events = EPOLLIN;
    ev.data.fd = sockfd;

    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, sockfd, &ev) < 0) {
        clientLogger->error("epoll_ctl failed: {}", strerror(errno));
        close(epoll_fd);
        close(sockfd);
        return false;
    }

    // Ожидание ответа от сервера
    epoll_event events[1];
    int nfds = epoll_wait(epoll_fd, events, 1, 2000); // timeout 2000ms

    if (nfds == 0) {
        clientLogger->error("Timeout: no response received from server");
        close(epoll_fd);
        close(sockfd);
        return false;
    } else if (nfds < 0) {
        clientLogger->error("epoll_wait failed: {}", strerror(errno));
        close(epoll_fd);
        close(sockfd);
        return false;
    }

    char buffer[1024];
    sockaddr_in from_addr{};
    socklen_t from_len = sizeof(from_addr);
    ssize_t received = recvfrom(sockfd, buffer, sizeof(buffer) - 1, 0,
                                (sockaddr*)&from_addr, &from_len);

    if (received <= 0) {
        clientLogger->error("Failed to receive response: {}", strerror(errno));
        close(epoll_fd);
        close(sockfd);
        return false;
    }

    buffer[received] = '\0';
    std::string response(buffer);
    clientLogger->info("Received response: {}", response);

    close(epoll_fd);
    close(sockfd);
    return true;
}
