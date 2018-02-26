#include "widget.h"
#include <QFormLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QSpinBox>
#include <QComboBox>
#include <QPushButton>
#include <QLineEdit>
#include <QGroupBox>
#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QDebug>
#include <QFile>
#include <QJsonDocument>
#include <cmath>
#include <QJsonObject>
#include <QStringList>
#include "NetworkTaskWorker.h"
#include "newwidget.h"

static const QString CONFIG_FILE = "config.json";
static const double VCDRO = 9600.0;

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , mWorkerThread()
    , mLOHz(0.0)
    , mDDS1Value(0.0)
    , mDDS1ControlWord(0)
    , mDDS2Value(0.0)
    , mDDS2ControlWord(0)
    , mFilterWord(false)
    , mLOWord(false)
    , mDownConversionVec()
    , mUpConversionVec()
{
    QHBoxLayout *pInputLayout = new QHBoxLayout;

    m_pInputLabel = new QLabel;
    m_pInputLabel->setObjectName("label");
    m_pInputLabel->setFixedSize(60, 25);
    m_pInputLabel->setText(QString::fromLocal8Bit("输入频率:"));

    m_pInputSpinBox = new QDoubleSpinBox;
    m_pInputSpinBox->setFixedSize(150, 25);
    m_pInputSpinBox->setObjectName("spinbox");
    m_pInputSpinBox->setSingleStep(0.1);
    m_pInputSpinBox->setRange(9600.0, 10400.0);
    m_pInputSpinBox->setValue(10010.0);
    m_pInputSpinBox->setSuffix("MHz");
    m_pInputSpinBox->setDecimals(6);

    m_pHMCheckBox = new QCheckBox;

    m_pHMCheckBox->setText(QString::fromLocal8Bit("手动"));
    m_pHMCheckBox->setObjectName("checkbox");

    pInputLayout->addWidget(m_pInputLabel);
    pInputLayout->addWidget(m_pInputSpinBox);

    m_pDDS1Label = new QLabel;
    m_pDDS1Label->setObjectName("label");
    m_pDDS1Label->setFixedSize(60, 25);
    m_pDDS1Label->setText("DDS1:");

    m_pDDS1SpinBox = new QDoubleSpinBox;
    m_pDDS1SpinBox->setObjectName("spinbox");
    m_pDDS1SpinBox->setFixedSize(150, 25);
    m_pDDS1SpinBox->setSingleStep(0.1);
    m_pDDS1SpinBox->setRange(0.0, 800.0);
    m_pDDS1SpinBox->setSuffix("MHz");
    m_pDDS1SpinBox->setDecimals(12);

    QHBoxLayout *pDDS1Layout = new QHBoxLayout;
    pDDS1Layout->addWidget(m_pDDS1Label);
    pDDS1Layout->addWidget(m_pDDS1SpinBox);

    m_pDDS2Label = new QLabel;
    m_pDDS2Label->setObjectName("label");
    m_pDDS2Label->setFixedSize(60, 25);
    m_pDDS2Label->setText("DDS2:");

    m_pDDS2SpinBox = new QDoubleSpinBox;
    m_pDDS2SpinBox->setObjectName("spinbox");
    m_pDDS2SpinBox->setFixedSize(150, 25);
    m_pDDS2SpinBox->setSingleStep(0.1);
    m_pDDS2SpinBox->setRange(0.0, 800.0);
    m_pDDS2SpinBox->setSuffix("MHz");
    m_pDDS2SpinBox->setDecimals(12);

    QHBoxLayout *pDDS2Layout = new QHBoxLayout;
    pDDS2Layout->addWidget(m_pDDS2Label);
    pDDS2Layout->addWidget(m_pDDS2SpinBox);

    QHBoxLayout *pInputPowerLayout = new QHBoxLayout;

    m_pPowerLabel = new QLabel;
    m_pPowerLabel->setObjectName("label");
    m_pPowerLabel->setFixedSize(60, 25);
    m_pPowerLabel->setText(QString::fromLocal8Bit("输入功率:"));

    m_pPowerSpinBox = new QDoubleSpinBox;
    m_pPowerSpinBox->setObjectName("spinbox");
    m_pPowerSpinBox->setFixedSize(100, 25);
    m_pPowerSpinBox->setSingleStep( 0.1);
    m_pPowerSpinBox->setRange(-40.0, 0.0);
    m_pPowerSpinBox->setValue(-30.0);
    m_pPowerSpinBox->setSuffix("dBm");
    m_pPowerSpinBox->setDecimals(1);

    pInputPowerLayout->addWidget(m_pPowerLabel);
    pInputPowerLayout->addWidget(m_pPowerSpinBox);

    QHBoxLayout *pOutputPowerLayout = new QHBoxLayout;

    m_pOutputLabel = new QLabel;
    m_pOutputLabel->setObjectName("label");
    m_pOutputLabel->setFixedSize(60, 25);
    m_pOutputLabel->setText(QString::fromLocal8Bit("输出功率:"));

    m_pOutputSpinBox = new QDoubleSpinBox;
    m_pOutputSpinBox->setObjectName("spinbox");
    m_pOutputSpinBox->setFixedSize(150, 25);
    m_pOutputSpinBox->setSingleStep(0.1);
    m_pOutputSpinBox->setRange(-90.0, 0.0);
    m_pOutputSpinBox->setValue(-40.0);
    m_pOutputSpinBox->setSuffix("dBm");
    m_pOutputSpinBox->setDecimals(1);

    pOutputPowerLayout->addWidget(m_pOutputLabel);
    pOutputPowerLayout->addWidget(m_pOutputSpinBox);

    QPushButton *pGenBtn = new QPushButton;
    pGenBtn->setObjectName("button");
    pGenBtn->setText("generate");
    pGenBtn->setFixedSize(75, 25);

    QGridLayout *parasLayout = new QGridLayout;
    parasLayout->setVerticalSpacing(20);
    parasLayout->setMargin(20);

    parasLayout->addLayout(pInputLayout, 1, 1, 1, 1, Qt::AlignLeft);
    parasLayout->addWidget(m_pHMCheckBox, 1, 2, 1, 1, Qt::AlignLeft);
    parasLayout->addLayout(pDDS1Layout, 2, 1, 1, 1, Qt::AlignLeft);
    parasLayout->addLayout(pDDS2Layout, 2, 2, 1, 1, Qt::AlignLeft);
    parasLayout->addLayout(pInputPowerLayout, 3, 1, 1, 1, Qt::AlignLeft);
    parasLayout->addLayout(pOutputPowerLayout, 3, 2, 1, 1, Qt::AlignLeft);

    QGroupBox *pParasBox = new QGroupBox;

    pParasBox->setTitle(QString::fromLocal8Bit("参数配置"));
    pParasBox->setLayout(parasLayout);

    m_pIPLabel = new QLabel;
    m_pIPLabel->setObjectName("label");
    m_pIPLabel->setFixedSize(30, 25);
    m_pIPLabel->setText(QString::fromLocal8Bit("IP:"));

    m_pIPEdit = new QLineEdit;
    m_pIPEdit->setObjectName("lineEdit");
    m_pIPEdit->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    m_pIPEdit->setFixedSize(120, 25);

    QRegExp regExp("^(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$");
    QRegExpValidator *regExpValidator = new QRegExpValidator(regExp);
    m_pIPEdit->setValidator(regExpValidator);

    QHBoxLayout *pIpLayout = new QHBoxLayout;

    pIpLayout->addWidget(m_pIPLabel);

    pIpLayout->addWidget(m_pIPEdit);

    QPushButton *pConnectBtn = new QPushButton;
    pConnectBtn->setObjectName("button");
    pConnectBtn->setText(QString::fromLocal8Bit("连接"));
    pConnectBtn->setFixedSize(75, 25);

    QLabel *pShowLabel = new QLabel;
    pShowLabel->setObjectName("label");
    pShowLabel->setFixedSize(60, 25);
    pShowLabel->setText(QString::fromLocal8Bit("连接状态:"));

    m_pStatusLabel = new QLabel;
    m_pStatusLabel->setObjectName("connectLabel");
    m_pStatusLabel->setFixedSize(60, 25);

    QHBoxLayout *pConnectLayout = new QHBoxLayout;
    pConnectLayout->setMargin(5);

    pConnectLayout->addStretch(1);
    pConnectLayout->addLayout(pIpLayout);
    pConnectLayout->addSpacing(20);
    pConnectLayout->addWidget(pConnectBtn);
    pConnectLayout->addStretch(1);

    QHBoxLayout *pStatusLayout = new QHBoxLayout;
    pStatusLayout->setMargin(5);

    pStatusLayout->addStretch(1);
    pStatusLayout->addWidget(pShowLabel);
    pStatusLayout->addWidget(m_pStatusLabel);
    pStatusLayout->addStretch(1);

    QVBoxLayout *pNetworkLayout = new QVBoxLayout;

    pNetworkLayout->addLayout(pConnectLayout);
    pNetworkLayout->addLayout(pStatusLayout);

    QGroupBox *pNetWorkBox = new QGroupBox;

    pNetWorkBox->setTitle(QString::fromLocal8Bit("网络配置"));
    pNetWorkBox->setLayout(pNetworkLayout);

    QHBoxLayout *pBottomLayout = new QHBoxLayout;

    QPushButton *pSearchBtn = new QPushButton;
    pSearchBtn->setObjectName("button");
    pSearchBtn->setText(QString::fromLocal8Bit("搜索"));
    pSearchBtn->setFixedSize(75, 25);

    QPushButton *pSendBtn = new QPushButton;
    pSendBtn->setObjectName("button");
    pSendBtn->setText(QString::fromLocal8Bit("发送"));
    pSendBtn->setFixedSize(75, 25);

    pBottomLayout->addStretch(1);
    pBottomLayout->addWidget(pSearchBtn);
    pBottomLayout->addSpacing(20);
    pBottomLayout->addWidget(pSendBtn);
    pBottomLayout->addStretch(1);

    QVBoxLayout *pTotalLayout = new QVBoxLayout;
    pTotalLayout->addSpacing(10);
    pTotalLayout->addWidget(pParasBox);
    pTotalLayout->addSpacing(10);
    pTotalLayout->addWidget(pNetWorkBox);
    pTotalLayout->addSpacing(10);
    pTotalLayout->addStretch(1);
    pTotalLayout->addLayout(pBottomLayout);
    pTotalLayout->addSpacing(10);

    setLayout(pTotalLayout);
    setObjectName("widget");

    connect(pGenBtn, &QPushButton::clicked, this, &Widget::onGenBtnClicked);
    connect(pConnectBtn, &QPushButton::clicked, this, &Widget::onConnectBtnClicked);
    connect(pSearchBtn, &QPushButton::clicked, this, &Widget::onSearchBtnClicked);
    connect(pSendBtn, &QPushButton::clicked, this, &Widget::onSendBtnClicked);
    connect(m_pHMCheckBox, &QCheckBox::clicked, this, &Widget::onHMBoxClicked);
    connect(m_pInputSpinBox, SIGNAL(valueChanged(double)), this, SLOT(onInputEditFinished()));

    switchInputMode(false);
    changeNetworkStatus(false);
    restoreParamsFromJson();
    initWorkers();
    loadConversion("down_conversion.txt", mDownConversionVec);
    loadConversion("search_down_conversion.txt", mSearchDownConversionVec);
    loadUPConversion("up_conversion.txt", mUpConversionVec);
    calculateDDS1Para();
    calculateDDS2Para();
}

