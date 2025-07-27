#include "PgwServer.h"

PgwServer::PgwServer() {
    loadConfiguration();
}

PgwServer::~PgwServer() {
    if (running || cleanupRunning) {
        stop();
    }

    udpServer.reset();
    httpServer.reset();
    sessionManager.reset();
    cdrWriter.reset();
}


void PgwServer::initLogging(LoggerSettings loggerSettings) {
    static const std::unordered_map<std::string, spdlog::level::level_enum> logLevelMap = {
        {"DEBUG", spdlog::level::debug},
        {"INFO",  spdlog::level::info},
        {"WARN",  spdlog::level::warn},
        {"ERROR", spdlog::level::err}
    };

    serverLogger = spdlog::basic_logger_mt("serverLogger", loggerSettings.logFile);
    serverLogger->set_pattern("[%Y-%m-%d %H:%M:%S] [%^%l%$] %v");

    auto it = logLevelMap.find(loggerSettings.logLevel);
    spdlog::level::level_enum level = (it != logLevelMap.end()) ? it->second : spdlog::level::info;
    serverLogger->set_level(level);

    serverLogger->flush_on(spdlog::level::info);
    serverLogger->info("---------------------------------------");
    serverLogger->info("ServerLogger initialized. Log file: {}, level: {}", loggerSettings.logFile, loggerSettings.logLevel);
}


void PgwServer::loadConfiguration() {
    ServerConfigLoader loader;
    auto raw = loader.loadFromFile(Path::serverConfig);

    StructSplitter splitter;
    auto udpSettings = splitter.makeUdpSettings(raw);
    auto httpSettings = splitter.makeHttpSettings(raw);
    auto loggerSettings = splitter.makeLoggerSettings(raw);
    auto sessionSettings = splitter.makeSessionManagerSettings(raw);

    initLogging(loggerSettings);

    cdrWriter = std::make_unique<CdrWriter>(sessionSettings.cdrFilePath);
    sessionManager = std::make_unique<SessionManager>(sessionSettings, *cdrWriter);
    udpServer = std::make_unique<UdpServer>(udpSettings, *sessionManager);
    httpServer = std::make_unique<HttpServer>(httpSettings, *sessionManager);

    httpServer->setStopCallback([this]() {
        this->terminateRequested = true;
    });

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
    }});

    registerSignalHandlers();

    while (!terminateRequested) {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    stop();

}

void PgwServer::stop() {
    if (!running) return;

    serverLogger->info("Stopping PGW Server...");

    running = false;
    cleanupRunning = false;

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

static PgwServer* instancePtr = nullptr;

void PgwServer::registerSignalHandlers() {
    instancePtr = this;
    std::signal(SIGINT, PgwServer::handleSignal);
    std::signal(SIGTERM, PgwServer::handleSignal);
}

void PgwServer::handleSignal(int signal) {
    if (instancePtr) {
        instancePtr->serverLogger->info("Signal {} received, initiating shutdown", signal);
        instancePtr->terminateRequested = true;
    }
}
