#include "NetworkTaskWorker.h"
#include <QDebug>
#include <string>
#include <fstream>

NetworkTaskWorker::NetworkTaskWorker(QObject *parent)
    : QObject(parent)
{
    conn_flag = 0;

#ifdef ZMQ_DLL
    m_pContext = zmq_ctx_new();
#else
    m_pContext = initRatioCtrl();
#endif
}

NetworkTaskWorker::~NetworkTaskWorker()
{
//    zmq_ctx_destroy(m_pContext);
    destroyRatioCtrl(m_pContext);
}

void NetworkTaskWorker::onConnectToServer(QString ipAddr)
{
    int rc;
#ifdef ZMQ_DLL
    request = zmq_socket(m_pContext, ZMQ_REQ);
    QString ip = "tcp://" + ipAddr + ":5555";
    qDebug() << ip;
    rc = zmq_connect(request, ip.toLatin1().data());
    recordMsg(ip.toStdString());
#else
    const char *ip = (const char *)malloc(ipAddr.toLocal8Bit().size()+1);
    memset((void*)ip,0,ipAddr.toLocal8Bit().size()+1);
    memcpy((void*)ip,ipAddr.toLocal8Bit().data(),ipAddr.toLocal8Bit().size()+1);
    rc = connectTo(&ip);
    free((void*)ip);
#endif

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
    zmq_msg_t msg_rep;
    zmq_msg_init_size (&msg_rep, bytes.size());
    if(conn_flag == 1)
    {
        qDebug()<<"onsend message";
        zmq_msg_init_size (&message, bytes.length());
        memcpy(zmq_msg_data(&message), bytes.data(), bytes.length());
//        recordMsg(QString::fromLocal8Bit(m_jarray).toStdString());
        zmq_msg_send(&message, request, 0);
        zmq_msg_recv(&msg_rep, request, 0);
        emit sendFinished();
    }
}

void NetworkTaskWorker::setFilePath(QString datapath)
{
    const char *down = 0;
    const char *search = 0;
    const char *up = 0;


    QString tmp = datapath+"/down_conversion.txt";
    qDebug()<<tmp<<"--"<<strlen(tmp.toLocal8Bit().data());
    tmp = tmp.replace('/','\\');
    QByteArray ba = tmp.toLatin1();
    down = (const char *)malloc(strlen(ba.data())+1);
    memset((void*)down,0,strlen(ba.data())+1);
    memcpy((void*)down,(void*)ba.data(),strlen(ba.data())+1);

    {
//        readFilePath(&down);
    }


    tmp=datapath+"/search_down_conversion.txt";
    qDebug()<<tmp<<"--"<<tmp.size();
    tmp = tmp.replace('/','\\');
    search = (const char *)malloc(strlen(ba.data())+1);
    memset((void*)search,0,strlen(ba.data())+1);
    memcpy((void*)search,ba.data(),strlen(ba.data())+1);


    tmp=datapath+"/up_conversion.txt";
    qDebug()<<tmp<<"--"<<tmp.size();
    tmp = tmp.replace('/','\\');
    up = (const char *)malloc(strlen(ba.data())+1);
    memset((void*)up,0,strlen(ba.data())+1);
    memcpy((void*)up,tmp.toLocal8Bit().data(),strlen(ba.data())+1);

    loadConversionAndUpConversion(&down,&search,&up);
    free((void*)down);
    free((void*)search);
    free((void*)up);

}

void NetworkTaskWorker::onSetParam(double DDS1Freq, double DDS2Freq, int DDS2Phase, double inputPower, double outputPower, int is_400)
{
    sendSetParamMsg(DDS1Freq,DDS2Freq,DDS2Phase,inputPower,outputPower,is_400);
    emit sendFinished();
}

void NetworkTaskWorker::onJumpFreq(double DDS1Freq, double DDS2Freq,quint32 t0,quint32 t1)
{
    long long mDDS1word = (DDS1Freq/100000000.0)*(((long long)1) << 48);
    long long mDDS2word = (DDS2Freq/ 100000000.0)* (((long long)1) << 48);
    qDebug()<<mDDS1word<<" -- "<<mDDS2word;



    sendSetJumpFreqParamMsg(DDS1Freq,DDS2Freq,t0,t1,1);
    emit sendFinished();
}

void NetworkTaskWorker::onPhaseLock(double dds1Freq, double OutputPwd)
{
    sendPhaseLockMsg(dds1Freq,OutputPwd);
    emit sendFinished();
}

void NetworkTaskWorker::onStartScan(float freq_start, float freq_step, float freq_stop, int freq_enable, const char **mpower_tabel_file_path, float power_start, float power_step, float power_stop, int up_power_enable)
{
    sendStartScanMsg(freq_start,freq_step,freq_stop,freq_enable,mpower_tabel_file_path,power_start,power_step,power_stop,up_power_enable);
    emit sendFinished();
}

void NetworkTaskWorker::readFilePath(const char **file)
{

    const char * downconversionpath = *file;
    std::string m1 = downconversionpath;

    std::ifstream fs(downconversionpath,std::ios::out);
    if(!fs.is_open())
    {
        recordMsg(m1+" error!!!");
        qDebug()<<downconversionpath<<" can't open!!!";
        return ;
    }
}

void NetworkTaskWorker::recordMsg(std::string str)
{

    std::fstream fs;
    fs.open("ui_msg.txt",std::ios::app);
    if(fs.is_open())
    {
        fs<<str<<"\n";
        fs.close();
    }
}
