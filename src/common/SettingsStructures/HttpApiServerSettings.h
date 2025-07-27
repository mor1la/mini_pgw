#ifndef HTTPAPISERVERSETTINGS_H
#define HTTPAPISERVERSETTINGS_H

struct HttpApiServerSettings {
    int port;
    unsigned int gracefulShutdownRate;
};

#endif