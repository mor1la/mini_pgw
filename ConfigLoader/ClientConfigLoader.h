#ifndef CLIENTCONFIGLOADER_H
#define CLIENTCONFIGLOADER_H
#include "IConfigLoader.h"
#include "./SettingsStructures/ClientSettings.h"


class ClientConfigLoader : public IConfigLoader<ClientSettings> {
public:
    ClientSettings loadFromFile(const std::string& path);
};

#endif