#include "StructSplitter.h"

UdpServerSettings StructSplitter::makeUdpSettings(const RawServerSettings &raw)
{
    return { raw.udp_ip, raw.udp_port };
}

HttpApiServerSettings StructSplitter::makeHttpSettings(const RawServerSettings &raw)
{
    return { raw.http_port, raw.graceful_shutdown_rate };
}

LoggerSettings StructSplitter::makeLoggerSettings(const RawServerSettings &raw)
{
    return { raw.log_file, raw.log_level };
}

SessionManagerSettings StructSplitter::makeSessionManagerSettings(const RawServerSettings &raw)
{
    return { raw.session_timeout_sec, raw.blacklist, raw.cdr_file};
}
