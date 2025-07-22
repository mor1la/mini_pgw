#ifndef CDRWRITER_H
#define CDRWRITER_H

#include <fstream>
#include <mutex>
#include <string>
#include <memory>
#include <spdlog/spdlog.h>

class CdrWriter {
public:
    enum actions {
        
    }
    CdrWriter(const std::string& filename);
    ~CdrWriter();

    
    void write(const std::string& imsi, const std::string& action);

private:
    std::ofstream cdrFile;
    std::mutex writeMutex;
    std::shared_ptr<spdlog::logger> logger;

    std::string getTimestamp();  
};

#endif // CDRWRITER_H
