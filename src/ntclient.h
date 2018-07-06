#ifndef NTCLIENT_H
#define NTCLIENT_H

/*

    This is Neoton, a public broadcasting system
       (c) Asterleen ~ https://asterleen.com
    Licensed under BSD 3-Clause License, see LICENSE

*/

#include <QObject>
#include <QString>
#include <QStringList>
#include <QAbstractSocket>
#include <QWebSocket>

#include "ntlog.h"
#include "ntauth.h"

class NTClient : public QObject
{
    Q_OBJECT
public:
    explicit NTClient(QString address, uint port, QString password, int endpointId,
                      bool isSecure = false, QObject *parent = 0);

    void start();
    void stop();
    void sendCommand (QString command);

private:
    QWebSocket *sock;

    QString address;
    uint port;
    QString password;
    int endpointId;
    bool isSecure;


    void log (QString message, NTLog::LogLevel level = NTLog::LL_DEBUG, QString component = "clien");

signals:
    void authProcessed (bool);
    void streamParametersSet (QString, uint);
    void volumeSet(uint);
    void serverError (int, QString);
    void playRequest();
    void stopRequest();
    void connected();
    void disconnected();

private slots:
    void processCommand (QString commandLine);

    void onSocketConnect();
    void onSocketDisconnect();
    void onSocketError(QAbstractSocket::SocketError error);
    void onSocketStateChange (QAbstractSocket::SocketState state);

public slots:
};

#endif // NTCLIENT_H
