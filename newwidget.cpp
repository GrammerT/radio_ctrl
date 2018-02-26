#include "newwidget.h"
#include "ui_newwidget.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>

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
    initControl(false);
    on_m_pDSpinBoxInputFreq_valueChanged(ui->m_pDSpinBoxInputFreq->value());
}

void NewWidget::initControl(bool)
{
    ui->m_pLineEdit300_400->setEnabled(false);
    ui->m_pDSpinBoxDROFreq->setEnabled(false);
    ui->m_pDSpinBoxOutputFreq->setEnabled(false);
}

void NewWidget::on_m_pDSpinBoxInputFreq_valueChanged(double arg1)
{
    if(arg1>=9910000000&&arg1<10010000000)
    {
        ui->m_pLineEdit300_400->setText("300 MHz");
    }
    else
    {
        ui->m_pLineEdit300_400->setText("400 MHz");
    }
}

void NewWidget::on_m_pCBLinkage_toggled(bool checked)
{
    qDebug()<<checked;
    if(checked)
    {
        double inputFrequency = ui->m_pDSpinBoxInputFreq->value();
        double DDS1Value = inputFrequency - 9600000000.0 - (ui->m_pLineEdit300_400->text().toInt()*1000000.0);
        ui->m_pDSpinBoxDDS1Freq->setValue(DDS1Value);
    }
}
