#include "SessionManager.h"
#include "./SettingsStructures/HttpApiServerSettings.h"
#include <crow.h>
#include <atomic>
#include <thread>
#include "CdrWriter.h"

class HttpServer {
public:
    HttpServer(HttpApiServerSettings settings, SessionManager& sessionManager, CdrWriter &cdrWriter);
    ~HttpServer();

    void start();
    void stop();

private:
    crow::SimpleApp app;
    HttpApiServerSettings settings;
    SessionManager& sessionManager;
    CdrWriter& cdrWriter;

    std::atomic<bool> running{false};
    std::thread serverThread;

    std::shared_ptr<spdlog::logger> serverLogger;

    void gracefulOffload();
};
