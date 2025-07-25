#pragma once
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <chrono>
#include <string>
#include <mutex>
#include <spdlog/spdlog.h>

class SessionManager {
public:
    SessionManager(int timeoutSeconds, std::unordered_set<std::string> blacklist);

    bool initSession(const std::string &imsi);
    bool isBlacklisted(const std::string &imsi) const;

    bool hasSession(const std::string &imsi) const;

    void removeSession(const std::string &imsi);
    std::vector<std::string> cleanupExpiredSessions();

    std::unordered_map<std::string, std::chrono::steady_clock::time_point> getAllSessions() const;
private:
    int timeoutSeconds;
    std::unordered_set<std::string> blacklist;

    mutable std::mutex sessionMutex;
    std::unordered_map<std::string, std::chrono::steady_clock::time_point> sessions;

    std::shared_ptr<spdlog::logger> serverLogger;
};
