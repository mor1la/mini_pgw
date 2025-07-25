#include "PgwServer.h"
#include <csignal>
#include <atomic>
#include <iostream>


int main() {
    try {
        PgwServer server;
        server.start(); 
    } catch (const std::exception& ex) {
        std::cerr << "Server initialization failed: " << ex.what() << std::endl;
        return 1;
    }
    return 0;
}