Widget::~Widget()
{
    mWorkerThread.quit();
//    mWorkerThread.wait();
}

void Widget::onConnected()
{
    changeNetworkStatus(true);
}

void Widget::onSendFinished()
{

}

void Widget::closeEvent(QCloseEvent *event)
{
    saveParamsToJson();
    QWidget::closeEvent(event);
}

void Widget::onSendBtnClicked()
{
    sendControlCommand();
}

void Widget::onConnectBtnClicked()
{
    QString ipAddr = m_pIPEdit->text();
    emit connectToServer(ipAddr);
}

void Widget::onSearchBtnClicked()
{
    sendSearchCommand();
}

void Widget::onGenBtnClicked()
{
    calculateDDS1Para();
}

void Widget::onHMBoxClicked(bool checked)
{
    switchInputMode(checked);
    if (!checked)
    {
        calculateDDS1Para();
        calculateDDS2Para();
    }
}

void Widget::onInputEditFinished()
{
    calculateDDS1Para();
}

void Widget::changeNetworkStatus(bool status)
{
    if (!status)
    {
        m_pStatusLabel->setStyleSheet("QLabel{color:rgb(175, 0, 0)}");
        m_pStatusLabel->setText(QString::fromLocal8Bit("未连接"));
    }
    else
    {
        m_pStatusLabel->setStyleSheet("QLabel{color:rgb(0, 165, 235)}");
        m_pStatusLabel->setText(QString::fromLocal8Bit("已连接"));
    }
}

