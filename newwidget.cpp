#include "newwidget.h"
#include "ui_newwidget.h"
#include <QFile>
#include <QTextStream>

NewWidget::NewWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::NewWidget)
{
    QFile file(":/theme.qss");
    file.open(QIODevice::ReadOnly);
    QTextStream out(&file);
    this->setStyleSheet(out.readAll());
    file.close();
    ui->setupUi(this);
    initWidget();
}

NewWidget::~NewWidget()
{
    delete ui;
}

void NewWidget::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);
}


void NewWidget::initWidget()
{
    QPalette palette = this->palette();
    palette.setBrush(QPalette::Window,
            QBrush(QPixmap(":/image/background.png").scaled(// 缩放背景图.
                this->size(),
                Qt::IgnoreAspectRatio,
                Qt::SmoothTransformation)));             // 使用平滑的缩放方式
    this->setPalette(palette);
}
