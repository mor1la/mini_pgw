#ifndef RAWSERVERSETTINGS_H
#define RAWSERVERSETTINGS_H
#include <string>
#include <vector>

struct RawServerSettings {
    std::string udp_ip;
    int udp_port;
    int session_timeout_sec;
    std::string cdr_file;
    int http_port;
    int graceful_shutdown_rate;
    std::string log_file;
    std::string log_level;
    std::vector<std::string> blacklist;
};

#endif