#include "signalgenerator.h"

#include <QtGlobal>
#include <cmath>
#include <cstdlib>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

SignalGenerator::SignalGenerator(QObject *parent)
    : QObject(parent),
      m_timer(new QTimer(this)),
      m_step(0),
      m_manualMode(false),
      m_manualSpeed(0.0),
      m_manualFuel(60.0),
      m_manualTemperature(80.0)
{
    m_timer->setInterval(100);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(generateData()));
    reset();
}

void SignalGenerator::start()
{
    if(!m_timer->isActive())
        m_timer->start();
}

void SignalGenerator::stop()
{
    m_timer->stop();
}

void SignalGenerator::reset()
{
    m_step = 0;
    m_currentData = VehicleData();
    m_currentData.fuel = 60.0;
    m_currentData.temperature = 80.0;
    m_manualSpeed = m_currentData.speed;
    m_manualFuel = m_currentData.fuel;
    m_manualTemperature = m_currentData.temperature;
}

void SignalGenerator::setTurnSignal(int signal)
{
    m_currentData.turnSignal = qBound(0, signal, 2);
    if(m_currentData.turnSignal != 0)
        m_currentData.hazard = false;
}

void SignalGenerator::setHazardEnabled(bool enabled)
{
    m_currentData.hazard = enabled;
    if(enabled)
        m_currentData.turnSignal = 0;
}

void SignalGenerator::setManualMode(bool enabled)
{
    m_manualMode = enabled;
    if(enabled)
    {
        m_manualSpeed = m_currentData.speed;
        m_manualFuel = m_currentData.fuel;
        m_manualTemperature = m_currentData.temperature;
    }
}

void SignalGenerator::setManualSpeed(double speed)
{
    m_manualSpeed = qBound(0.0, speed, 140.0);
}

void SignalGenerator::setManualFuel(double fuel)
{
    m_manualFuel = qBound(0.0, fuel, 60.0);
}

void SignalGenerator::setManualTemperature(double temperature)
{
    m_manualTemperature = qBound(80.0, temperature, 100.0);
}

void SignalGenerator::generateData()
{
    if(m_manualMode)
    {
        m_currentData.speed = m_manualSpeed;
        m_currentData.fuel = m_manualFuel;
        m_currentData.temperature = m_manualTemperature;
    }
    else
    {
        const double t = m_step * 0.1;
        const double wave = std::sin(2.0 * M_PI * t / 12.0);
        m_currentData.speed = qBound(0.0, 70.0 + 70.0 * wave, 140.0);

        m_currentData.fuel = qMax(0.0, m_currentData.fuel - 0.003);

        const double delta = (std::rand() % 11 - 5) * 0.25;
        m_currentData.temperature = qBound(80.0, m_currentData.temperature + delta, 100.0);
    }

    m_currentData.rpm = static_cast<int>(m_currentData.speed * 40.0);
    m_currentData.speedFault = m_currentData.speed > 120.0;

    ++m_step;
    emit vehicleDataGenerated(m_currentData);
}
