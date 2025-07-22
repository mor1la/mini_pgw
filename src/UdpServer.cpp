#include "UdpServer.h"

UdpServer::UdpServer(const UdpServerSettings settings, SessionManager &sessionManager) :
    settings(settings), sessionManager(sessionManager){
        logger = spdlog::get("serverLogger");
        if (!logger) {
            throw std::logic_error("Global serverLogger is not initialized");
        }
    }

void UdpServer::start()
{
    isRunning = true;
    thread = std::thread(&UdpServer::run, this);
}


void UdpServer::stop()
{
    isRunning = false;
    if (socket_fd != -1) {
        close(socket_fd);
        socket_fd = -1;
    }
    if (thread.joinable()) {
        thread.join();
    }
}

void UdpServer::run()
{
    socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (socket_fd < 0) {
        //logger_.log_error("Failed to create UDP socket");
        return;
    }

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(settings.port);
    server_addr.sin_addr.s_addr = inet_addr(settings.ip.c_str());

    if (bind(socket_fd, (sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        //logger_.log_error("Failed to bind UDP socket");
        return;
    }

    //logger_.log_info("UDP server started on port " + std::to_string(settings_.udp_port));

    char buffer[1024];
    while (isRunning) {
        sockaddr_in client_addr{};
        socklen_t client_len = sizeof(client_addr);
        ssize_t recv_len = recvfrom(socket_fd, buffer, sizeof(buffer), 0,
                                    (sockaddr*)&client_addr, &client_len);
        if (recv_len <= 0) continue;

        std::string bcd_imsi(buffer, recv_len);
        std::string imsi = decodeBcd(bcd_imsi);

        //logger_.log_info("Received IMSI: " + imsi);
        std::string response;
        if (!sessionManager.initSession(imsi)) {
            response = Response::failure;
            //cdr_writer_.write_cdr(imsi, "rejected");
            //logger_.log_info("IMSI is failed(подумать над корр формулировкой): " + imsi);
        } else {
            response = Response::success;
            //cdr_writer_.write_cdr(imsi, "created");
            //logger_.log_info("Session created for IMSI: " + imsi);
        }
        
        sendto(socket_fd, response.c_str(), response.size(), 0,
            (sockaddr*)&client_addr, client_len);
    }
    //logger_.log_info("UDP server stopped");
    
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