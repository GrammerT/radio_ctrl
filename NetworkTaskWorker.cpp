#include "NetworkTaskWorker.h"
#include <QDebug>

NetworkTaskWorker::NetworkTaskWorker(QObject *parent)
    : QObject(parent)
{
    conn_flag = 0;
    m_pContext = zmq_ctx_new();
}

NetworkTaskWorker::~NetworkTaskWorker()
{
    zmq_ctx_destroy(m_pContext);
}

void NetworkTaskWorker::onConnectToServer(QString ipAddr)
{
    int rc;
    request = zmq_socket(m_pContext, ZMQ_REQ);
    QString ip = "tcp://" + ipAddr + ":5555";
    qDebug() << ip;
    rc = zmq_connect(request, ip.toLatin1().data());
    if(rc < 0)
    {
        conn_flag = 0;
    }
    else
    {
        conn_flag = 1;
        emit connected();
    }
}

void NetworkTaskWorker::onSendMessage(QByteArray bytes)
{
    zmq_msg_t message;
    if(conn_flag == 1)
    {
        qDebug()<<"onsend message";
        zmq_msg_init_size (&message, bytes.length());
        memcpy(zmq_msg_data(&message), bytes.data(), bytes.length());
        zmq_msg_send(&message, request, 0);
        zmq_msg_recv(&message, request, 0);
        emit sendFinished();
    }
}
