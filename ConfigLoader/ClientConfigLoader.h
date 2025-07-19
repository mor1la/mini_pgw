#ifndef CLIENTCONFIGLOADER_H
#define CLIENTCONFIGLOADER_H
#include "IConfigLoader.h"
#include "ServerSettings.h"
#include "ClientSettings.h"


class ClientConfigLoader : public IConfigLoader<ClientSettings> {
public:
    bool loadFromFile(const std::string& path, ClientSettings& settings);
};

#endif