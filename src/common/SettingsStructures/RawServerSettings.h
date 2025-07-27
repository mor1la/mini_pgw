#ifndef RAWSERVERSETTINGS_H
#define RAWSERVERSETTINGS_H
#include <string>
#include <unordered_set>

struct RawServerSettings {
    std::string udpIp;
    int udpPort;
    int sessionTimeoutSec;
    std::string cdrFile;
    int httpPort;
    int gracefulShutdownRate;
    std::string logFile;
    std::string logLevel;
    std::unordered_set<std::string> blacklist;
};

#endif