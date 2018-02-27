#ifndef NEWWIDGET_H
#define NEWWIDGET_H

#include <QWidget>
#include <QThread>

namespace Ui {
class NewWidget;
}

struct Conversion
{
    float value;
    int RF;
    int LF;
};

struct UPConversion
{
    float value;
    int RF;
    int LF_ONE;
    int LF_TWO;
};

class NewWidget : public QWidget
{
    Q_OBJECT

public:
    explicit NewWidget(QWidget *parent = 0);
    ~NewWidget();

public slots:
    void onConnected();
    void onSendFinished();

signals:
    void connectToServer(QString ipAddr);
    void sendMessage(QByteArray bytes);


protected:
    virtual void paintEvent(QPaintEvent *event)override;

private slots:
    void on_m_pDSpinBoxInputFreq_valueChanged(double arg1);

    void on_m_pCBLinkage_toggled(bool checked);

    void on_m_pBtnHelp_clicked();

    void on_m_pBtnConnWifi_clicked();

    void on_m_pBtnPhaseLock_clicked();

    void on_m_pBtnSendMsg_clicked();

private:
    void initWidget();
    void initControl(bool);
    void outputFreqValue();
    void DDS1Value();
    void DROValue(bool autoCalc);
    void changeNetworkStatus(bool status);
    void initNetworkTask();

    void loadConversion(const QString &fileName, QVector<Conversion> &conversionVec);
    void loadUPConversion(const QString &fileName, QVector<UPConversion> &conversionVec);
    int findAttenuation(double powerValue, const QVector<Conversion> &conversionVec);
    int findUPAttenuation(double powerValue, const QVector<UPConversion> &conversionVec);

private:
    Ui::NewWidget *ui;
    QVector<Conversion> mDownConversionVec;
    QVector<Conversion> mSearchDownConversionVec;
    QVector<UPConversion> mUpConversionVec;
    QThread mWorkerThread;
};

#endif // NEWWIDGET_H
