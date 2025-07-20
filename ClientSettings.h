#ifndef ClIENTSETTINGS_H
#define ClIENTSETTINGS_H
#include <string>

struct ClientSettings {
    std::string server_ip;
    int server_port;
    std::string log_file;
    std::string log_level;
};



#endif