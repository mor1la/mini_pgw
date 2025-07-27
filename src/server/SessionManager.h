#ifndef SESSIONMANAGER_H
#define SESSIONMANAGER_H

#include "CdrWriter.h"
#include "./SettingsStructures/SessionManagerSettings.h"

#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <chrono>
#include <spdlog/spdlog.h>

class SessionManager {
public:
    SessionManager(const SessionManagerSettings settings, CdrWriter &cdrWriter, std::shared_ptr<spdlog::logger> logger = nullptr);
    bool initSession(const std::string &imsi);
    bool isBlacklisted(const std::string &imsi) const;

    bool hasSession(const std::string &imsi) const;
    std::unordered_map<std::string, std::chrono::steady_clock::time_point> getAllSessions() const;

    void removeSession(const std::string &imsi);
    std::vector<std::string> cleanupExpiredSessions();

    void offloadSession(const std::string &imsi);

private:
    const SessionManagerSettings settings;
    mutable std::mutex sessionMutex;
    std::unordered_map<std::string, std::chrono::steady_clock::time_point> sessions;
    CdrWriter &cdrWriter;
    std::shared_ptr<spdlog::logger> serverLogger;
};

#endif