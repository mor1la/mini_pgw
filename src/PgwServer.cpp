#include "PgwServer.h"
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>

PgwServer::PgwServer() {
    loadConfiguration();
    initLogging();

    sessionManager = std::make_unique<SessionManager>(
        sessionManagerSettings.timeoutSeconds,
        sessionManagerSettings.blacklist
    );

    cdrWriter = std::make_unique<CdrWriter>(sessionManagerSettings.cdrFilePath);

    udpServer = std::make_unique<UdpServer>(udpSettings, *sessionManager);

    cdrWriter = std::make_unique<CdrWriter>(sessionManagerSettings.cdrFilePath);

    httpServer = std::make_unique<HttpServer>(httpSettings); // временная заглушка
}

PgwServer::~PgwServer() {
    stop();
}

void PgwServer::initLogging() {
    auto file_logger = spdlog::basic_logger_mt("serverLogger", loggerSettings.log_file);
    spdlog::set_default_logger(file_logger);
    spdlog::set_pattern("[%Y-%m-%d %H:%M:%S] [%^%l%$] %v");

    if (loggerSettings.log_level == "DEBUG") {
        spdlog::set_level(spdlog::level::debug);
    } else if (loggerSettings.log_level == "INFO") {
        spdlog::set_level(spdlog::level::info);
    } else if (loggerSettings.log_level == "WARN") {
        spdlog::set_level(spdlog::level::warn);
    } else if (loggerSettings.log_level == "ERROR") {
        spdlog::set_level(spdlog::level::err);
    } else {
        spdlog::set_level(spdlog::level::info); 
    }
    spdlog::flush_on(spdlog::level::info);

    spdlog::info("Logger initialized. Log file: {}, level: {}", loggerSettings.log_file, loggerSettings.log_level);
}


void PgwServer::loadConfiguration() {
    ServerConfigLoader serverLoader;
    auto rawServerSettings = serverLoader.loadFromFile("../../config/server_config.json");

    StructSplitter splitter;
    udpSettings = splitter.makeUdpSettings(rawServerSettings);
    httpSettings = splitter.makeHttpSettings(rawServerSettings);
    loggerSettings = splitter.makeLoggerSettings(rawServerSettings);
    sessionManagerSettings = splitter.makeSessionManagerSettings(rawServerSettings);
}

void PgwServer::start() {
    running = true;

    udpThread = std::thread([this]() {
        spdlog::info("Starting UDP server...");
         std::cout << "[DEBUG] Starting UDP server thread...\n";
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
