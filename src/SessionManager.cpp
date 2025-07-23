#include "SessionManager.h"

SessionManager::SessionManager(int timeoutSeconds, std::unordered_set<std::string> blacklist)
    : timeoutSeconds(timeoutSeconds), blacklist(blacklist) {
        logger = spdlog::get("serverLogger");
        if (!logger) {
            throw std::logic_error("Global clientLogger is not initialized");
        }
    }

bool SessionManager::isBlacklisted(const std::string &imsi) const {
    return blacklist.find(imsi) != blacklist.end();
}

bool SessionManager::initSession(const std::string &imsi) {
    if (isBlacklisted(imsi)) {
        return false; 
    }

    if (updateSession(imsi) || createSession(imsi)) {
        return true;
    }

    return false;
}

bool SessionManager::updateSession(const std::string &imsi) {
    if (hasSession(imsi)) {
        sessions[imsi] = std::chrono::steady_clock::now();
        // лог: Сессия обновлена
        return true;
    }
    return false;
}

bool SessionManager::createSession(const std::string &imsi) {
    sessions[imsi] = std::chrono::steady_clock::now();
    // лог: Сессия создана 
    return true;
}

bool SessionManager::hasSession(const std::string &imsi) const {
    return sessions.find(imsi) != sessions.end();
}

void SessionManager::removeSession(const std::string &imsi) {
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
