#ifndef NTBASSPLAYER_H
#define NTBASSPLAYER_H

/*

    This is Neoton, a public broadcasting system
       (c) Asterleen ~ https://asterleen.com
    Licensed under BSD 3-Clause License, see LICENSE

*/

#include <QObject>
#include <QString>
#include <QList>
#include <QUrl>

#include "bass.h"
#include "chiptune.h"

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
    static AudioDevice deviceInfo(int device);
    int device();

    int init(int device = -1, int samplingRate = 48000);
    void free();

    void setUrl(QUrl url);
    int setVolume(uint vol);

    int playUrl();
    int playTest();
    void stop();

    int bassStatus();

private:
    QUrl streamUrl;

    HSTREAM bassStream;

signals:

public slots:
};

#endif // NTBASSPLAYER_H
