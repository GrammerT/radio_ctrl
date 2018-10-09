#include "newwidget.h"
#include "ui_newwidget.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QJsonObject>
#include <QJsonDocument>
#include <QByteArray>
#include <QTimer>
#include <QHostAddress>
#include <QMessageBox>
#include <QDir>
#include <QDesktopServices>
#include <QUrl>
#include <QProcess>
#include "NetworkTaskWorker.h"
#include "ratio_dll.h"

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
    initNetworkTask();
    restoreParamsFromJson();
    outputFreqValue();
    changeNetworkStatus(false);
    initComboBox();
    on_m_pComBoxDataList_currentTextChanged(ui->m_pComBoxDataList->currentText());
    this->on_m_pCBLinkage_toggled(true);
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
    requireVec.removeFirst();
    if(requireVec.size()!=0)
    {
        return ;
    }
    if(ui->m_pBtnPhaseLock->text().contains("..."))
    {
        ui->m_pBtnPhaseLock->setText("\351\224\201\347\233\270\346\215\225\350\216\267");
    }
    ui->m_pBtnSendMsg->setEnabled(true);
    ui->m_pBtnPhaseLock->setEnabled(true);
    ui->m_pStateBar->setText(("\345\217\221\351\200\201\346\210\220\345\212\237"));
    QTimer::singleShot(5000,this,[&](){
        ui->m_pStateBar->clear();
    });
}

void NewWidget::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);
}

void NewWidget::closeEvent(QCloseEvent *event)
{
    saveParamsToJson();
    return QWidget::closeEvent(event);
}


