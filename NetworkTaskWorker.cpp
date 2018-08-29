#include "NetworkTaskWorker.h"
#include <QDebug>

NetworkTaskWorker::NetworkTaskWorker(QObject *parent)
    : QObject(parent)
{
    conn_flag = 0;
//    m_pContext = zmq_ctx_new();
    m_pContext = initRatioCtrl();
}

NetworkTaskWorker::~NetworkTaskWorker()
{
//    zmq_ctx_destroy(m_pContext);
    destroyRatioCtrl(m_pContext);
}

void NetworkTaskWorker::onConnectToServer(QString ipAddr)
{
    int rc;
//    request = zmq_socket(m_pContext, ZMQ_REQ);
//    QString ip = "tcp://" + ipAddr + ":5555";
//    qDebug() << ip;
//    rc = zmq_connect(request, ip.toLatin1().data());
    const char *ip = ipAddr.toLocal8Bit().data();
    rc = connectTo(&ip);
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
//    zmq_msg_t message;
//    zmq_msg_t msg_rep;
//    zmq_msg_init_size (&msg_rep, 100);
//    if(conn_flag == 1)
//    {
//        qDebug()<<"onsend message";
//        zmq_msg_init_size (&message, bytes.length());
//        memcpy(zmq_msg_data(&message), bytes.data(), bytes.length());
//        char json[102400];
//        memset(json,0,102400);
//        if (bytes.length()<102400)
//            memcpy(json, zmq_msg_data(&message), bytes.length());
//        QByteArray m_jarray(json, bytes.length());
//        qDebug()<<"--------server-----------"<<m_jarray;
//        zmq_msg_send(&message, request, 0);
//        zmq_msg_recv(&msg_rep, request, 0);
//        emit sendFinished();
    //    }
}

void NetworkTaskWorker::setFilePath(QString datapath)
{
    QString tmp = datapath+"\\down_conversion.txt";
    const char *down = tmp.toLocal8Bit().data();
    tmp=datapath+"\\search_down_conversion.txt";
    const char *search =  tmp.toLocal8Bit().data();
    tmp=datapath+"\\up_conversion.txt";
//    qDebug()<<tmp.toLocal8Bit().
    const char *up = tmp.toLocal8Bit().data();
    loadConversionAndUpConversion(&down,&search,&up);
}

void NetworkTaskWorker::onSetParam(double DDS1Freq, double DDS2Freq, int DDS2Phase, int inputPower, int outputPower, int is_400)
{
    sendSetParamMsg(DDS1Freq,DDS2Freq,DDS2Phase,inputPower,outputPower,is_400);
}

void NetworkTaskWorker::onPhaseLock(double dds1Freq, double OutputPwd)
{
    sendPhaseLockMsg(dds1Freq,OutputPwd);
}

void NetworkTaskWorker::onStartScan(float freq_start, float freq_step, float freq_stop, int freq_enable, const char **mpower_tabel_file_path, float power_start, float power_step, float power_stop, int up_power_enable)
{
    sendStartScanMsg(freq_start,freq_step,freq_stop,freq_enable,mpower_tabel_file_path,power_start,power_step,power_stop,up_power_enable);
}
