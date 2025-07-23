#ifndef UDPSERVER_H
#define UDPSERVER_H
#include <thread>
#include <memory>
#include <spdlog/spdlog.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <atomic>
#include "./SettingsStructures/UdpServerSettings.h"
#include "SessionManager.h"
#include "./Consts/ResponseConsts.h"
#include <iostream>
#include "CdrWriter.h"


class UdpServer {
public:
    UdpServer(const UdpServerSettings settings, SessionManager& sessionManager);
    void start();
    void stop();
    
private:
    void run();
    void handleImsi(const std::string &bcd_imsi, sockaddr_in &client_addr);
    std::string decodeBcd(const std::string &data);

    const UdpServerSettings settings;
    SessionManager& sessionManager;
    CdrWriter& cdrWriter;
    std::shared_ptr<spdlog::logger> logger;
    std::thread thread;
    int socket_fd{-1};
    int epoll_fd{-1};
    std::atomic<bool> isRunning{false};

};  

#endif