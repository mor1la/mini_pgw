#include "./SettingsStructures/RawServerSettings.h"
#include "./SettingsStructures/UdpServerSettings.h"
#include "./SettingsStructures/HttpApiServerSettings.h"
#include "./SettingsStructures/SessionManagerSettings.h"
#include "./SettingsStructures/LoggerSettings.h"

class StructSplitter {
public:
    static UdpServerSettings makeUdpSettings(const RawServerSettings& raw);

    static HttpApiServerSettings makeHttpSettings(const RawServerSettings& raw);

    static LoggerSettings makeLoggerSettings(const RawServerSettings &raw);

    static SessionManagerSettings makeSessionManagerSettings(const RawServerSettings &raw);
};