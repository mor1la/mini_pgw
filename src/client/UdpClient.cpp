#include "UdpClient.h"

UdpClient::UdpClient()
    : clientSettings(clientSettings) {
    loadConfiguration();
    clientLogger = spdlog::get("clientLogger");
    if (!clientLogger) {
        throw std::logic_error("Global clientLogger is not initialized");
    }
}

std::string UdpClient::encodeBcd(const std::string &imsi) {
    std::string result;
    for (size_t i = 0; i < imsi.length(); i += 2) {
        if (!isdigit(imsi[i]) || (i + 1 < imsi.length() && !isdigit(imsi[i + 1]))) {
            throw std::invalid_argument("IMSI contains non-digit characters");
        }

        uint8_t digit1 = imsi[i] - '0';
        uint8_t digit2 = (i + 1 < imsi.length()) ? (imsi[i + 1] - '0') : 0x0F;

        uint8_t byte = (digit2 << 4) | digit1;
        result.push_back(byte);
    }
    return result;
}

ClientSettings UdpClient::getClientSettings() {
    return ClientSettings();
}

void UdpClient::initLogging() {
    static const std::unordered_map<std::string, spdlog::level::level_enum> logLevelMap = {
        {"DEBUG", spdlog::level::debug},
        {"INFO",  spdlog::level::info},
        {"WARN",  spdlog::level::warn},
        {"ERROR", spdlog::level::err}
    };

    clientLogger = spdlog::basic_logger_mt("clientLogger", clientSettings.logFile);
    clientLogger->set_pattern("[%Y-%m-%d %H:%M:%S] [%^%l%$] %v");

    auto it = logLevelMap.find(clientSettings.logLevel);
    spdlog::level::level_enum level = (it != logLevelMap.end()) ? it->second : spdlog::level::info;
    clientLogger->set_level(level);

    clientLogger->flush_on(spdlog::level::info);
    clientLogger->info("---------------------------------------");
    clientLogger->info("ClientLogger initialized. Log file: {}, level: {}", clientSettings.logFile, clientSettings.logLevel);
}

bool UdpClient::sendImsi(const std::string &imsi) {
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        clientLogger->error("Failed to create UDP socket: {}", strerror(errno));
        return false;
    }

    int flags = fcntl(sockfd, F_GETFL, 0);
    if (fcntl(sockfd, F_SETFL, flags | O_NONBLOCK) < 0) {
        clientLogger->error("Failed to set socket to non-blocking: {}", strerror(errno));
        close(sockfd);
        return false;
    }

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(clientSettings.serverPort);
    if (inet_pton(AF_INET, clientSettings.serverIp.c_str(), &server_addr.sin_addr) <= 0) {
        clientLogger->error("Invalid server IP address: {}", clientSettings.serverIp);
        close(sockfd);
        return false;
    }

    std::string bcd = encodeBcd(imsi);
    clientLogger->info("Trying to send UDP packet: IMSI({})", imsi);

    ssize_t sent = sendto(sockfd, bcd.c_str(), bcd.size(), 0,
                          (sockaddr*)&server_addr, sizeof(server_addr));
    if (sent <= 0) {
        clientLogger->error("Failed to send UDP packet: {}", strerror(errno));
        close(sockfd);
        return false;
    }

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

    epoll_event events[1];
    int nfds = epoll_wait(epoll_fd, events, 1, 2000); 

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

void UdpClient::loadConfiguration() {
    ClientConfigLoader loader;
    clientSettings = loader.loadFromFile(Path::clientConfig);

    initLogging();
}