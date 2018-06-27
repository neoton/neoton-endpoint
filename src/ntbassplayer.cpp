#include "ntbassplayer.h"

/*

    This is Neoton, a public broadcasting system
       (c) Asterleen ~ https://asterleen.com
    Licensed under BSD 3-Clause License, see LICENSE

*/

NTBassPlayer::NTBassPlayer(QObject *parent) : QObject(parent)
{

}

QList<NTBassPlayer::AudioDevice> NTBassPlayer::listDevices()
{
    QList<AudioDevice> devicesList;

    int currentDevice = 0;
    bool devicesLeft = true;

    do {
        AudioDevice tmp = deviceInfo(currentDevice);

        if (tmp.id > -1)
            devicesList.append(tmp);
        else
            devicesLeft = false;

        currentDevice++;
    } while (devicesLeft);

    return devicesList;
}

NTBassPlayer::AudioDevice NTBassPlayer::deviceInfo(int device)
{
    BASS_DEVICEINFO info;

    AudioDevice dev;
    dev.id = -1;

    if (device < 0)
        return dev;

    if (BASS_GetDeviceInfo(device, &info) && (info.flags & BASS_DEVICE_ENABLED))
    {
        dev.id = device;
        dev.name = QString::fromLocal8Bit(info.name);
        dev.driver = QString::fromLocal8Bit(info.driver);
    }

    return dev;
}

int NTBassPlayer::device()
{
    return BASS_GetDevice();
}


int NTBassPlayer::init(int device, int samplingRate)
{
    if (BASS_Init(device, samplingRate, 0, 0, 0))
        return 0;
    else
        return BASS_ErrorGetCode();
}

void NTBassPlayer::free()
{
    BASS_Free();
}

void NTBassPlayer::setUrl(QUrl url)
{
    streamUrl = url;
}

int NTBassPlayer::setVolume(uint vol)
{
    if (vol > 100)
        return -2;

    if (bassStream == 0)
        return -3;

    if (BASS_ChannelSetAttribute(bassStream, BASS_ATTRIB_VOL, vol / 100))
        return 0;
    else
        return BASS_ErrorGetCode();
}

int NTBassPlayer::playUrl()
{
    stop();

    bassStream = BASS_StreamCreateURL(streamUrl.toString().toLocal8Bit().data(), 0, BASS_STREAM_AUTOFREE, 0, 0);

    if (bassStream == 0)
        return BASS_ErrorGetCode() + 100;

    if (BASS_ChannelPlay(bassStream, false))
        return 0;
    else
        return BASS_ErrorGetCode() + 200;

}

int NTBassPlayer::playTest()
{
    stop();

    bassStream = (HSTREAM)BASS_MusicLoad(true, &chiptune, 0, SZ_CHIPTUNE, BASS_MUSIC_RAMP, 0);

    if (bassStream == 0)
        return BASS_ErrorGetCode() + 100;

    if (BASS_ChannelPlay(bassStream, false))
        return 0;
    else
        return BASS_ErrorGetCode() + 200;
}

void NTBassPlayer::stop()
{
    if (bassStream != 0)
    {
        BASS_StreamFree(bassStream);
        bassStream = 0;
    }
}

int NTBassPlayer::bassStatus()
{
    return BASS_ChannelIsActive(bassStream);
}
