#ifndef SERVERCONFIGLOADER_H
#define SERVERCONFIGLOADER_H
#include "IConfigLoader.h"
#include "../ServerSettings.h"


class ServerConfigLoader : public IConfigLoader<ServerSettings> {
public:
    bool loadFromFile(const std::string& path, ServerSettings& settings);
};

#endif  