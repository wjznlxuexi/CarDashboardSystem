#include "signalgenerator.h"
#include <cstdlib>   // for rand()
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

SignalGenerator::SignalGenerator(QObject *parent)
    : QObject(parent), m_timer(new QTimer(this)), m_step(0)
{
    // 设置定时器周期为100ms
    m_timer->setInterval(100);
    connect(m_timer, &QTimer::timeout, this, &SignalGenerator::generateData);

    // 初始值
    reset();
}

void SignalGenerator::start()
{
    m_timer->start();
}

void SignalGenerator::stop()
{
    m_timer->stop();
}

void SignalGenerator::reset()
{
    m_step = 0;
    m_currentData = VehicleData();          // 恢复所有默认值
    m_currentData.speed = 60.0;
    m_currentData.rpm = 2400;
    m_currentData.fuel = 60.0;
    m_currentData.temperature = 90.0;
    // 故障标志全部清除（默认构造函数已经清除了）
}

void SignalGenerator::generateData()
{
    // 1. 速度：正弦波 60 ± 80 km/h，周期10秒
    //    t = step * 0.1 秒
    double t = m_step * 0.1;
    double speed = 60.0 + 80.0 * sin(2.0 * M_PI * t / 10.0);
    if (speed < 0.0) speed = 0.0;
    if (speed > 140.0) speed = 140.0;
    m_currentData.speed = speed;

    // 2. 转速 = 速度 × 40
    m_currentData.rpm = static_cast<int>(speed * 40);

    // 3. 油量：每100ms减少0.001L，不低于0
    m_currentData.fuel -= 0.001;
    if (m_currentData.fuel < 0.0) m_currentData.fuel = 0.0;

    // 4. 水温：随机游走（每步 -2.5 ~ +2.5 °C），限制在 80~100
    double delta = (rand() % 11 - 5) / 2.0;   // -2.5, -2.0, ..., +2.5
    double newTemp = m_currentData.temperature + delta;
    if (newTemp < 80.0) newTemp = 80.0;
    if (newTemp > 100.0) newTemp = 100.0;
    m_currentData.temperature = newTemp;

    // 5. 超速标志：速度 > 120 时触发（注意：speedFault 是模拟器自动计算的）
    m_currentData.speedFault = (speed > 120.0);

    // 注意：absFault, airbagFault, batteryFault, brakeFault 由 SimulatorServer 中的故障生成器负责，
    // SignalGenerator 不生成随机故障，只生成基础信号。
    // 但是为了保证数据结构完整，我们保留其原有值（reset时已清零），
    // 后续 SimulatorServer 会覆盖这些故障字段。

    // 6. 转向灯、双闪：这些由外部指令控制，SignalGenerator 不应修改，
    //    所以保留上一次的值（m_currentData 中 turnSignal, hazard 保持不变）。

    m_step++;

    // 发出信号
    emit vehicleDataGenerated(m_currentData);
}
