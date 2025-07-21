#ifndef SESSION_MANAGER_H
#define SESSION_MANAGER_H

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <chrono>
#include <vector>

class SessionManager {
public:
    SessionManager(int timeoutSeconds, std::unordered_set<std::string> blacklist);

    bool initSession(const std::string& imsi);
    void removeSession(const std::string& imsi);
    std::vector<std::string> cleanupExpiredSessions();

private:
    bool isBlacklisted(const std::string& imsi) const;
    bool createSession(const std::string& imsi);
    bool updateSession(const std::string &imsi);
    bool hasSession(const std::string &imsi) const;

    int timeoutSeconds;
    std::unordered_set<std::string> blacklist;
    std::unordered_map<std::string, std::chrono::steady_clock::time_point> sessions;
};

#endif 
