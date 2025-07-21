#ifndef SESSIONMANAGERSETTINGS_H
#define SESSIONMANAGERSETTINGS_H
#include <string>
#include <unordered_set>


struct SessionManagerSettings {
    int timeoutSeconds;
    std::unordered_set<std::string> blacklist;
    std::string cdrFilePath;
};

#endif