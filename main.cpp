#include "./ServerSettings.h"
#include "./ClientSettings.h"
#include "./ConfigLoader/ServerConfigLoader.h"
#include <iostream>

int main() {
    ServerSettings settings;
    ServerConfigLoader loader;

    if (!loader.loadFromFile("config/server_config.json", settings)) {
        std::cerr << "Failed to load config\n";
        return 1;
    }
    std::cout << settings.udp_ip;
    
}
