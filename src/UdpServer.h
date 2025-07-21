#ifndef UDPSERVER_H
#define UDPSERVER_H
#include <thread>
#include <memory>
#include <spdlog/spdlog.h>
#include "./SettingsStructures/UdpServerSettings.h"
#include "SessionManager.h"


class UdpServer {
public:
    UdpServer(const UdpServerSettings settings, SessionManager& sessionManager);
    void start();

private:
    void run();


    const UdpServerSettings settings;
    SessionManager& sessionManager;
    std::shared_ptr<spdlog::logger> logger;
    std::thread thread;

};

#endif