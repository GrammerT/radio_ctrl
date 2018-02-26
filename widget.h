#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QThread>
#include <QVector>

class QLabel;
class QSpinBox;
class QComboBox;
class QLineEdit;
class QDoubleSpinBox;
class QCheckBox;

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = 0);
    ~Widget();

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

public slots:
    void onConnected();
    void onSendFinished();

signals:
    void connectToServer(QString ipAddr);
    void sendMessage(QByteArray bytes);

protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void onSendBtnClicked();
    void onConnectBtnClicked();
    void onSearchBtnClicked();
    void onGenBtnClicked();
    void onHMBoxClicked(bool checked);
    void onInputEditFinished();

private:
    //! change label status
    void changeNetworkStatus(bool status);
    void switchInputMode(bool mode);

    void saveParamsToJson();
    void restoreParamsFromJson();
    void sendControlCommand();
    void sendSearchCommand();
    void initWorkers();
    void loadConversion(const QString &fileName, QVector<Conversion> &conversionVec);
    void loadUPConversion(const QString &fileName, QVector<UPConversion> &conversionVec);
    int findAttenuation(double powerValue, const QVector<Conversion> &conversionVec);
    int findUPAttenuation(double powerValue, const QVector<UPConversion> &conversionVec);

    //! calculator
    void calculateLOHz();
    void calculateDDS1();
    void calculateDDS1Word();
    void calculateDDS1Para();
    void calculateDDS2();
    void calculateDDS2Word();
    void calculateDDS2Para();

    QLabel *m_pInputLabel;
    QDoubleSpinBox *m_pInputSpinBox;
    QLabel *m_pHMLabel;
    QCheckBox *m_pHMCheckBox;

    QLabel *m_pDDS1Label;
    QDoubleSpinBox *m_pDDS1SpinBox;
    QLabel *m_pDDS2Label;
    QDoubleSpinBox *m_pDDS2SpinBox;

    QLabel *m_pPowerLabel;
    QDoubleSpinBox *m_pPowerSpinBox;

    QLabel *m_pOutputLabel;
    QDoubleSpinBox *m_pOutputSpinBox;
    QLabel *m_pIPLabel;
    QLineEdit *m_pIPEdit;

    QLabel *m_pStatusLabel;

    double mLOHz;
    double mDDS1Value;
    qint64 mDDS1ControlWord;
    double mDDS2Value;
    qint64 mDDS2ControlWord;
    bool mFilterWord;
    bool mLOWord;

    QVector<Conversion> mDownConversionVec;
    QVector<Conversion> mSearchDownConversionVec;
    QVector<UPConversion> mUpConversionVec;

    QThread mWorkerThread;
};

#endif // WIDGET_H
