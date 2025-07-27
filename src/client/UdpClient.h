#ifndef UDPCLIENT_H
#define UDPCLIENT_H

#include "./SettingsStructures/ClientSettings.h"
#include "./ConfigLoader/ClientConfigLoader.h"
#include "StructSplitter.h"
#include "./Consts/PathConsts.h"
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <string>

class UdpClient {
public:
    UdpClient();
    bool send_imsi(const std::string& imsi); 
    static std::string encode_bcd(const std::string& imsi);
    ClientSettings getClientSettings();

private:
    void initLogging();
    void loadConfiguration();

    ClientSettings clientSettings;
    std::shared_ptr<spdlog::logger> clientLogger;
};

#endif