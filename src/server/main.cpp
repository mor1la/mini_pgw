#include "PgwServer.h"
#include <csignal>
#include <atomic>
#include <iostream>

std::atomic<bool> terminateRequested(false);
PgwServer* serverPtr = nullptr;

void signalHandler(int signum) {
    std::cout << "\nSignal " << signum << " received. Shutting down PGW Server...\n";
    terminateRequested = true;
    if (serverPtr) {
        serverPtr->stop();
    }
}

int main() {
    try {
        PgwServer server;
        serverPtr = &server;

        // Обработка Ctrl+C и других сигналов завершения
        std::signal(SIGINT, signalHandler);
        std::signal(SIGTERM, signalHandler);

        server.start();

        // Ожидание сигнала завершения
        while (!terminateRequested) {
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }

        // Stop уже вызван из signalHandler, но можно и здесь на всякий случай
        server.stop();

    } catch (const std::exception& ex) {
        std::cerr << "Server initialization failed: " << ex.what() << std::endl;
        return 1;
    }

    return 0;
}
