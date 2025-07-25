#include "HttpServer.h"

HttpServer::HttpServer(HttpApiServerSettings settings, SessionManager& sessionManager)
    : settings(std::move(settings)), sessionManager(sessionManager) {
        serverLogger = spdlog::get("serverLogger");
        if (!serverLogger) {
            throw std::logic_error("Global serverLogger is not initialized");
        }
    }

HttpServer::~HttpServer() {
    stop();
}

void HttpServer::start() {
    running = true;

    // /check_subscriber?imsi=...
    CROW_ROUTE(app, "/check_subscriber")
    ([this](const crow::request& req) {
        auto imsi = req.url_params.get("imsi");
        if (!imsi) {
            return crow::response(400, "Missing imsi");
        }

        std::string status = sessionManager.hasSession(imsi) ? "active" : "not active";
        return crow::response(200, status);
    });

    // /stop
    CROW_ROUTE(app, "/stop")
    ([this]() {
        std::thread(&HttpServer::gracefulOffload, this).detach();
        return crow::response(200, "Shutting down");
    });

    serverThread = std::thread([this]() {
        app.port(settings.port).multithreaded().concurrency(4).run();
    });
}

void HttpServer::stop() {
    if (!running) return;
    running = false;

    app.stop(); 

    if (serverThread.joinable()) {
        serverThread.join();
    }
}

void HttpServer::gracefulOffload() {

    serverLogger->info("Graceful offload started...");

    constexpr std::chrono::milliseconds offloadDelay(200);

    auto sessions = sessionManager.getAllSessions();
    std::vector<std::string> imsiList;
    for (const auto& [imsi, _] : sessions) {
        imsiList.push_back(imsi);
    }

    for (const auto& imsi : imsiList) {
        sessionManager.offloadSession(imsi);
        std::this_thread::sleep_for(offloadDelay);
    }

    if (stopCallback) {
        stopCallback();  
    }

    serverLogger->info("Graceful offload completed. {} session(s) removed.", imsiList.size());
}


void HttpServer::setStopCallback(std::function<void()> cb) {
    stopCallback = std::move(cb);
}
