#include "CdrWriter.h"
#include <chrono>
#include <iomanip>
#include <sstream>

CdrWriter::CdrWriter(const std::string& filename) {
    cdrFile.open(filename, std::ios::app);
    if (!cdrFile.is_open()) {
        throw std::runtime_error("Failed to open CDR file: " + filename);
    }
    logger = spdlog::get("serverLogger");
    if (!logger) {
        throw std::logic_error("Global serverLogger is not initialized");
    }
}

CdrWriter::~CdrWriter() {
    if (cdrFile.is_open()) {
        cdrFile.close();
    }
}

void CdrWriter::write(const std::string& imsi, Action action) {
    std::lock_guard<std::mutex> lock(writeMutex);

    std::string timestamp = getTimestamp();
    cdrFile << timestamp << "," << imsi << "," << actionToString(action) << std::endl;

    //logger->info("CDR entry written: {}, {}, {}", timestamp, imsi, action);
}

std::string CdrWriter::getTimestamp() {
    using namespace std::chrono;

    auto now = system_clock::now();
    std::time_t now_c = system_clock::to_time_t(now);
    std::tm tm = *std::localtime(&now_c);
    
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

std::string CdrWriter::actionToString(Action action) {
    static const std::unordered_map<Action, std::string> actionMap = {
        {Action::Create, "create"},
        {Action::Reject, "reject"},
        {Action::Delete, "delete"},
        {Action::Offload, "offload"},
        {Action::Update, "update"}
    };
    
    auto it = actionMap.find(action);
    return it != actionMap.end() ? it->second : "unknown";
}