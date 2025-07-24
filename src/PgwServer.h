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
#include "HttpServer.h"

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

    std::unique_ptr<std::thread> udpThread;
    std::unique_ptr<std::thread> httpThread;
    std::unique_ptr<std::thread> cleanupThread;

    std::atomic<bool> running{false};
    std::atomic<bool> cleanupRunning{false};

    std::shared_ptr<spdlog::logger> serverLogger;
};

#endif 




