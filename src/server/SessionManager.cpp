#include "SessionManager.h"
#include <spdlog/sinks/null_sink.h>

SessionManager::SessionManager(const SessionManagerSettings settings, CdrWriter &cdrWriter, std::shared_ptr<spdlog::logger> logger)
    : settings(settings), cdrWriter(cdrWriter), serverLogger(logger ? logger : spdlog::get("serverLogger")) {

    if (!serverLogger) {
        auto null_sink = std::make_shared<spdlog::sinks::null_sink_mt>();
        serverLogger = std::make_shared<spdlog::logger>("null_logger", null_sink);
    }
}


bool SessionManager::isBlacklisted(const std::string &imsi) const {
    if (settings.blacklist.find(imsi) != settings.blacklist.end()) {
        cdrWriter.write(imsi, CdrWriter::Action::Reject);
        return true;
    }
    return false;
}

bool SessionManager::initSession(const std::string &imsi) {
    if (isBlacklisted(imsi)) {
        return false;
    }

    std::lock_guard<std::mutex> lock(sessionMutex);

    auto it = sessions.find(imsi);
    if (it != sessions.end()) {
        it->second = std::chrono::steady_clock::now();
        cdrWriter.write(imsi, CdrWriter::Action::Update);
        serverLogger->debug("Session updated for IMSI {}", imsi);
        return true;
    } else {
        sessions[imsi] = std::chrono::steady_clock::now();
        cdrWriter.write(imsi, CdrWriter::Action::Create);
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
    cdrWriter.write(imsi, CdrWriter::Action::Delete);
    serverLogger->debug("Session removed for IMSI {}", imsi);
}

std::vector<std::string> SessionManager::cleanupExpiredSessions() {
    std::lock_guard<std::mutex> lock(sessionMutex);

    std::vector<std::string> removedImsis;
    auto now = std::chrono::steady_clock::now();

    for (auto it = sessions.begin(); it != sessions.end(); ) {
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - it->second).count();
        if (elapsed >= settings.timeoutSeconds) {
            std::string imsi = it->first;
            removedImsis.push_back(imsi);
            it = sessions.erase(it);
            cdrWriter.write(imsi, CdrWriter::Action::Delete);
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

void SessionManager::offloadSession(const std::string &imsi) {
    std::lock_guard<std::mutex> lock(sessionMutex);
    if (sessions.erase(imsi) > 0) {
        cdrWriter.write(imsi, CdrWriter::Action::Offload);
        serverLogger->info("Session for IMSI {} was gracefully offloaded", imsi);
    }
}
