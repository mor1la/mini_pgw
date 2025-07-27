#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include "SessionManager.h"
#include "./SettingsStructures/HttpApiServerSettings.h"
#include <crow.h>
#include <atomic>
#include <thread>
#include <chrono>
#include <sstream>

class HttpServer {
public:
    HttpServer(HttpApiServerSettings settings, SessionManager& sessionManager);
    ~HttpServer();

    void start();
    void stop();

    void setStopCallback(std::function<void()> cb);
private:
    void gracefulOffload();
    std::function<void()> stopCallback;

    crow::SimpleApp app;
    HttpApiServerSettings settings;
    SessionManager& sessionManager;
    std::atomic<bool> running{false};
    std::thread serverThread;
    std::shared_ptr<spdlog::logger> serverLogger;
};

#endif
