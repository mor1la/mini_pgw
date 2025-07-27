#include "ClientConfigLoader.h"

ClientSettings ClientConfigLoader::loadFromFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open config file: " + path);
    }

    nlohmann::json j;
    file >> j;

    ClientSettings settings;

    try {
        settings.serverIp = j.at("server_ip").get<std::string>();
        settings.serverPort = j.at("server_port").get<int>();
        settings.logFile = j.at("log_file").get<std::string>();
        settings.logLevel = j.at("log_level").get<std::string>();
    } catch (const nlohmann::json::exception& e) {
        throw std::runtime_error(std::string("Invalid or missing config field: ") + e.what());
    }

    return settings;
}

