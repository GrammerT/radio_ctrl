#ifndef NETWORKTASKWORKER_H
#define NETWORKTASKWORKER_H

#include <QObject>
#include "ratio_dll.h"
#include "zmq.h"
#include <string>


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

    //!  call ratio_dll
    //!
    void setFilePath(QString datapath);
    void onSetParam(double DDS1Freq,
                    double DDS2Freq,
                    int DDS2Phase,
                    double inputPower,
                    double outputPower,
                    int is_400);
    void onJumpFreq(double DDS1Freq, double DDS2Freq, quint32 t0, quint32 t1);
    void onPhaseLock(double dds1Freq,double OutputPwd);
    void onStartScan(float freq_start,float freq_step,
                     float freq_stop,int freq_enable,
                     const char** mpower_tabel_file_path, float power_start, float power_step,
                     float power_stop, int up_power_enable);
    void readFilePath(const char **file);

    void recordMsg(std::string str);
signals:
    void connected();
    void sendFinished();

private:
    void *m_pContext;
    void *request;
    int conn_flag;
};

#endif // NETWORKTASKWORKER_H
