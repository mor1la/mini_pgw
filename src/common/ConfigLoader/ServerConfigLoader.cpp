#include "ServerConfigLoader.h"

RawServerSettings ServerConfigLoader::loadFromFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open config file: " + path);
    }

    nlohmann::json j;
    file >> j;

    RawServerSettings settings;

    try {
        settings.udpIp = j.at("udp_ip").get<std::string>();
        settings.udpPort = j.at("udp_port").get<int>();
        settings.sessionTimeoutSec = j.at("session_timeout_sec").get<int>();
        settings.cdrFile = j.at("cdr_file").get<std::string>();
        settings.httpPort = j.at("http_port").get<int>();
        settings.gracefulShutdownRate = j.at("graceful_shutdown_rate").get<int>();
        settings.logFile = j.at("log_file").get<std::string>();
        settings.logLevel = j.at("log_level").get<std::string>();
        settings.blacklist = j.at("blacklist").get<std::unordered_set<std::string>>();
    } catch (const nlohmann::json::exception& e) {
        throw std::runtime_error(std::string("Invalid or missing config field: ") + e.what());
    }

    return settings;
}
