#ifndef FUELGAUGEWIDGET_H
#define FUELGAUGEWIDGET_H

#include <QWidget>

class FuelGaugeWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FuelGaugeWidget(QWidget *parent = nullptr);

public slots:
    void setFuelValue(double fuel);

signals:
    void lowFuelStateChanged(bool active);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    double m_fuel;
};

#endif