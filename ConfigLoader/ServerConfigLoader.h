#ifndef SERVERCONFIGLOADER_H
#define SERVERCONFIGLOADER_H
#include "IConfigLoader.h"
#include "./SettingsStructures/RawServerSettings.h"


class ServerConfigLoader : public IConfigLoader<RawServerSettings> {
public:
    RawServerSettings loadFromFile(const std::string& path);
};

#endif  