#include "PgwServer.h"
#include "HttpServer.h"  
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>

PgwServer::PgwServer() {
    loadConfiguration();
}

PgwServer::~PgwServer() {
    serverLogger->info("DESTRUCTOR");
    if (running || cleanupRunning) {
        stop();
    }

    udpServer.reset();
    httpServer.reset();
    sessionManager.reset();
    cdrWriter.reset();

    serverLogger->info("DESTRUCTOR");
    //spdlog::drop("serverLogger"); 
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
    httpServer = std::make_unique<HttpServer>(httpSettings, *sessionManager, *cdrWriter);
}


void PgwServer::start() {
    running = true;
    cleanupRunning = true;

    udpThread = std::make_unique<std::thread>([this]() {
        serverLogger->info("Starting UDP server...");
        udpServer->start();
    });

    httpThread = std::make_unique<std::thread>([this]() {
        serverLogger->info("Starting HTTP server...");
        httpServer->start();
    });

    serverLogger->info("PGW Server started.");

    cleanupThread = std::make_unique<std::thread>([this]() {
    serverLogger->info("Starting session cleanup thread...");
    while (cleanupRunning) {
        auto expiredSessions = sessionManager->cleanupExpiredSessions();
        for (const auto& session : expiredSessions) {
            cdrWriter->write(session, CdrWriter::Action::Delete);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(500)); 
    }
    }   
);

}

void PgwServer::stop() {
    if (!running) return;

    serverLogger->info("Stopping PGW Server...");

    running = false;
    cleanupRunning = false;

    serverLogger->info("ALO");

    serverLogger->info("Stopping UDP server...");
    udpServer->stop();
    if (udpThread && udpThread->joinable()) {
        serverLogger->info("Waiting for UDP thread...");
        udpThread->join();
        serverLogger->info("UDP thread finished.");
    }

    serverLogger->info("Stopping HTTP server...");
    httpServer->stop();
    if (httpThread && httpThread->joinable()) {
        serverLogger->info("Waiting for HTTP thread...");
        httpThread->join();
        serverLogger->info("HTTP thread finished.");
    }
    
    if (cleanupThread && cleanupThread->joinable()) cleanupThread->join();

    serverLogger->info("PGW Server stopped.");
}
