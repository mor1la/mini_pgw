#ifndef HTTPAPISERVERSETTINGS_H
#define HTTPAPISERVERSETTINGS_H

struct HttpApiServerSettings {
    int port;
    int gracefulShutdownRate;
};

#endif