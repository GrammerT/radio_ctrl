#include "zmq_thread.h"
#include "zmq.h"
#include "spi_opt.h"
#include "memdev.h"


#include <QJsonDocument>
#include <QByteArray>
#include <QJsonObject>
#include <QDebug>
#include <QFile>



#define TYPE_SPI0 0X00000001
#define TYPE_SPI1 0X00000002
#define TYPE_REG  0X00000003


 



zmq_thread::zmq_thread()
{

}


void zmq_thread::run()
{
    char json[102400];

    void *context = zmq_ctx_new();
    void *responder = zmq_socket (context, ZMQ_REP);
    int rc = zmq_bind (responder, "tcp://*:5555");
    qDebug("rc=%d",rc);
    /**********************init config**************************/
    QFile m_conf("./server.conf");
    if (!m_conf.open(QIODevice::ReadOnly)) {
        qDebug() << "read json file failed";
    } else {
        QByteArray m_jarray = m_conf.readAll();
        m_conf.flush();
        m_conf.close();
        qDebug()<<QString(m_jarray);
    //    processCmd(m_jarray);
        QJsonParseError json_error;
        QJsonDocument prase_document = QJsonDocument::fromJson(m_jarray, &json_error);

    }

    /*******************************************************************/
    while (1) {
        zmq_msg_t message;
        zmq_msg_init(&message);
        int size = zmq_msg_recv(&message, responder, 0);
        qDebug("size = %d", size);
        if (size == -1)
            qDebug("error zmq_msg_recv");
        memset(json,0,102400);
        if (size<102400)
            memcpy(json, zmq_msg_data(&message), size);
        QByteArray m_jarray(json, size);
        qDebug()<<QString(m_jarray);
        processCmd(m_jarray);
        zmq_msg_t msg_rep;
        zmq_msg_init_size (&msg_rep, 10);
        memcpy(zmq_msg_data(&msg_rep), "ok", 3);
        zmq_msg_send(&msg_rep, responder, 0);
    }//end while
}

void zmq_thread::processCmd(QByteArray m_cmd)
{

}

void zmq_thread::trigged()
{

}

