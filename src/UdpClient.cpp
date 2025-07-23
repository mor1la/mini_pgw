#include "UdpClient.h"

#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <iostream>

UdpClient::UdpClient(const ClientSettings& settings)
    : settings_(settings) {
        logger = spdlog::get("clientLogger");
        if (!logger) {
            throw std::logic_error("Global clientLogger is not initialized");
        }
    }

std::string UdpClient::encode_bcd(const std::string& imsi) {
    std::string result;
    for (size_t i = 0; i < imsi.length(); i += 2) {
        char high = imsi[i];
        char low = (i + 1 < imsi.length()) ? imsi[i + 1] : 'F';
        unsigned char bcd = ((low - '0') & 0x0F) << 4 | ((high - '0') & 0x0F);
        result.push_back(bcd);
    }
    return result;
}

bool UdpClient::send_imsi(const std::string& imsi) {
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        //logger_.log_error("Failed to create socket");
        return false;
    }

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(settings_.server_port);
    inet_pton(AF_INET, settings_.server_ip.c_str(), &server_addr.sin_addr);

    std::string bcd = encode_bcd(imsi);

    //logger_.log_info("Sending IMSI: " + imsi);
    ssize_t sent = sendto(sockfd, bcd.c_str(), bcd.size(), 0,
                          (sockaddr*)&server_addr, sizeof(server_addr));
    if (sent <= 0) {
        //logger_.log_error("Failed to send UDP packet");
        close(sockfd);
        return false;
    }
    char buffer[1024];
    sockaddr_in from_addr{};
    socklen_t from_len = sizeof(from_addr);
    ssize_t received = recvfrom(sockfd, buffer, sizeof(buffer) - 1, 0,
                                (sockaddr*)&from_addr, &from_len);
    close(sockfd);

    if (received <= 0) {
        //logger_.log_error("No response received");
        return false;
    }

    buffer[received] = '\0';
    std::string response(buffer);
    //logger_.log_info("Received response: " + response);

    return true;
}
