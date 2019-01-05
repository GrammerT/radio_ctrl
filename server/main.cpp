#include <QCoreApplication>
#include <QObject>
#include "zmq_thread.h"
#include "triggerpocess.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    zmq_thread m_thread;
//    triggerPocess m_trigger;
//    QObject::connect(&m_trigger, SIGNAL(trigger()), &m_thread, SLOT(trigged()));
    m_thread.start();
//    m_trigger.start();
    return a.exec();
}
