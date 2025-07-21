#include <iostream>
#include <thread>
#include <chrono>
#include <unordered_set>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include "SessionManager.h"
#include "StructSplitter.h"
#include "UdpServer.h"
#include "UdpClient.h"
#include "./SettingsStructures/RawServerSettings.h"
#include "./SettingsStructures/ClientSettings.h"
#include "./ConfigLoader/ServerConfigLoader.h"
#include "./ConfigLoader/ClientConfigLoader.h"
#include <spdlog/sinks/basic_file_sink.h>


int main() {

    auto file_logger = spdlog::basic_logger_mt("serverLogger", "server_log.txt"); 

    spdlog::set_default_logger(file_logger);
    spdlog::set_pattern("[%Y-%m-%d %H:%M:%S] [%^%l%$] %v");
    spdlog::set_level(spdlog::level::info);

    spdlog::info("Logger initialized, application starting...");

    // try {
    //     auto serverSettings = loader.loadFromFile("../../config/server_config.json");  
    // } catch (const std::exception& e) {
    //     std::cerr << "Failed to load server config: " << e.what() << std::endl;
    //     return 1;
    // }
    // try {
    //     ClientConfigLoader loader;
    //     ClientSettings clientSettings = loader.loadFromFile("../../config/client_config.json");
    // } catch (const std::exception& e) {
    //     std::cerr << "Failed to load client config: " << e.what() << std::endl;
    //     return 1;
    // }

    ServerConfigLoader serverLoader;
    auto serverSettings = serverLoader.loadFromFile("../../config/server_config.json"); 
    ClientConfigLoader clientLoader;
    ClientSettings clientSettings = clientLoader.loadFromFile("../../config/client_config.json");

    
    StructSplitter splitter;
    auto udpServerSettings = splitter.makeUdpSettings(serverSettings);

    // Шаг 1: создаём чёрный список
    std::unordered_set<std::string> blacklist = {
        "001010123456789", // заблокированный
        "001010000000001"  // заблокированный
    };


    // Шаг 2: создаём SessionManager с таймаутом 2 секунды
    SessionManager manager(2, blacklist);

    // Шаг 3: пробуем создать сессию для разрешённого IMSI
    std::string allowedImsi = "001010999999999";
    std::string blockedImsi = "001010123456789";

    if (manager.initSession(allowedImsi)) {
        std::cout << "Сессия создана для IMSI " << allowedImsi << "\n";
    } else {
        std::cout << "Сессия НЕ создана для IMSI " << allowedImsi << "\n";
    }

    // Шаг 4: пробуем создать сессию для запрещённого IMSI
    if (manager.initSession(blockedImsi)) {
        std::cout << "Сессия создана для IMSI " << blockedImsi << " (ошибка)\n";
    } else {
        std::cout << "Сессия отклонена (чёрный список) для IMSI " << blockedImsi << "\n";
    }


    // Шаг 6: ждём, чтобы таймаут сработал
    std::cout << "Ждём 3 секунды для проверки таймаута...\n";
    std::this_thread::sleep_for(std::chrono::seconds(3));

    // Шаг 7: вызываем очистку просроченных сессий
    std::vector<std::string> expired = manager.cleanupExpiredSessions();
    for (const std::string& imsi : expired) {
        std::cout << "Сессия просрочена и удалена: " << imsi << "\n";
    }


    UdpServer udpServer(udpServerSettings, manager);
    std::thread serverThread([&udpServer]() {
        udpServer.start(); // blocking call
    });
    std::this_thread::sleep_for(std::chrono::seconds(1));

    UdpClient udpClient(clientSettings);

    std::string imsi = "001010123456780";

    if (!udpClient.send_imsi(imsi)) {
        std::cerr << "Client failed to send IMSI" << std::endl;
    }

    // Подождать немного для обработки
    std::this_thread::sleep_for(std::chrono::seconds(2));

    // === Gracefully shut down the server ===
    udpServer.stop();

    if (serverThread.joinable())
        serverThread.join();
    
    return 0;
}
