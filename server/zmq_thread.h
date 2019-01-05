#ifndef ZMQ_THREAD_H
#define ZMQ_THREAD_H

#include <QObject>
#include <QThread>
#include <QByteArray>
#include <QVector>
#include <QMutex>



#include "zmq.h"


typedef struct freq_scan {
    double start;
    double step;
    double stop;

}FREQ_SCAN;

typedef struct  power_scan {
    double start;
    double step;
    double stop;
}POWER_SCAN;

typedef struct UPConversion
{
    float value;
    int RF;
    int LF_ONE;
    int LF_TWO;
}UPCONVERSION;


class zmq_thread : public QThread
{
Q_OBJECT
public:
    zmq_thread();
protected:
    void run();
private:
    int dir_freq;
    int dir_power;
    double cur_freq;
    double cur_power;
    QMutex mutex;
    QVector<UPConversion> mUpConversionVec;
    struct freq_scan m_freq_scan;
    struct power_scan m_power_scan;
    volatile int enable_freq_trig;
    volatile int enable_power_trig;
    void save_conf(QByteArray m_data);

    void processCmd(QByteArray m_cmd);

public slots:
    void trigged();
};

#endif // ZMQ_THREAD_H
