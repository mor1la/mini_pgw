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
