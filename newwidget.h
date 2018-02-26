#ifndef NEWWIDGET_H
#define NEWWIDGET_H

#include <QWidget>

namespace Ui {
class NewWidget;
}

class NewWidget : public QWidget
{
    Q_OBJECT

public:
    explicit NewWidget(QWidget *parent = 0);
    ~NewWidget();
protected:
    virtual void paintEvent(QPaintEvent *event)override;

private slots:
    void on_m_pDSpinBoxInputFreq_valueChanged(double arg1);

    void on_m_pCBLinkage_toggled(bool checked);

private:
    void initWidget();
    void initControl(bool);
private:
    Ui::NewWidget *ui;
};

#endif // NEWWIDGET_H