void NewWidget::initWidget()
{
    QPalette palette = this->palette();
    palette.setBrush(QPalette::Window,
            QBrush(QPixmap(":/image/background.jpg").scaled(
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
    double value =ui->m_pDSpinBoxDROFreq->value()- ui->m_pDSpinBoxDDS2Freq->value()-
            300000000.0;
    ui->m_pDSpinBoxOutputFreq->setValue(value);
}

void NewWidget::DDS1Value()
{
    double DDS1Value = ui->m_pDSpinBoxInputFreq->value() -
                                        9600000000.0 -
            (ui->m_pLineEdit300_400->text().toInt()*1000000.0);
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
#ifndef ZMQ_DLL
    connect(this,SIGNAL(sig_setpath(QString)),worker,SLOT(setFilePath(QString)));
    connect(this,SIGNAL(sig_phaseLock(double,double)),worker,SLOT(onPhaseLock(double,double)));
    connect(this,SIGNAL(sig_setParam(double,double,int,int,int,int)),worker,SLOT(onSetParam(double,double,int,int,int,int)));
    connect(this,SIGNAL(sig_startScan(float,float,float,int,const char**,float,float,float,int)),
                    worker,SLOT(onStartScan(float,float,float,int,const char**,float,float,float,int)));
#endif
    mWorkerThread.start();
}

void NewWidget::loadConversion(const QString &fileName, QVector<Conversion> &conversionVec)
{
    conversionVec.clear();
    QFile file(fileName);
    if(!file.open(QIODevice::ReadOnly))
    {
        QMessageBox::warning(this,tr("\351\224\231\350\257\257"),tr("\346\211\223\345\274\200%1\345\244\261\350\264\245").arg(fileName));
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
        QMessageBox::warning(this,tr("\351\224\231\350\257\257"),tr("\346\211\223\345\274\200%1\345\244\261\350\264\245").arg(fileName));
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

void NewWidget::saveParamsToJson()
{
    QJsonObject params;
    //! input_power is error . should be input freq
    params.insert("input_freq", ui->m_pDSpinBoxInputFreq->value());
    params.insert("input_power",ui->m_pSpinBoxInputPower->value());
    params.insert("DDS_1", ui->m_pDSpinBoxDDS1Freq->value());
    params.insert("DDS_2", ui->m_pDSpinBoxDDS2Freq->value());
    params.insert("output_freq",ui->m_pDSpinBoxOutputFreq->value());
    params.insert("output_power", ui->m_pDSpinBoxOutputPower->value());
    params.insert("IP", ui->m_pLineEditIP->text());

    QJsonDocument document;

    document.setObject(params);
    QByteArray bytes = document.toJson();

    QFile file(QApplication::applicationDirPath()+ "//config.json");
    qDebug()<<QApplication::applicationDirPath()+ "//config.json";
    if(file.open(QIODevice::WriteOnly))
    {
        file.write(bytes);
        file.close();
        return ;
    }
    qDebug()<<bytes;
}

void NewWidget::restoreParamsFromJson()
{
    QFile file(QApplication::applicationDirPath()+ "//config.json");
    if(!file.open(QIODevice::ReadOnly))
        return;
    QByteArray bytes = file.readAll();
    QJsonDocument document = QJsonDocument::fromJson(bytes);
    if(document.isObject())
    {
        QJsonObject paramas = document.object();
        if(paramas.contains("input_freq"))
        {
            ui->m_pDSpinBoxInputFreq->setValue(paramas.take("input_freq").toDouble());
        }
        if(paramas.contains("input_power"))
        {
            ui->m_pSpinBoxInputPower->setValue(paramas.take("input_power").toDouble());
        }
        if(paramas.contains("DDS_1"))
        {
            ui->m_pDSpinBoxDDS1Freq->setValue(paramas.take("DDS_1").toDouble());
        }
        if(paramas.contains("DDS_2"))
        {
            ui->m_pDSpinBoxDDS2Freq->setValue(paramas.take("DDS_2").toDouble());
        }
        if(paramas.contains("output_freq"))
        {
            ui->m_pDSpinBoxOutputFreq->setValue(paramas.take("output_freq").toDouble());
        }
        if(paramas.contains("output_power"))
        {
            ui->m_pDSpinBoxOutputPower->setValue(paramas.take("output_power").toDouble());
        }
        if(paramas.contains("IP"))
        {
            ui->m_pLineEditIP->setText(paramas.take("IP").toString());
        }
    }
}

void NewWidget::initComboBox()
{
    QString exepath = QApplication::applicationDirPath();
    qDebug()<<exepath;
    QDir dir(exepath+"\\data");
    QStringList list = dir.entryList();
    list.removeAt(0);
    list.removeAt(0);
    foreach(QString filename , list)
    {
        ui->m_pComBoxDataList->addItem(filename);
    }
}

void NewWidget::loadData(QString datapath)
{
    loadConversion(datapath+"/down_conversion.txt", mDownConversionVec);
    loadConversion(datapath+"/search_down_conversion.txt", mSearchDownConversionVec);
    loadUPConversion(datapath+"/up_conversion.txt", mUpConversionVec);
    emit sig_setpath(datapath);
}

void NewWidget::on_m_pDSpinBoxInputFreq_valueChanged(double arg1)
{
    if(arg1<10010000000)
    {
        ui->m_pLineEdit300_400->setText("300");
    }
    else
    {
        ui->m_pLineEdit300_400->setText("400");
    }
    if(!ui->m_pCBLinkage->isChecked())
    {
        DROValue(true);
    }
    else
    {
        DDS1Value();
        DROValue(false);
    }
    outputFreqValue();
}

void NewWidget::on_m_pCBLinkage_toggled(bool checked)
{
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
    outputFreqValue();
}

void NewWidget::on_m_pBtnHelp_clicked()
{
    QString str = QApplication::applicationDirPath()+"\\help.pdf";
    QStringList strList;
    strList.append(str);
    QProcess *pdfProcess = new QProcess(this);
    pdfProcess->start(QApplication::applicationDirPath()+"\\mupdf.exe", strList);
}

void NewWidget::on_m_pBtnConnWifi_clicked()
{
    QString ipAddr = ui->m_pLineEditIP->text();
    QHostAddress addr;
    if(addr.setAddress(ipAddr))
    {
        emit connectToServer(ipAddr);
    }
    else
    {
        QMessageBox::warning(this,tr("\351\224\231\350\257\257"),tr("\350\257\267\346\243\200\346\237\245IP\345\234\260\345\235\200\346\240\274\345\274\217!"));
    }
}

void NewWidget::on_m_pBtnPhaseLock_clicked()
{
    this->on_m_pBtnSendMsg_clicked();
    requireVec.append(eLockPhase);
    ui->m_pBtnPhaseLock->setText(tr("\346\215\225\350\216\267\344\270\255..."));
    ui->m_pBtnPhaseLock->setEnabled(false);
    ui->m_pBtnSendMsg->setEnabled(false);
    qint64 mDDS1ControlWord = (ui->m_pDSpinBoxDDS1Freq->value() / 400000000.0) * (((long long)1) << 48);

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
    qDebug()<<"lock:"<<bytes;

#ifdef ZMQ_DLL
    emit sendMessage(bytes);
#else
    emit sig_phaseLock(ui->m_pDSpinBoxDDS1Freq->value(),ui->m_pDSpinBoxOutputPower->value());
#endif
    this->on_m_pBtnSendMsg_clicked();
}

void NewWidget::on_m_pBtnSendMsg_clicked()
{
    requireVec.append(eMsgSend);
    ui->m_pBtnPhaseLock->setEnabled(false);
    ui->m_pBtnSendMsg->setEnabled(false);
    qint64 mDDS1ControlWord = (ui->m_pDSpinBoxDDS1Freq->value() / 400000000.0) * (((long long)1) << 48);
    qint64 mDDS2ControlWord = (ui->m_pDSpinBoxDDS2Freq->value() / 300000000.0) * (((long long)1) << 48);
    qint32 mPhase =((ui->m_pSpinBoxDDS2Phase->value())*16384)/2/3.1415;
    int inputIndex = findAttenuation(ui->m_pSpinBoxInputPower->value(), mDownConversionVec);
    int outputIndex = findUPAttenuation(ui->m_pDSpinBoxOutputPower->value(), mUpConversionVec);
    bool mFilterWord = ui->m_pLineEdit300_400->text().toInt()==400?true:false;

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
    sendObject.insert("dds2_phase", QString::number(mPhase));
    QJsonDocument document;
    document.setObject(sendObject);
    QByteArray bytes = document.toJson();
    qDebug()<<"set:"<<bytes;
#ifdef ZMQ_DLL
    emit sendMessage(bytes);
#else
    emit sig_setParam(ui->m_pDSpinBoxDDS1Freq->value(),ui->m_pDSpinBoxDDS2Freq->value(),
                                    ui->m_pSpinBoxDDS2Phase->value(),ui->m_pSpinBoxInputPower->value(),
                                ui->m_pDSpinBoxOutputPower->value(),0);
#endif

}

void NewWidget::on_m_pDSpinBoxDDS2Freq_valueChanged(double arg1)
{
    outputFreqValue();
}

void NewWidget::on_m_pComBoxDataList_currentTextChanged(const QString &arg1)
{
    loadData(QApplication::applicationDirPath()+"/data/"+ui->m_pComBoxDataList->currentText());
}

void NewWidget::on_m_pDSpinBoxDDS1Freq_valueChanged(double arg1)
{
    DROValue(!ui->m_pCBLinkage->isChecked());
    outputFreqValue();
}

void NewWidget::on_m_pBtnStartScan_clicked()
{
    requireVec.append(eMsgSend);
    QJsonObject sendObject;
    sendObject.insert("freq_start",QString::number(ui->m_pDSBStartFrenq->value()));
    sendObject.insert("freq_step", QString::number(ui->m_pDSBBujinFrenq->value()));
    sendObject.insert("freq_stop", QString::number(ui->m_pDSBEndFrenq->value()));
    sendObject.insert("freq_enable", QString::number(ui->m_pCBScanFreq->isChecked()));

    QFile file(QApplication::applicationDirPath()+"\\data\\"+ui->m_pComBoxDataList->currentText()+"\\up_conversion.txt");//路径需要修改成实际使用路径
    if(!file.open(QIODevice::ReadOnly))
    {
        QMessageBox::warning(this,tr("\351\224\231\350\257\257"),tr("\346\211\223\345\274\200%1\345\244\261\350\264\245"));
        return;
    }
    QByteArray m_bytes = file.readAll();
    QString m_data(m_bytes);
    file.close();

    sendObject.insert("power_table", m_data);
    sendObject.insert("power_start",QString::number(ui->m_pDSBStartPower->value()));//功率实际为-dBm
    sendObject.insert("power_step",QString::number(ui->m_pDSBBujinpower->value()));//功率实际为-dBm

    sendObject.insert("power_stop",QString::number(ui->m_pDSBEndPower->value()));//功率实际为-dBm
    sendObject.insert("up_power_enable", QString::number(ui->m_pCBScanpower->isChecked()));

    QJsonDocument document;
    document.setObject(sendObject);
    QByteArray bytes = document.toJson();
    qDebug()<<"on_m_pBtnStartScan_clicked:"<<bytes;
#ifdef ZMQ_DLL
    emit sendMessage(bytes);
#else
    const char *filepath =(const char *)malloc(QFileInfo(file).absoluteFilePath().toLocal8Bit().size()+1);
    memset((void*)filepath,0,QFileInfo(file).absoluteFilePath().toLocal8Bit().size()+1);
    memcpy((void*)filepath,QFileInfo(file).absoluteFilePath().toLocal8Bit().data(),QFileInfo(file).absoluteFilePath().toLocal8Bit().size()+1);
    emit sig_startScan(ui->m_pDSBStartFrenq->value(),
                                ui->m_pDSBBujinFrenq->value(),
                                ui->m_pDSBEndFrenq->value(),
                                ui->m_pCBScanFreq->isChecked(),
                                &filepath
                                ,ui->m_pDSBStartPower->value(),
                                    ui->m_pDSBBujinpower->value(),
                                    ui->m_pDSBEndPower->value(),
                                    ui->m_pCBScanpower->isChecked());

#endif

}
