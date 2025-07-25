#include "SessionManager.h"
#include "./SettingsStructures/HttpApiServerSettings.h"
#include <crow.h>
#include <atomic>
#include <thread>

class HttpServer {
public:
    HttpServer(HttpApiServerSettings settings, SessionManager& sessionManager);
    ~HttpServer();

    void start();
    void stop();

    void setStopCallback(std::function<void()> cb);
private:
    crow::SimpleApp app;
    HttpApiServerSettings settings;
    SessionManager& sessionManager;

    std::atomic<bool> running{false};
    std::thread serverThread;

    std::shared_ptr<spdlog::logger> serverLogger;

    std::function<void()> stopCallback;

    void gracefulOffload();
};
