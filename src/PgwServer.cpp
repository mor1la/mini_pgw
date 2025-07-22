#include "PgwServer.h"
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>

PgwServer::PgwServer() {
    loadConfiguration();
    initLogging(loggerSettings.log_file);

    sessionManager = std::make_unique<SessionManager>(
        sessionManagerSettings.timeoutSeconds,
        sessionManagerSettings.blacklist
    );

    cdrWriter = std::make_unique<CdrWriter>(sessionManagerSettings.cdrFilePath);

    udpServer = std::make_unique<UdpServer>(udpSettings, *sessionManager);

    httpServer = std::make_unique<HttpServer>(httpSettings); // временная заглушка
}

PgwServer::~PgwServer() {
    stop();
}

void PgwServer::initLogging(const std::string& logFile) {
    auto file_logger = spdlog::basic_logger_mt("serverLogger", logFile);
    spdlog::set_default_logger(file_logger);
    spdlog::set_pattern("[%Y-%m-%d %H:%M:%S] [%^%l%$] %v");
    spdlog::set_level(spdlog::level::info);

    spdlog::info("Logger initialized. Log file: {}", logFile);
}

void PgwServer::loadConfiguration() {
    ServerConfigLoader serverLoader;
    serverSettings = serverLoader.loadFromFile("../../config/server_config.json");

    StructSplitter splitter;
    udpSettings = splitter.makeUdpSettings(serverSettings);
    httpSettings = splitter.makeHttpSettings(serverSettings);
    loggerSettings = splitter.makeLoggerSettings(serverSettings);
    sessionManagerSettings = splitter.makeSessionManagerSettings(serverSettings);
}

void PgwServer::start() {
    running = true;

    udpThread = std::thread([this]() {
        spdlog::info("Starting UDP server...");
        udpServer->start();
    });

    httpThread = std::thread([this]() {
        spdlog::info("Starting HTTP server...");
        httpServer->start();
    });

    spdlog::info("PGW Server started.");
}

void PgwServer::stop() {
    if (!running) return;

    spdlog::info("Stopping PGW Server...");

    udpServer->stop();
    httpServer->stop();

    if (udpThread.joinable()) udpThread.join();
    if (httpThread.joinable()) httpThread.join();

    running = false;

    spdlog::info("PGW Server stopped.");
}
