#ifndef UDPCLIENT_H
#define UDPCLIENT_H

#include <string>
#include "./SettingsStructures/ClientSettings.h"
#include <spdlog/spdlog.h>

class UdpClient {
public:
    UdpClient(const ClientSettings& settings);
    bool send_imsi(const std::string& imsi); 

private:
    std::string encode_bcd(const std::string& imsi);

    const ClientSettings& clientSettings;
    std::shared_ptr<spdlog::logger> clientLogger;
    void initLogging();
};

#endif