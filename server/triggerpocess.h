#ifndef TRIGGERPOCESS_H
#define TRIGGERPOCESS_H
#include <QObject>
#include <QThread>
#include <QByteArray>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <fcntl.h>
#include <ctype.h>
#include <sys/types.h>


class triggerPocess : public QThread
{
Q_OBJECT
public:
    triggerPocess();

protected:
    void run();
private:
    int trigger_fd;
public:
signals:
    void trigger();

};

#endif // TRIGGERPOCESS_H
