#include "triggerpocess.h"

triggerPocess::triggerPocess()
{

}

void triggerPocess::run()
{
    char buf[4];
    trigger_fd = open("/dev/skybee", O_RDWR | O_SYNC);
    if(trigger_fd<0)
    {
        qDebug("error open triger");
    }
    while(1)
    {
        int ret = read(trigger_fd, buf, 4);

        if(ret < 0)
        {
            qDebug("error when read");
        }
//        QThread::sleep(1);
        emit trigger();
    }
}
