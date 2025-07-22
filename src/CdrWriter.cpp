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

void CdrWriter::write(const std::string& imsi, const std::string& action) {
    std::lock_guard<std::mutex> lock(writeMutex);

    std::string timestamp = getTimestamp();
    cdrFile << timestamp << "," << imsi << "," << action << std::endl;

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
