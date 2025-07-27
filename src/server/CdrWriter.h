#ifndef CDRWRITER_H
#define CDRWRITER_H

#include <fstream>
#include <mutex>
#include <string>
#include <memory>
#include <spdlog/spdlog.h>
#include <chrono>
#include <iomanip>
#include <sstream>

class CdrWriter {
public:
    enum Action {
        Create,
        Reject,
        Delete,
        Offload,
        Update
    };

    CdrWriter(const std::string &filename, std::shared_ptr<spdlog::logger> logger = nullptr);
    ~CdrWriter();

    virtual void write(const std::string &imsi, Action action);

private:
    std::string getTimestamp();  
    std::string actionToString(Action action);

    std::ofstream cdrFile;
    std::mutex writeMutex;
    std::shared_ptr<spdlog::logger> serverLogger;
};

#endif 
