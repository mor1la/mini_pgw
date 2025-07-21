#include "ServerConfigLoader.h"
#include <fstream>
#include <nlohmann/json.hpp>
#include <stdexcept>

RawServerSettings ServerConfigLoader::loadFromFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open config file: " + path);
    }

    nlohmann::json j;
    file >> j;

    RawServerSettings settings;

    try {
        settings.udp_ip = j.at("udp_ip").get<std::string>();
        settings.udp_port = j.at("udp_port").get<int>();
        settings.session_timeout_sec = j.at("session_timeout_sec").get<int>();
        settings.cdr_file = j.at("cdr_file").get<std::string>();
        settings.http_port = j.at("http_port").get<int>();
        settings.graceful_shutdown_rate = j.at("graceful_shutdown_rate").get<int>();
        settings.log_file = j.at("log_file").get<std::string>();
        settings.log_level = j.at("log_level").get<std::string>();
        settings.blacklist = j.at("blacklist").get<std::vector<std::string>>();
    } catch (const nlohmann::json::exception& e) {
        throw std::runtime_error(std::string("Invalid or missing config field: ") + e.what());
    }

    return settings;
}
