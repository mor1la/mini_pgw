#include "./RawServerSettings.h"
#include "./ClientSettings.h"
#include "./ConfigLoader/ServerConfigLoader.h"
#include "ClientConfigLoader.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <unordered_set>
#include "SessionManager.h"

int main() {
    ServerConfigLoader loader;
    try {
        auto settings = loader.loadFromFile("config/server_config.json");  

        std::cout << settings.udp_ip;
        std::cout << settings.cdr_file;
        std::cout << settings.http_port;
    } catch (const std::exception& e) {
        std::cerr << "Failed to load server config: " << e.what() << std::endl;
        return 1;
    }

    try {
        ClientConfigLoader loader;
        ClientSettings settings = loader.loadFromFile("config/client_config.json");
        // Дальше используешь settings
    } catch (const std::exception& e) {
        std::cerr << "Failed to load client config: " << e.what() << std::endl;
        return 1;
    }

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
}
