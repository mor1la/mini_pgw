#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include "./SettingsStructures/HttpApiServerSettings.h"
#include "SessionManager.h"
#include <crow.h>
#include <thread>
#include <atomic>
#include <memory>

class HttpServer {
public:
    HttpServer(const HttpApiServerSettings& settings, SessionManager& sessionManager);
    void start();
    void stop();

private:
    HttpApiServerSettings settings;
    SessionManager& sessionManager;
    std::unique_ptr<std::thread> serverThread;
    std::atomic<bool> running{false};
    crow::SimpleApp app; 
};

#endif
