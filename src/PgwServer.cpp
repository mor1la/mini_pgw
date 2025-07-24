#include "PgwServer.h"
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>

PgwServer::PgwServer() {
    loadConfiguration();
}

PgwServer::~PgwServer() {
    stop();
}

void PgwServer::initLogging(LoggerSettings loggerSettings) {
    serverLogger = spdlog::basic_logger_mt("serverLogger", loggerSettings.log_file);
    serverLogger->set_pattern("[%Y-%m-%d %H:%M:%S] [%^%l%$] %v");
    
    if (loggerSettings.log_level == "DEBUG") {
        serverLogger->set_level(spdlog::level::debug);
    } else if (loggerSettings.log_level == "INFO") {
        serverLogger->set_level(spdlog::level::info);
    } else if (loggerSettings.log_level == "WARN") {
        serverLogger->set_level(spdlog::level::warn);
    } else if (loggerSettings.log_level == "ERROR") {
        serverLogger->set_level(spdlog::level::err);
    } else {
        serverLogger->set_level(spdlog::level::info); 
    }
    serverLogger->flush_on(spdlog::level::info);
    serverLogger->info("---------------------------------------");
    serverLogger->info("ServerLogger initialized. Log file: {}, level: {}", loggerSettings.log_file, loggerSettings.log_level);
}

void PgwServer::loadConfiguration() {
    ServerConfigLoader loader;
    auto raw = loader.loadFromFile("../../config/server_config.json");

    StructSplitter splitter;
    auto udpSettings = splitter.makeUdpSettings(raw);
    auto httpSettings = splitter.makeHttpSettings(raw);
    auto loggerSettings = splitter.makeLoggerSettings(raw);
    auto sessionSettings = splitter.makeSessionManagerSettings(raw);

    initLogging(loggerSettings);

    sessionManager = std::make_unique<SessionManager>(
        sessionSettings.timeoutSeconds,
        sessionSettings.blacklist
    );
    cdrWriter = std::make_unique<CdrWriter>(sessionSettings.cdrFilePath);
    udpServer = std::make_unique<UdpServer>(udpSettings, *sessionManager, *cdrWriter);
    httpServer = std::make_unique<HttpServer>(httpSettings);
}


void PgwServer::start() {
    running = true;
    cleanupRunning = true;

    udpThread = std::thread([this]() {
        serverLogger->info("Starting UDP server...");
        udpServer->start();
    });

    httpThread = std::thread([this]() {
        serverLogger->info("Starting HTTP server...");
        httpServer->start();
    });

    serverLogger->info("PGW Server started.");

    cleanupThread = std::thread([this]() {
    serverLogger->info("Starting session cleanup thread...");
    while (cleanupRunning) {
        auto expiredSessions = sessionManager->cleanupExpiredSessions();
        for (const auto& session : expiredSessions) {
            cdrWriter->write(session, CdrWriter::Action::Delete);
        }
        std::this_thread::sleep_for(std::chrono::seconds(1)); 
    }
    }   
);

}

void PgwServer::stop() {
    if (!running) return;

    serverLogger->info("Stopping PGW Server...");

    running = false;
    cleanupRunning = false;

    udpServer->stop();
    httpServer->stop();


    if (udpThread.joinable()) udpThread.join();
    if (httpThread.joinable()) httpThread.join();
    if (cleanupThread.joinable()) cleanupThread.join();


    serverLogger->info("PGW Server stopped.");


}
