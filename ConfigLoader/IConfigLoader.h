#ifndef ICONFIGLOADER_H
#define ICONFIGLOADER_H
#include <string>


template<typename SettingsT>
class IConfigLoader {
public:
    virtual bool loadFromFile(const std::string& path, SettingsT& settings) = 0;
    virtual ~IConfigLoader() = default;
};

#endif