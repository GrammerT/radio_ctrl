#include "newwidget.h"
#include "ui_newwidget.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QJsonObject>
#include <QJsonDocument>
#include <QByteArray>
#include <QTimer>
#include "NetworkTaskWorker.h"

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
    loadConversion("down_conversion.txt", mDownConversionVec);
    loadConversion("search_down_conversion.txt", mSearchDownConversionVec);
    loadUPConversion("up_conversion.txt", mUpConversionVec);
    outputFreqValue();
    changeNetworkStatus(false);
}

NewWidget::~NewWidget()
{
    delete ui;
}

void NewWidget::onConnected()
{
    changeNetworkStatus(true);
}

void NewWidget::onSendFinished()
{
    ui->m_pStateBar->setText(("\345\217\221\351\200\201\346\210\220\345\212\237"));
    QTimer::singleShot(5000,this,[&](){
        ui->m_pStateBar->clear();
    });
}

void NewWidget::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);
}


void NewWidget::initWidget()
{
    QPalette palette = this->palette();
    palette.setBrush(QPalette::Window,
            QBrush(QPixmap(":/image/background.png").scaled(
                this->size(),
                Qt::IgnoreAspectRatio,
                Qt::SmoothTransformation)));            
    this->setPalette(palette);
    initControl(false);
    on_m_pDSpinBoxInputFreq_valueChanged(ui->m_pDSpinBoxInputFreq->value());
    QRegExp regExp("(([01]\\d{2}|2[0-4]\\d|25[0-5]|\\d{1,2})\\.){3}([01]\\d{2}|2[0-4]\\d|25[0-5]|\\d{1,2})");
    QRegExpValidator *regExpValidator = new QRegExpValidator(regExp);
    ui->m_pLineEditIP->setValidator(regExpValidator);
}

void NewWidget::initControl(bool)
{
    ui->m_pLineEdit300_400->setEnabled(false);
    ui->m_pDSpinBoxDROFreq->setEnabled(false);
    ui->m_pDSpinBoxOutputFreq->setEnabled(false);
}

void NewWidget::outputFreqValue()
{
    double value = ui->m_pDSpinBoxDDS2Freq->value()+300000000.0+ui->m_pDSpinBoxDROFreq->value();
    ui->m_pDSpinBoxOutputFreq->setValue(value);
}

void NewWidget::DDS1Value()
{
    double inputFrequency = ui->m_pDSpinBoxInputFreq->value();
    double DDS1Value = inputFrequency - 9600000000.0 - (ui->m_pLineEdit300_400->text().toInt()*1000000.0);
    ui->m_pDSpinBoxDDS1Freq->setValue(DDS1Value);
}

void NewWidget::DROValue(bool autoCalc)
{
    if(autoCalc)
    {
        ui->m_pDSpinBoxDROFreq->setValue(ui->m_pDSpinBoxInputFreq->value()
                                                                    -ui->m_pDSpinBoxDDS1Freq->value()
                                                                    -(ui->m_pLineEdit300_400->text().toInt()*1000000.0));
    }
    else
    {
        ui->m_pDSpinBoxDROFreq->setValue(9600000000.000000);
    }
}

void NewWidget::changeNetworkStatus(bool status)
{
    if (!status)
    {
        ui->m_pLabelWifiState->setStyleSheet("QLabel{color:rgb(175, 0, 0)}");
        ui->m_pLabelWifiState->setText(("\346\234\252\350\277\236\346\216\245"));
    }
    else
    {
        ui->m_pLabelWifiState->setStyleSheet("QLabel{color:rgb(0, 165, 235)}");
        ui->m_pLabelWifiState->setText(("\345\267\262\350\277\236\346\216\245"));
    }
}

void NewWidget::initNetworkTask()
{
    NetworkTaskWorker *worker = new NetworkTaskWorker;
    worker->moveToThread(&mWorkerThread);
    connect(&mWorkerThread, &QThread::finished, worker, &QObject::deleteLater);
    connect(this, &NewWidget::connectToServer, worker, &NetworkTaskWorker::onConnectToServer);
    connect(this, &NewWidget::sendMessage, worker, &NetworkTaskWorker::onSendMessage);
    connect(worker, &NetworkTaskWorker::connected, this, &NewWidget::onConnected);
    connect(worker, &NetworkTaskWorker::sendFinished, this, &NewWidget::onSendFinished);
    mWorkerThread.start();
}

void NewWidget::loadConversion(const QString &fileName, QVector<Conversion> &conversionVec)
{
    conversionVec.clear();
    QFile file(fileName);
    if(!file.open(QIODevice::ReadOnly))
    {
        qDebug() << fileName << " open error!" << file.exists();
        return;
    }
    QByteArray bytes;

    QRegExp splitExp("\\s+");

    while(!(bytes = file.readLine()).isEmpty())
    {
        QString line = QString::fromLatin1(bytes);
        QStringList wordList = line.split(splitExp);
        if (wordList.size() < 3)
        {
            continue;
        }
        Conversion conversion;
        QString value = wordList[0];
        QString RF = wordList[1];
        QString LF = wordList[2];
        conversion.value = value.toFloat();
        conversion.RF = RF.toInt();
        conversion.LF = LF.toInt();

        conversionVec.push_back(conversion);
    }
    file.close();
}

