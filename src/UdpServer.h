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


class UdpServer {
public:
    UdpServer(const UdpServerSettings settings, SessionManager& sessionManager);
    void start();
    void stop();
    
private:
    void run();
    std::string decodeBcd(const std::string& data);

    const UdpServerSettings settings;
    SessionManager& sessionManager;
    std::shared_ptr<spdlog::logger> logger;
    std::thread thread;
    int socket_fd;
    std::atomic<bool> isRunning{false};

};  

#endif