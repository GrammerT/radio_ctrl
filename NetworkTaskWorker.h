#ifndef NETWORKTASKWORKER_H
#define NETWORKTASKWORKER_H

#include <QObject>
#include "zmq.h"


class NetworkTaskWorker : public QObject
{
    Q_OBJECT
public:
    NetworkTaskWorker(QObject *parent = NULL);
    ~NetworkTaskWorker();

public slots:
    //! thread running work
    void onConnectToServer(QString ipAddr);

    //! jsonFormat
    void onSendMessage(QByteArray bytes);

signals:
    void connected();
    void sendFinished();

private:
    void *m_pContext;
    void *request;
    int conn_flag;
};

#endif // NETWORKTASKWORKER_H
