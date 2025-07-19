#include "ServerConfigLoader.h"
#include <fstream>
#include <nlohmann/json.hpp>


bool ServerConfigLoader::loadFromFile(const std::string& path, ServerSettings& settings) {
    std::ifstream file(path);
    if (!file.is_open()) return false;

    nlohmann::json j;
    file >> j;

    settings.udp_ip = j.value("udp_ip", "0.0.0.0");
    settings.udp_port = j.value("udp_port", 9000);
    settings.session_timeout_sec = j.value("session_timeout_sec", 30);
    settings.cdr_file = j.value("cdr_file", "cdr.log");
    settings.http_port = j.value("http_port", 8080);
    settings.graceful_shutdown_rate = j.value("graceful_shutdown_rate", 10);
    settings.log_file = j.value("log_file", "pgw.log");
    settings.log_level = j.value("log_level", "INFO");
    settings.blacklist = j.value("blacklist", std::vector<std::string>{});

    return true;
}
