#include "ntendpoint.h"

NTEndpoint::NTEndpoint(QString config, QObject *parent) : QObject(parent), config(config)
{
    if (!QFile::exists(config))
    {
        if (config.isEmpty())
            config = "<none>";

        printf ("Configuration file %s does not exist. Neoton Endpoint can't operate without a config file. Exiting.\n",
                config.toUtf8().data());
        exit(-1);
    }

    loadConfig(config);

    NTLog::instance = new NTLog(logFile, (NTLog::LogLevel)logLevel);
    log ("Welcome to Neoton Endpoint Player!", NTLog::LL_INFO);
}

void NTEndpoint::loadConfig(QString configFile)
{
    QSettings settings(configFile, QSettings::IniFormat);

    settings.beginGroup("endpoint");
    logLevel = (NTLog::LogLevel)settings.value("log_level", NTLog::LL_DEBUG).toInt();
    logFile = settings.value("log_file", "stdout").toString();
    settings.endGroup();
}

void NTEndpoint::log(QString message, NTLog::LogLevel level)
{
    NTLog::instance->log(message, level, "endpt");
}
