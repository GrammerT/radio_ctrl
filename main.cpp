#include "widget.h"
#include <QApplication>
#include <QDebug>
#include <QDir>
#include "newwidget.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setApplicationName("Tool");
    qDebug() << QApplication::applicationDirPath();
    qDebug() << QDir::currentPath();
    NewWidget w1;
    w1.show();
//    Widget w;
//    w.show();
    return a.exec();
}
