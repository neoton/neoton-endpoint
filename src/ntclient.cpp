#include "ntclient.h"

/*

    This is Neoton, a public broadcasting system
       (c) Asterleen ~ https://asterleen.com
    Licensed under BSD 3-Clause License, see LICENSE

*/

NTClient::NTClient(QString address, uint port, QString password, int endpointId, bool isSecure, QObject *parent) :
        QObject(parent), address(address), port(port), password(password), endpointId(endpointId), isSecure(isSecure)
{
    sock = new QWebSocket();
    connect(sock, &QWebSocket::connected, this, &NTClient::onSocketConnect);
    connect(sock, &QWebSocket::disconnected, this, &NTClient::onSocketDisconnect);
    connect(sock, &QWebSocket::textMessageReceived, this, &NTClient::processCommand);
    connect(sock, &QWebSocket::stateChanged, this, &NTClient::onSocketStateChange);

    // oh god... https://forum.qt.io/topic/54264/connect-unresolved-overloaded-function-type
    connect(sock, static_cast<void (QWebSocket::*) (QAbstractSocket::SocketError)>(&QWebSocket::error), this, &NTClient::onSocketError);
}

void NTClient::start()
{
    if (sock->state() == QAbstractSocket::ConnectedState)
        sock->close();

   QUrl link((isSecure ? "wss://" : "ws://")+address+":"+QString::number(port));

   log ("Connecting to "+link.toString());

    sock->open(link);
}

void NTClient::stop()
{
    sock->close();
}

void NTClient::sendCommand(QString command)
{
    log ("Send: "+command);

    if (sock->state() == QAbstractSocket::ConnectedState)
        sock->sendTextMessage(command);
    else
        log ("Trying to send message when socket is not connected!", NTLog::LL_WARNING);
}

void NTClient::log(QString message, NTLog::LogLevel level, QString component)
{
    NTLog::instance->log(message, level, component);
}

void NTClient::processCommand(QString commandLine)
{
    log ("Server command: "+commandLine);

    QStringList commands = commandLine.split(" ", QString::SkipEmptyParts);
    QString fulltext = commandLine.mid(commandLine.indexOf("#")+1);

    if (commands.count() == 0)
        return;

    if (commands[0] == "AUTH")
    {
        if (commands.count() < 2)
        {
            log ("Bad server command: not enough parameters");
            return;
        }

        if (commands[1] == "OK")
        {
            log ("Server accepted us as an endpoint, auth done!");
            emit authProcessed(true);
            return;
        }

        if (commands[1] == "ERROR")
        {
            log ("Server declined our attempt, auth failed!", NTLog::LL_ERROR);
            emit authProcessed(false);
            return;
        }

        QString challengeResponse = NTAuth::sha256(commands[1] + NTAuth::sha256(password));
        sendCommand(QString("AUTH %1 %2").arg(endpointId).arg(challengeResponse));

        return;
    }

    if (commands[0] == "ERROR")
    {
        if (commands.count() < 2)
        {
            log ("Bad syntax in command!", NTLog::LL_WARNING);
            return;
        }

        bool ok;
        int errorCode = commands[1].toInt(&ok);

        if (!ok)
        {
            log ("Bad syntax in command!", NTLog::LL_WARNING);
            return;
        }

        emit serverError(errorCode, fulltext);
        return;
    }

    if (commands[0] == "STREAM")
    {
        if (commands.count() < 3)
        {
            log ("Bad syntax for STREAM!", NTLog::LL_WARNING);
            return;
        }

        bool ok;
        uint port = commands[2].toUInt(&ok);

        if (!ok || port == 0 || port > 65536)
        {
            log ("Bad syntax for STREAM!", NTLog::LL_WARNING);
            return;
        }

        emit streamParametersSet(QString(commands[1]), port);
        return;
    }

    if (commands[0] == "VOLUME")
    {
        if (commands.count() < 2)
        {
            log ("Bad syntax for VOLUME!", NTLog::LL_WARNING);
            return;
        }

        bool ok;
        uint volume = commands[1].toUInt(&ok);

        if (!ok || volume > 100)
        {
            log ("Bad syntax for VOLUME!", NTLog::LL_WARNING);
            return;
        }

        emit volumeSet(volume);
        return;
    }

    if (commands[0] == "PLAY")
    {
        emit playRequest();
    }

    if (commands[0] == "STOP")
    {
        emit stopRequest();
    }
}

void NTClient::onSocketConnect()
{
    log ("Socket is connected, sending initial command...", NTLog::LL_INFO);
    sendCommand("ENDPOINT");
    emit connected();
}

void NTClient::onSocketDisconnect()
{
    log ("Socket disconnected!", NTLog::LL_WARNING);
    emit disconnected();
}

void NTClient::onSocketError(QAbstractSocket::SocketError error)
{
    log (QString("Socket error #%1").arg(error));
    // emit disconnected(true);
}

void NTClient::onSocketStateChange(QAbstractSocket::SocketState state)
{
    log (QString("Socket changes state to #%1").arg(state));
}
