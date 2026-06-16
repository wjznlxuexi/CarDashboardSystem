#ifndef SIGNALGENERATOR_H
#define SIGNALGENERATOR_H

#include <QObject>
#include <QTimer>

#include "common/vehicledata.h"

class SignalGenerator : public QObject
{
    Q_OBJECT

public:
    explicit SignalGenerator(QObject *parent = NULL);

    void start();
    void stop();
    void reset();
    void setTurnSignal(int signal);
    void setHazardEnabled(bool enabled);
    void setManualMode(bool enabled);
    void setManualSpeed(double speed);
    void setManualFuel(double fuel);
    void setManualTemperature(double temperature);

signals:
    void vehicleDataGenerated(const VehicleData &data);

private slots:
    void generateData();

private:
    QTimer *m_timer;
    VehicleData m_currentData;
    int m_step;
    bool m_manualMode;
    double m_manualSpeed;
    double m_manualFuel;
    double m_manualTemperature;
};

#endif // SIGNALGENERATOR_H
