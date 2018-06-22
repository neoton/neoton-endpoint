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

    log ("Setting up audio player...");
    player = new NTBassPlayer(this);
    int playerInitResult = player->init(deviceNum, samplingFrequency);
    if (playerInitResult != 0)
    {
        log (QString("Could not initialize player, code %1").arg(playerInitResult), NTLog::LL_ERROR);
        exit(1);
    }

    log ("Instantiating socket connection...");
    client = new NTClient(serverAddress, serverPort, password, endpointId, serverSecure, this);
    connect (client, SIGNAL(playRequest()), this, SLOT(onPlayRequest()));
    connect (client, SIGNAL(streamParametersSet(QString,uint)), this, SLOT(onStreamParametersSet(QString,uint)));
    connect (client, SIGNAL(volumeSet(uint)), this, SLOT(onVolumeSet(uint)));
    connect (client, SIGNAL(playRequest()), this, SLOT(onPlayRequest()));
    connect (client, SIGNAL(stopRequest()), this, SLOT(onStopRequest()));

    client->start();
}

void NTEndpoint::loadConfig(QString configFile)
{
    QSettings settings(configFile, QSettings::IniFormat);

    settings.beginGroup("endpoint");
    logLevel = (NTLog::LogLevel)settings.value("log_level", NTLog::LL_DEBUG).toInt();
    logFile = settings.value("log_file", "stdout").toString();
    deviceNum = settings.value("device_number", -1).toInt();
    samplingFrequency = settings.value("sampling_frequency", 48000).toInt();
    settings.endGroup();


    settings.beginGroup("server");
    serverAddress = settings.value("address", "localhost").toString();
    serverPort = settings.value("port", 1337).toUInt();
    endpointId = settings.value("endpoint_id", -1).toInt();
    password = settings.value("password", "aw_sh_ee_tt").toString();
    serverSecure = settings.value("secure", false).toBool();
    reconnectOnError = settings.value("reconnect_on_error", true).toBool();
    settings.endGroup();
}

void NTEndpoint::log(QString message, NTLog::LogLevel level)
{
    NTLog::instance->log(message, level, "endpt");
}

void NTEndpoint::onVolumeSet(uint vol)
{
    player->setVolume(vol);
}

void NTEndpoint::onStreamParametersSet(QString mount, uint port)
{
    /*
        %1 => username (endpoint_<endpoint_id>)
        %2 => password
        %3 => server address
        %4 => server audio port
        %5 => mount point
    */
    player->setUrl(QUrl(QString("http://endpoint_%1:%2@%3:%4/%5")
                        .arg(endpointId).arg(password).arg(serverAddress)
                        .arg(port).arg(mount)));
}

void NTEndpoint::onPlayRequest()
{
    log ("Play start requested!");
    int playResult = player->playUrl();

    if (playResult != 0)
        log (QString("Play was not successful, code %1").arg(playResult), NTLog::LL_WARNING);
}

void NTEndpoint::onStopRequest()
{
    log ("Play stop requested!");
    player->stop();
}

void NTEndpoint::onAuthResult(bool res)
{
    if (!res)
    {
        log ("Could not authorize on the server, exiting.", NTLog::LL_ERROR);
        exit(2);
    }
}

void NTEndpoint::onServerDisconnect()
{
    // todo: perform reconnect logic
}
