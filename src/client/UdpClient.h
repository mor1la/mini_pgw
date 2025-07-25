#ifndef UDPCLIENT_H
#define UDPCLIENT_H

#include <string>
#include "./SettingsStructures/ClientSettings.h"
#include "./ConfigLoader/ClientConfigLoader.h"
#include "StructSplitter.h"
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>

class UdpClient {
public:
    UdpClient();
    bool send_imsi(const std::string& imsi); 

private:
    std::string encode_bcd(const std::string& imsi);

    ClientSettings clientSettings;
    std::shared_ptr<spdlog::logger> clientLogger;
    void initLogging();
    void loadConfiguration();
};

#endif