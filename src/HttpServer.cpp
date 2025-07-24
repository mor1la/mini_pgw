#include "HttpServer.h"
#include <spdlog/spdlog.h>
#include <chrono>
#include <thread>

HttpServer::HttpServer(const HttpApiServerSettings& settings, SessionManager& sessionManager)
    : settings(settings), sessionManager(sessionManager) {}

void HttpServer::start() {
    running = true;

    auto serverLogger = spdlog::get("serverLogger");

    // Регистрируем маршруты заранее
    CROW_ROUTE(app, "/check_subscriber")
    ([this, serverLogger](const crow::request& req) {
        auto imsi = req.url_params.get("imsi");
        if (!imsi) {
            serverLogger->warn("Missing IMSI in /check_subscriber request");
            return crow::response(400, "Missing IMSI");
        }

        std::string imsiStr(imsi);
        bool active = sessionManager.hasSession(imsiStr);
        serverLogger->info("Check subscriber for IMSI {}: {}", imsiStr, active ? "active" : "not active");

        return crow::response(active ? "active" : "not active");
    });

    CROW_ROUTE(app, "/stop")
    ([this, serverLogger]() {
        serverLogger->info("/stop requested, starting graceful shutdown...");
        running = false;

        int rate = settings.gracefulShutdownRate;
        int delayMs = 1000 / (rate > 0 ? rate : 1);

        std::vector<std::string> allSessions;
        for (const auto& pair : sessionManager.getAllSessions()) {
            allSessions.push_back(pair.first);
        }

        for (const auto& imsi : allSessions) {
            sessionManager.removeSession(imsi);
            serverLogger->info("Gracefully removed session: {}", imsi);
            std::this_thread::sleep_for(std::chrono::milliseconds(delayMs));
        }

        serverLogger->info("Graceful shutdown complete");
        return crow::response("Shutting down...");
    });

    serverThread = std::make_unique<std::thread>([this]() {
        app.port(settings.port).run();
    });
}

void HttpServer::stop() {
    if (running) {
        running = false;
        if (serverThread && serverThread->joinable()) {
            serverThread->join();
        }
    }
}
