#ifndef PGWSERVER_H
#define PGWSERVER_H

#include "StructSplitter.h"
#include "UdpServer.h"
#include "SessionManager.h"
#include "CdrWriter.h"
#include "HttpServer.h" 
#include "./Consts/PathConsts.h"
#include "./ConfigLoader/ServerConfigLoader.h" 
#include <memory>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>

class PgwServer {
public:
    PgwServer();
    ~PgwServer();

    void start();
    void stop();

private:
    void initLogging(LoggerSettings loggerSettings);
    void loadConfiguration();
    void registerSignalHandlers();
    static void handleSignal(int signal);

    std::unique_ptr<SessionManager> sessionManager;
    std::unique_ptr<CdrWriter> cdrWriter;
    std::unique_ptr<UdpServer> udpServer;
    std::unique_ptr<HttpServer> httpServer;

    std::unique_ptr<std::thread> udpThread;
    std::unique_ptr<std::thread> httpThread;
    std::unique_ptr<std::thread> cleanupThread;

    std::atomic<bool> running{false};
    std::atomic<bool> cleanupRunning{false};
    std::atomic<bool> terminateRequested{false};

    std::shared_ptr<spdlog::logger> serverLogger;
};

#endif 




