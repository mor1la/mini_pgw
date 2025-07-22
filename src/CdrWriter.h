#ifndef CDRWRITER_H
#define CDRWRITER_H

#include <fstream>
#include <mutex>
#include <string>
#include <memory>
#include <spdlog/spdlog.h>

class CdrWriter {
public:
    enum Action {
        Create,
        Reject,
        Delete,
        Offload,
        Update
    };

    CdrWriter(const std::string& filename);
    ~CdrWriter();

    
    void write(const std::string& imsi, Action action);

private:
    std::ofstream cdrFile;
    std::mutex writeMutex;
    std::shared_ptr<spdlog::logger> logger;

    std::string getTimestamp();  
    std::string actionToString(Action action);
};

#endif // CDRWRITER_H
