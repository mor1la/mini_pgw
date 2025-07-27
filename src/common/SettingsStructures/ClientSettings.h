#ifndef ClIENTSETTINGS_H
#define ClIENTSETTINGS_H
#include <string>

struct ClientSettings {
    std::string serverIp;
    int serverPort;
    std::string logFile;
    std::string logLevel;
};



#endif