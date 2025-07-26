#ifndef ICONFIGLOADER_H
#define ICONFIGLOADER_H
#include <string>
#include <fstream>
#include <nlohmann/json.hpp>
#include <stdexcept>


template<typename SettingsT>
class IConfigLoader {
public:
    virtual  SettingsT loadFromFile(const std::string& path) = 0;
    virtual ~IConfigLoader() = default;
};

#endif