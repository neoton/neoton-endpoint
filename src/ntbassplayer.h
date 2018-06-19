#ifndef NTBASSPLAYER_H
#define NTBASSPLAYER_H

#include <QObject>
#include <QString>
#include <QList>
#include <QUrl>

#include "bass.h"

class NTBassPlayer : public QObject
{
    Q_OBJECT
public:
    explicit NTBassPlayer(QObject *parent = 0);

    struct AudioDevice {
        int id;
        QString name;
        QString driver;
    };

    static QList<AudioDevice> listDevices();

    int init(int device = -1, int samplingRate = 48000);
    void free();

    void setUrl(QUrl url);

    int playUrl();
    void playTest();

private:
    int channelHandle;
    QUrl streamUrl;
    HSTREAM bassStream;

signals:

public slots:
};

#endif // NTBASSPLAYER_H
