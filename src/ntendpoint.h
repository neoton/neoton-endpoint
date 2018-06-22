#ifndef NTENDPOINT_H
#define NTENDPOINT_H

/*

    This is Neoton, a public broadcasting system
       (c) Asterleen ~ https://asterleen.com
    Licensed under BSD 3-Clause License, see LICENSE

*/

#include <QObject>
#include <QString>
#include <QList>
#include <QFile>
#include <QSettings>

#include "ntlog.h"
#include "ntclient.h"
#include "ntbassplayer.h"


class NTEndpoint : public QObject
{
    Q_OBJECT
public:
    explicit NTEndpoint(QString config, QObject *parent = 0);

private:
    QString config;
    QString logFile;
    NTLog::LogLevel logLevel;

    int deviceNum;
    int samplingFrequency;

    NTClient *client;
    NTBassPlayer *player;

    QString serverAddress;
    uint serverPort;
    QString password;
    int endpointId;
    bool serverSecure;
    bool reconnectOnError;

    void loadConfig (QString configFile);

    void log (QString message, NTLog::LogLevel level = NTLog::LL_DEBUG);

private slots:
    void onVolumeSet (uint vol);
    void onStreamParametersSet(QString mount, uint port);
    void onPlayRequest();
    void onStopRequest();
    void onAuthResult(bool res);
    void onServerDisconnect();

signals:

public slots:
};

#endif // NTENDPOINT_H
