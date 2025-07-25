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
    serverLogger->info("Starting HTTP server on port {}", settings.port);
    running = true;
    
    // /check_subscriber?imsi=...
    CROW_ROUTE(app, "/check_subscriber")
    ([this](const crow::request& req) {
        auto imsi = req.url_params.get("imsi");
        if (!imsi) {
            serverLogger->warn("/check_subscriber called without 'imsi' parameter");
            return crow::response(400, "Missing imsi");
        }
    
        std::string status = sessionManager.hasSession(imsi) ? "active" : "not active";
        serverLogger->info("/check_subscriber called for IMSI: {}, status: {}", imsi, status);
        return crow::response(200, status);
    });

    // /stop
    CROW_ROUTE(app, "/stop")
    ([this]() {
        std::thread(&HttpServer::gracefulOffload, this).detach();
        return crow::response(200, "Shutting down");
    });

    serverThread = std::thread([this]() {
        try {
            app.port(settings.port).multithreaded().concurrency(4).run();
        } catch (const std::exception& e) {
            serverLogger->error("Exception in HTTP server thread: {}", e.what());
        }
    });
}

void HttpServer::stop() {
    if (!running) {
        serverLogger->warn("HttpServer::stop called but server is not running");
        return;
    } 

    running = false;
    app.stop(); 

    if (serverThread.joinable()) {
        serverThread.join();
    }
}

void HttpServer::gracefulOffload() {
    serverLogger->info("Graceful offload started...");

    std::chrono::milliseconds delay(1000 / settings.gracefulShutdownRate);

    auto sessions = sessionManager.getAllSessions();
    std::vector<std::string> imsiList;
    for (const auto& [imsi, _] : sessions) {
        imsiList.push_back(imsi);
    }

    for (const auto& imsi : imsiList) {
        sessionManager.offloadSession(imsi);
        std::this_thread::sleep_for(delay);
    }

    if (stopCallback) {
        stopCallback();
    }

    serverLogger->info("Graceful offload completed. {} session(s) removed.", imsiList.size());
}

void HttpServer::setStopCallback(std::function<void()> cb) {
    serverLogger->debug("Stop callback set");
    stopCallback = std::move(cb);
}
