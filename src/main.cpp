#include <iostream>
#include <thread>
#include <chrono>
#include <csignal>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>

#include "PgwServer.h"
#include "UdpClient.h"
#include "./ConfigLoader/ClientConfigLoader.h"

std::unique_ptr<PgwServer> server;

void signalHandler(int signum) {
    spdlog::warn("Interrupt signal ({}) received. Stopping server...", signum);
    if (server) {
        server->stop();
    }
    std::_Exit(signum);  // экстренный выход
}

int main() {
    try {
        // === Установить логгер для старта ===
        auto logger = spdlog::basic_logger_mt("startupLogger", "startup_log.txt");
        spdlog::set_default_logger(logger);
        spdlog::set_pattern("[%Y-%m-%d %H:%M:%S] [%^%l%$] %v");
        spdlog::flush_on(spdlog::level::info);
        spdlog::set_level(spdlog::level::info);

        spdlog::info("Starting PGW server...");


        // === Запускаем сервер ===
        server = std::make_unique<PgwServer>();
        server->start();

        // === Ждём немного, чтобы сервер запустился ===
        std::this_thread::sleep_for(std::chrono::seconds(1));

        // === Загружаем конфиг клиента ===
        ClientConfigLoader clientLoader;
        ClientSettings clientSettings = clientLoader.loadFromFile("../../config/client_config.json");
        auto file_logger = spdlog::basic_logger_mt("clientLogger", clientSettings.log_file);
        file_logger->set_pattern("[%Y-%m-%d %H:%M:%S] [%^%l%$] %v");
        file_logger->set_level(spdlog::level::info); // задается
        file_logger->flush_on(spdlog::level::info);


        // === Тестовый IMSI ===
        std::string testImsi1 = "001010123456789";
        std::string testImsi2 = "001010123456786";

        UdpClient udpClient1(clientSettings);
        UdpClient udpClient2(clientSettings);

        if (udpClient1.send_imsi(testImsi1)) {
            file_logger->info("IMSI {} sent successfully", testImsi1);
        } else {
            file_logger->error("Failed to send IMSI {}", testImsi1);
        }

        if (udpClient2.send_imsi(testImsi2)) {
            file_logger->info("IMSI {} sent successfully", testImsi2);
        } else {
            file_logger->error("Failed to send IMSI {}", testImsi2);
        }

        // Подождать немного, чтобы сервер успел обработать
        //std::this_thread::sleep_for(std::chrono::seconds(40));

        // === Завершить работу ===
        server->stop();
        spdlog::info("PGW server stopped cleanly");

    } catch (const std::exception& e) {
        spdlog::critical("Fatal error: {}", e.what());
        return 1;
    }

    return 0;
}
