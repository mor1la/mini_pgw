#pragma once

#include <string>
#include "./SettingsStructures/ClientSettings.h"
#include <spdlog/spdlog.h>

class Logger;

class UdpClient {
public:
    UdpClient(const ClientSettings& settings);
    bool send_imsi(const std::string& imsi); 

private:
    std::string encode_bcd(const std::string& imsi);

    const ClientSettings& settings_;
    std::shared_ptr<spdlog::logger> logger;
};
