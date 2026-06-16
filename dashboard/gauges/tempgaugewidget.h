#ifndef TEMPGAUGEWIDGET_H
#define TEMPGAUGEWIDGET_H

#include <QWidget>

class QPainter;

class TempGaugeWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TempGaugeWidget(QWidget *parent = NULL);

public slots:
    void setTemperature(double temperature);

signals:
    void highTemperatureStateChanged(bool active);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    double valueToAngle(double value) const;
    void drawGauge(QPainter &painter, int radius);

    double m_temperature;
    bool m_highTemperatureActive;
};

#endif // TEMPGAUGEWIDGET_H
