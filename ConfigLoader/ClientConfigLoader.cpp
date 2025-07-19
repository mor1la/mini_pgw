#include "ClientConfigLoader.h"
#include <fstream>
#include <nlohmann/json.hpp>


bool ClientConfigLoader::loadFromFile(const std::string& path, ClientSettings& settings) {
    std::ifstream file(path);
    if (!file.is_open()) return false;

    nlohmann::json j;
    file >> j;

    settings.server_ip = j.value("server_ip", "127.0.0.1");
    settings.server_port = j.value("server_port", 9000);
    settings.log_file = j.value("log_file", "client.log");
    settings.log_level = j.value("log_level", "INFO");

    return true;
}
