#ifndef PGWSERVER_H
#define PGWSERVER_H

#include <memory>
#include <thread>
#include "UdpServer.h"
#include "SessionManager.h"
#include "CdrWriter.h"
#include "./ConfigLoader/ServerConfigLoader.h"
#include "./ConfigLoader/ClientConfigLoader.h"
#include "StructSplitter.h"


// Заглушка для будущего HTTP-сервера
class HttpServer {
public:
    HttpServer(const HttpApiServerSettings&) {}
    void start() {}
    void stop() {}
};

class PgwServer {
public:
    PgwServer();
    ~PgwServer();

    void start();
    void stop();

private:
    void initLogging(LoggerSettings loggerSettings);
    void loadConfiguration();

    std::unique_ptr<SessionManager> sessionManager;
    std::unique_ptr<CdrWriter> cdrWriter;
    std::unique_ptr<UdpServer> udpServer;
    std::unique_ptr<HttpServer> httpServer;

    std::thread udpThread;
    std::thread httpThread;

    std::atomic<bool> running{false};

    std::thread cleanupThread;
    std::atomic<bool> cleanupRunning{false};

    std::shared_ptr<spdlog::logger> serverLogger;
};

#endif // PGWSERVER_H




