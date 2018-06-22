#ifndef NTCLIENT_H
#define NTCLIENT_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QWebSocket>

#include "ntlog.h"
#include "ntauth.h"

class NTClient : public QObject
{
    Q_OBJECT
public:
    explicit NTClient(QObject *parent = 0);

private:
    QWebSocket *sock;

    QString address;
    QString password;
    uint port;

    void log (QString message, NTLog::LogLevel level = NTLog::LL_DEBUG, QString component = "clien");

signals:
    void authProcessed (bool);
    void mountSet (QString);
    void volumeSet (int);
    void serverError (int, QString);
    void playRequest();
    void stopRequest();

private slots:
    void processCommand (QString commandLine);

public slots:
};

#endif // NTCLIENT_H