void NewWidget::loadUPConversion(const QString &fileName, QVector<UPConversion> &conversionVec)
{
    conversionVec.clear();

    QFile file(fileName);

    if(!file.open(QIODevice::ReadOnly))
    {
        qDebug() << fileName << " open error!" << file.exists();
        return;
    }

    QByteArray bytes;

    QRegExp splitExp("\\s+");

    while(!(bytes = file.readLine()).isEmpty())
    {
        QString line = QString::fromLatin1(bytes);
        QStringList wordList = line.split(splitExp);

        if (wordList.size() < 4)
        {
            continue;
        }

        UPConversion conversion;
        QString value = wordList[0];
        QString RF = wordList[1];
        QString LF_ONE = wordList[2];
        QString LF_TWO = wordList[3];
        conversion.value = value.toFloat();
        conversion.RF = RF.toInt();
        conversion.LF_ONE = LF_ONE.toInt();
        conversion.LF_TWO = LF_TWO.toInt();

        conversionVec.push_back(conversion);
    }

    file.close();
}

int NewWidget::findAttenuation(double powerValue, const QVector<Conversion> &conversionVec)
{
    int index = 0;
    double minDistance = 0.0;
    for(int i = 0; i < conversionVec.size(); ++i)
    {
        double distance = fabs(powerValue - conversionVec[i].value);
        if(0 == i)
        {
            minDistance = distance;
            index = i;
        }
        else
        {
            if (distance < minDistance)
            {
                minDistance = distance;
                index = i;
            }
        }
    }

    return index;
}

int NewWidget::findUPAttenuation(double powerValue, const QVector<UPConversion> &conversionVec)
{
    int index = 0;
    double minDistance = 0.0;
    for(int i = 0; i < conversionVec.size(); ++i)
    {
        double distance = fabs(powerValue - conversionVec[i].value);
        if(0 == i)
        {
            minDistance = distance;
            index = i;
        }
        else
        {
            if (distance < minDistance)
            {
                minDistance = distance;
                index = i;
            }
        }
    }
    return index;
}

void NewWidget::on_m_pDSpinBoxInputFreq_valueChanged(double arg1)
{
    if(arg1<10010000000)
    {
        ui->m_pLineEdit300_400->setText("300 MHz");
    }
    else
    {
        ui->m_pLineEdit300_400->setText("400 MHz");
    }
    if(!ui->m_pCBLinkage->isChecked())
    {
        DROValue(true);
    }
    else
    {
        DDS1Value();
    }
}

void NewWidget::on_m_pCBLinkage_toggled(bool checked)
{
    qDebug()<<checked;
    if(checked)
    {
        DDS1Value();
        DROValue(false);
        ui->m_pDSpinBoxDDS2Freq->setValue(107368230.000000);
        ui->m_pSpinBoxDDS2Phase->setValue(0);
        ui->m_pDSpinBoxDDS1Freq->setEnabled(!checked);
        ui->m_pDSpinBoxDDS2Freq->setEnabled(!checked);
        ui->m_pSpinBoxDDS2Phase->setEnabled(!checked);
    }
    else
    {
        DROValue(true);
        ui->m_pDSpinBoxDDS1Freq->setEnabled(!checked);
        ui->m_pDSpinBoxDDS2Freq->setEnabled(!checked);
        ui->m_pSpinBoxDDS2Phase->setEnabled(!checked);
    }

}

void NewWidget::on_m_pBtnHelp_clicked()
{


}

void NewWidget::on_m_pBtnConnWifi_clicked()
{
    QString ipAddr = ui->m_pLineEditIP->text();
    emit connectToServer(ipAddr);
}

void NewWidget::on_m_pBtnPhaseLock_clicked()
{
    qint64 mDDS1ControlWord = (ui->m_pDSpinBoxDDS1Freq->value() / 400.0) * (((long long)1) << 48);
    int inputIndex = findAttenuation(ui->m_pDSpinBoxOutputPower->value(), mSearchDownConversionVec);

    //! pendding para en
    int en = 0;

    QJsonObject sendObject;

    sendObject.insert("type", "search");
    sendObject.insert("dds1_control", QString::number(mDDS1ControlWord));
    sendObject.insert("dds1_rf", mSearchDownConversionVec[inputIndex].RF);
    sendObject.insert("dds1_lf", mSearchDownConversionVec[inputIndex].LF);
    sendObject.insert("en", en);

    QJsonDocument document;

    document.setObject(sendObject);
    QByteArray bytes = document.toJson();

    emit sendMessage(bytes);
}

void NewWidget::on_m_pBtnSendMsg_clicked()
{
    qint64 mDDS1ControlWord = (ui->m_pDSpinBoxDDS1Freq->value() / 400.0) * (((long long)1) << 48);
    qint64 mDDS2ControlWord = (ui->m_pDSpinBoxDDS2Freq->value() / 300.0) * (((long long)1) << 48);
    int inputIndex = findAttenuation(ui->m_pDSpinBoxOutputPower->value(), mDownConversionVec);
    int outputIndex = findUPAttenuation(ui->m_pDSpinBoxOutputPower->value(), mUpConversionVec);
    bool mFilterWord = ui->m_pLineEdit300_400->text().toInt()>400?true:false;

    QJsonObject sendObject;

    sendObject.insert("type", "normal");
    sendObject.insert("filter_word", mFilterWord);
    sendObject.insert("lo", ui->m_pLineEdit300_400->text().toInt());

    sendObject.insert("dds1_control", QString::number(mDDS1ControlWord));
    sendObject.insert("dds2_control", QString::number(mDDS2ControlWord));

    sendObject.insert("dds1_rf", mDownConversionVec[inputIndex].RF);
    sendObject.insert("dds1_lf", mDownConversionVec[inputIndex].LF);
    sendObject.insert("dds2_rf", mUpConversionVec[outputIndex].RF);
    sendObject.insert("dds2_lf_1", mUpConversionVec[outputIndex].LF_ONE);
    sendObject.insert("dds2_lf_2", mUpConversionVec[outputIndex].LF_TWO);


    QJsonDocument document;
    document.setObject(sendObject);
    QByteArray bytes = document.toJson();
    emit sendMessage(bytes);
}
