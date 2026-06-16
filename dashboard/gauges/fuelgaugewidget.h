#ifndef FUELGAUGEWIDGET_H
#define FUELGAUGEWIDGET_H

#include <QWidget>

class QPainter;

class FuelGaugeWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FuelGaugeWidget(QWidget *parent = NULL);

public slots:
    void setFuelValue(double fuel);

signals:
    void lowFuelStateChanged(bool active);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    double valueToAngle(double value) const;
    void drawGauge(QPainter &painter, int radius);

    double m_fuel;
    bool m_lowFuelActive;
};

#endif // FUELGAUGEWIDGET_H
