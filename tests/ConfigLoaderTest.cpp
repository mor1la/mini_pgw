#include <gtest/gtest.h>
#include <fstream>
#include <filesystem>
#include "../common/ConfigLoader/ClientConfigLoader.h"
#include "../common/ConfigLoader/ServerConfigLoader.h"

namespace fs = std::filesystem;

class ConfigLoaderTest : public ::testing::Test {
protected:
    std::string serverConfigPath;
    std::string clientConfigPath;

    void SetUp() override {
        serverConfigPath = std::filesystem::temp_directory_path() / "test_server_config.json";
        clientConfigPath = std::filesystem::temp_directory_path() / "test_client_config.json";

        std::ofstream serverOut(serverConfigPath);
        serverOut << R"({
            "udp_ip": "0.0.0.0",
            "udp_port": 9000,
            "session_timeout_sec": 30,
            "cdr_file": "cdr.log",
            "http_port": 8080,
            "graceful_shutdown_rate": 10,
            "log_file": "pgw.log",
            "log_level": "INFO",
            "blacklist": [
              "001010123456789",
              "001010000000001"
            ]
        })";
        serverOut.close();

        std::ofstream clientOut(clientConfigPath);
        clientOut << R"({
            "server_ip": "127.0.0.1",
            "server_port": 9000,
            "log_file": "client.log",
            "log_level": "INFO"
        })";
        clientOut.close();
    }

    void TearDown() override {
        std::filesystem::remove(serverConfigPath);
        std::filesystem::remove(clientConfigPath);
    }
};


TEST_F(ConfigLoaderTest, LoadValidServerConfig) {
    ServerConfigLoader loader;
    RawServerSettings settings = loader.loadFromFile(serverConfigPath);

    EXPECT_EQ(settings.udpIp, "0.0.0.0");
    EXPECT_EQ(settings.udpPort, 9000);
    EXPECT_EQ(settings.sessionTimeoutSec, 30);
    EXPECT_EQ(settings.cdrFile, "cdr.log");
    EXPECT_EQ(settings.httpPort, 8080);
    EXPECT_EQ(settings.gracefulShutdownRate, 10);
    EXPECT_EQ(settings.logFile, "pgw.log");
    EXPECT_EQ(settings.logLevel, "INFO");
    EXPECT_EQ(settings.blacklist.size(), 2);
    EXPECT_TRUE(settings.blacklist.count("001010123456789"));
    EXPECT_TRUE(settings.blacklist.count("001010000000001"));
}

TEST_F(ConfigLoaderTest, LoadValidClientConfig) {
    ClientConfigLoader loader;
    ClientSettings settings = loader.loadFromFile(clientConfigPath);

    EXPECT_EQ(settings.serverIp, "127.0.0.1");
    EXPECT_EQ(settings.serverPort, 9000);
    EXPECT_EQ(settings.logFile, "client.log");
    EXPECT_EQ(settings.logLevel, "INFO");
}

TEST_F(ConfigLoaderTest, MissingServerConfigFileThrows) {
    ServerConfigLoader loader;
    std::string badPath = fs::temp_directory_path() / "nonexistent.json";
    EXPECT_THROW(loader.loadFromFile(badPath), std::runtime_error);
}

TEST_F(ConfigLoaderTest, MissingClientConfigFileThrows) {
    ClientConfigLoader loader;
    std::string badPath = fs::temp_directory_path() / "missing_client.json";
    EXPECT_THROW(loader.loadFromFile(badPath), std::runtime_error);
}

TEST_F(ConfigLoaderTest, InvalidServerJsonThrows) {
    std::string path = fs::temp_directory_path() / "bad_server.json";
    std::ofstream out(path);
    out << R"({"udp_ip": "0.0.0.0"})"; // Недостаточно полей
    out.close();

    ServerConfigLoader loader;
    EXPECT_THROW(loader.loadFromFile(path), std::runtime_error);

    fs::remove(path);
}

TEST_F(ConfigLoaderTest, InvalidClientJsonThrows) {
    std::string path = fs::temp_directory_path() / "bad_client.json";
    std::ofstream out(path);
    out << R"({"server_ip": "127.0.0.1"})"; // Недостаточно полей
    out.close();

    ClientConfigLoader loader;
    EXPECT_THROW(loader.loadFromFile(path), std::runtime_error);

    fs::remove(path);
}
