#include "StructSplitter.h"

UdpServerSettings StructSplitter::makeUdpSettings(const RawServerSettings &raw)
{
    return { raw.udpIp, raw.udpPort };
}

HttpApiServerSettings StructSplitter::makeHttpSettings(const RawServerSettings &raw)
{
    return { raw.httpPort, raw.gracefulShutdownRate };
}

LoggerSettings StructSplitter::makeLoggerSettings(const RawServerSettings &raw)
{
    return { raw.logFile, raw.logLevel };
}

SessionManagerSettings StructSplitter::makeSessionManagerSettings(const RawServerSettings &raw)
{
    return { raw.sessionTimeoutSec, raw.blacklist, raw.cdrFile };
}
