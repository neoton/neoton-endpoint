#include "ntclient.h"

NTClient::NTClient(QObject *parent) : QObject(parent)
{

}

void NTClient::log(QString message, NTLog::LogLevel level, QString component)
{
    NTLog::instance->log(message, level, component);
}

void NTClient::processCommand(QString commandLine)
{
    log ("Server command: "+commandLine);

    QStringList commands = commandLine.split(" ", QString::SkipEmptyParts);
    QString fulltext = command.mid(command.indexOf("#")+1);

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
            emit onAuth(true);
            return;
        }

        if (commands[1] == "ERROR")
        {
            log ("Server declined our attempt, auth failed!", NTLog::LL_ERROR);
            emit onAuth(false);
            return;
        }

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

    if (commands[0] == "MOUNT")
    {
        if (commands.count() < 2)
        {
            log ("Bad syntax for MOUNT!", NTLog::LL_WARNING);
            return;
        }

        emit mountSet(commands[1]);
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
        int volume = commands[1].toInt(&ok);

        if (!ok || volume < 0 || volume > 100)
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