void Widget::switchInputMode(bool mode)
{
    if (!mode)
    {
        m_pInputSpinBox->setEnabled(true);
        m_pDDS1SpinBox->setEnabled(false);
        m_pDDS2SpinBox->setEnabled(false);
    }
    else
    {
        m_pInputSpinBox->setEnabled(false);
        m_pDDS1SpinBox->setEnabled(true);
        m_pDDS2SpinBox->setEnabled(true);
    }
}

void Widget::saveParamsToJson()
{
    QJsonObject params;

    params.insert("input_power", m_pInputSpinBox->value());
    params.insert("DDS_1", m_pDDS1SpinBox->value());
    params.insert("DDS_2", m_pDDS2SpinBox->value());
    params.insert("output_power", m_pOutputSpinBox->value());
    params.insert("IP", m_pIPEdit->text());

    QJsonDocument document;

    document.setObject(params);
    QByteArray bytes = document.toJson();

    QFile file(CONFIG_FILE);

    file.open(QIODevice::WriteOnly);

    file.write(bytes);

    file.close();
}

void Widget::restoreParamsFromJson()
{
    QFile file(CONFIG_FILE);

    if(!file.open(QIODevice::ReadOnly))
        return;

    QByteArray bytes = file.readAll();

    QJsonDocument document = QJsonDocument::fromJson(bytes);

    if(document.isObject())
    {
        QJsonObject paramas = document.object();

        if(paramas.contains("input_power"))
        {
            m_pInputSpinBox->setValue(paramas.take("input_power").toDouble());
        }

        if(paramas.contains("DDS_1"))
        {
            m_pDDS1SpinBox->setValue(paramas.take("DDS_1").toDouble());
        }

        if(paramas.contains("DDS_2"))
        {
            m_pDDS2SpinBox->setValue(paramas.take("DDS_2").toDouble());
        }

        if(paramas.contains("output_power"))
        {
            m_pOutputSpinBox->setValue(paramas.take("output_power").toDouble());
        }

        if(paramas.contains("IP"))
        {
            m_pIPEdit->setText(paramas.take("IP").toString());
        }

    }
}

