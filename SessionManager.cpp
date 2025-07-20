#include "SessionManager.h"

SessionManager::SessionManager(int timeoutSeconds, std::unordered_set<std::string> blacklist)
    : timeoutSeconds(timeoutSeconds), blacklist(blacklist) {}

bool SessionManager::isBlacklisted(const std::string& imsi) const {
    return blacklist.find(imsi) != blacklist.end();
}

bool SessionManager::createSession(const std::string& imsi) {
    if (isBlacklisted(imsi)) {
        return false; 
    }

    if (sessions.find(imsi) != sessions.end()) {
        return false; 
    }

    sessions[imsi] = std::chrono::steady_clock::now();
    return true;
}

bool SessionManager::hasSession(const std::string& imsi) const {
    return sessions.find(imsi) != sessions.end();
}

void SessionManager::removeSession(const std::string& imsi) {
    sessions.erase(imsi);
}

std::vector<std::string> SessionManager::cleanupExpiredSessions() {
    std::vector<std::string> removedImsis;
    auto now = std::chrono::steady_clock::now();

    for (auto it = sessions.begin(); it != sessions.end(); ) {
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - it->second).count();
        if (elapsed > timeoutSeconds) {
            removedImsis.push_back(it->first);
            it = sessions.erase(it);
        } else {
            ++it;
        }
    }

    return removedImsis;
}
