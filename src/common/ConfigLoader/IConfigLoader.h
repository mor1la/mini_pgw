#ifndef ICONFIGLOADER_H
#define ICONFIGLOADER_H
#include <string>


template<typename SettingsT>
class IConfigLoader {
public:
    virtual  SettingsT loadFromFile(const std::string& path) = 0;
    virtual ~IConfigLoader() = default;
};

#endif