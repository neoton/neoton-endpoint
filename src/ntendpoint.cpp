#include "ntendpoint.h"

/*

    This is Neoton, a public broadcasting system
       (c) Asterleen ~ https://asterleen.com
    Licensed under BSD 3-Clause License, see LICENSE

*/

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
    log ("Welcome to Neoton Endpoint Client!", NTLog::LL_INFO);

    log ("Setting up audio player...", NTLog::LL_INFO);
    player = new NTBassPlayer(this);
    int playerInitResult = player->init(deviceNum, samplingFrequency);
    if (playerInitResult != 0)
    {
        log (QString("Could not initialize player, code %1").arg(playerInitResult), NTLog::LL_ERROR);
        exit(1);
    }
        else
        log ("Player initialized successfully.", NTLog::LL_INFO);

    log ("Instantiating socket connection...", NTLog::LL_INFO);

    client = new NTClient(serverAddress, serverPort, password, endpointId, serverSecure);
    connect (client, &NTClient::streamParametersSet, this, &NTEndpoint::onStreamParametersSet);
    connect (client, &NTClient::volumeSet, this, &NTEndpoint::onVolumeSet);
    connect (client, &NTClient::playRequest, this, &NTEndpoint::onPlayRequest);
    connect (client, &NTClient::stopRequest, this, &NTEndpoint::onStopRequest);
    connect (client, &NTClient::disconnected, this, &NTEndpoint::onServerDisconnect);

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
    reconnectOnLoss = settings.value("reconnect_on_loss", true).toBool();
    settings.endGroup();
}

void NTEndpoint::log(QString message, NTLog::LogLevel level)
{
    NTLog::instance->log(message, level, "endpt");
}

void NTEndpoint::onVolumeSet(uint vol)
{
    log (QString("Volume altering command received, setting volume to %1/100").arg(vol));
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

    log (QString("New stream parameters received: mount %1; port %2").arg(mount).arg(port), NTLog::LL_INFO);

    QString url = QString("http://endpoint_%1:%2@%3:%4/%5")
            .arg(endpointId).arg(password).arg(serverAddress)
            .arg(port).arg(mount);

    QString maskedUrl = url;
    maskedUrl.replace(password, "<masked_password>");

    log ("New stream URL will look like this: "+maskedUrl);

    player->setUrl(QUrl(url));
}

void NTEndpoint::onPlayRequest()
{
    log ("Play start requested", NTLog::LL_INFO);
    int playResult = player->playUrl();

    if (playResult != 0)
        log (QString("Play was not successful, code %1").arg(playResult), NTLog::LL_WARNING);
    else
        log ("Play start OK");
}

void NTEndpoint::onStopRequest()
{
    log ("Play stop requested", NTLog::LL_INFO);
    player->stop();
}

void NTEndpoint::onAuthResult(bool res)
{
    if (!res)
    {
        log ("Could not authorize on the server, exiting.", NTLog::LL_ERROR);
        exit(2);
    }
        else
    {
        log ("Auth OK");
    }
}

void NTEndpoint::onServerDisconnect()
{
    if (reconnectOnLoss)
        client->start();
}
