#ifndef TEMPGAUGEWIDGET_H
#define TEMPGAUGEWIDGET_H

#include <QWidget>

class TempGaugeWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TempGaugeWidget(QWidget *parent = nullptr);

public slots:
    void setTemperature(double temperature);

signals:
    void highTemperatureStateChanged(bool active);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    double m_temperature;
};

#endif