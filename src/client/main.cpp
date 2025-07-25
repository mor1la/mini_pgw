#include "UdpClient.h"
#include <iostream>

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: pgw_client <IMSI>\n";
        return 1;
    }

    std::string imsi = argv[1];

    if (imsi.empty() || imsi.find_first_not_of("0123456789") != std::string::npos) {
        std::cerr << "Error: IMSI must contain only digits.\n";
        return 1;
    }

    try {
        UdpClient client;

        if (client.send_imsi(imsi)) {
            std::cout << "IMSI sent successfully.\n";
        } else {
            std::cerr << "Failed to send IMSI.\n";
            return 1;
        }
    } catch (const std::exception& ex) {
        std::cerr << "Client error: " << ex.what() << "\n";
        return 1;
    }

    return 0;
}
