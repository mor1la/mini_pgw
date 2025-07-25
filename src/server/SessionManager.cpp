#include "SessionManager.h"

SessionManager::SessionManager(int timeoutSeconds, std::unordered_set<std::string> blacklist)
    : timeoutSeconds(timeoutSeconds), blacklist(std::move(blacklist)) {
    serverLogger = spdlog::get("serverLogger");
    if (!serverLogger) {
        throw std::logic_error("Global serverLogger is not initialized");
    }
}

bool SessionManager::isBlacklisted(const std::string &imsi) const {
    return blacklist.find(imsi) != blacklist.end();
}

bool SessionManager::initSession(const std::string &imsi) {
    if (isBlacklisted(imsi)) {
        return false;
    }

    std::lock_guard<std::mutex> lock(sessionMutex);

    auto it = sessions.find(imsi);
    if (it != sessions.end()) {
        it->second = std::chrono::steady_clock::now();
        serverLogger->debug("Session updated for IMSI {}", imsi);
        return true;
    } else {
        sessions[imsi] = std::chrono::steady_clock::now();
        serverLogger->debug("Session created for IMSI {}", imsi);
        return true;
    }
}


bool SessionManager::hasSession(const std::string &imsi) const {
    std::lock_guard<std::mutex> lock(sessionMutex);
    return sessions.find(imsi) != sessions.end();
}

void SessionManager::removeSession(const std::string &imsi) {
    std::lock_guard<std::mutex> lock(sessionMutex);
    sessions.erase(imsi);
    serverLogger->debug("Session removed for IMSI {}", imsi);
}

std::vector<std::string> SessionManager::cleanupExpiredSessions() {
    std::lock_guard<std::mutex> lock(sessionMutex);

    std::vector<std::string> removedImsis;
    auto now = std::chrono::steady_clock::now();

    for (auto it = sessions.begin(); it != sessions.end(); ) {
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - it->second).count();
        if (elapsed >= timeoutSeconds) {
            std::string imsi = it->first;
            removedImsis.push_back(imsi);
            it = sessions.erase(it);

            serverLogger->info("Session for IMSI {} expired and was removed after {} seconds", imsi, elapsed);
        } else {
            ++it;
        }
    }   
    if (removedImsis.size() > 0) {
        serverLogger->info("Cleanup finished. {} expired session(s) removed.", removedImsis.size());
    }

    return removedImsis;
}

std::unordered_map<std::string, std::chrono::steady_clock::time_point> SessionManager::getAllSessions() const {
    std::lock_guard<std::mutex> lock(sessionMutex);
    return sessions;
}
