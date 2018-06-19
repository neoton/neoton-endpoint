#ifndef NTCLIENT_H
#define NTCLIENT_H

#include <QObject>

class NTClient : public QObject
{
    Q_OBJECT
public:
    explicit NTClient(QObject *parent = 0);

signals:

public slots:
};

#endif // NTCLIENT_H