void Widget::sendControlCommand()
{
    calculateDDS1Word();
    calculateDDS2Word();
    //! find power attenuation
    int inputIndex = findAttenuation(m_pPowerSpinBox->value(), mDownConversionVec);
    int outputIndex = findUPAttenuation(m_pOutputSpinBox->value(), mUpConversionVec);

    QJsonObject sendObject;

    sendObject.insert("type", "normal");
    sendObject.insert("filter_word", mFilterWord);
    sendObject.insert("lo", mLOHz);
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

void Widget::sendSearchCommand()
{
    calculateDDS1Word();
    int inputIndex = findAttenuation(m_pPowerSpinBox->value(), mSearchDownConversionVec);

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

void Widget::initWorkers()
{
    NetworkTaskWorker *worker = new NetworkTaskWorker;

    worker->moveToThread(&mWorkerThread);
    connect(&mWorkerThread, &QThread::finished, worker, &QObject::deleteLater);
    connect(this, &Widget::connectToServer, worker, &NetworkTaskWorker::onConnectToServer);
    connect(this, &Widget::sendMessage, worker, &NetworkTaskWorker::onSendMessage);
    connect(worker, &NetworkTaskWorker::connected, this, &Widget::onConnected);
    connect(worker, &NetworkTaskWorker::sendFinished, this, &Widget::onSendFinished);
    mWorkerThread.start();
}

void Widget::loadConversion(const QString &fileName, QVector<Conversion> &conversionVec)
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

void Widget::loadUPConversion(const QString &fileName, QVector<Widget::UPConversion> &conversionVec)
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

int Widget::findAttenuation(double powerValue, const QVector<Conversion> &conversionVec)
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

int Widget::findUPAttenuation(double powerValue, const QVector<Widget::UPConversion> &conversionVec)
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

void Widget::calculateLOHz()
{
    //! unit MHz
    double inputFrequency = m_pInputSpinBox->value();

    if (inputFrequency > 10010.0)
    {
        mLOHz = 400.0;
        mFilterWord = true;
        mLOWord = false;
    }
    else if (inputFrequency <= 10010.0)
    {
        mLOHz = 300.0;
        mFilterWord = false;
        mLOWord = true;
    }
}

void Widget::calculateDDS1()
{
    //! auto generate
    double inputFrequency = m_pInputSpinBox->value();
    mDDS1Value = inputFrequency - VCDRO - mLOHz;
    m_pDDS1SpinBox->setValue(mDDS1Value);
}

void Widget::calculateDDS1Word()
{
    mDDS1ControlWord = (m_pDDS1SpinBox->value() / 400.0) * (((long long)1) << 48);
}

void Widget::calculateDDS1Para()
{
    calculateLOHz();
    calculateDDS1();
}

void Widget::calculateDDS2()
{
    //! unit MHz
    mDDS2Value = VCDRO - 9192.631770 - 300;
    m_pDDS2SpinBox->setValue(mDDS2Value);
}

void Widget::calculateDDS2Word()
{
    mDDS2ControlWord = (m_pDDS2SpinBox->value() / 300.0) * (((long long)1) << 48);
}

void Widget::calculateDDS2Para()
{
    calculateDDS2();
}
