#ifndef SIGNALGENERATOR_H
#define SIGNALGENERATOR_H

#include <QObject>
#include <QTimer>
#include <cmath>
#include "common/vehicledata.h"

class SignalGenerator : public QObject
{
    Q_OBJECT
public:
    explicit SignalGenerator(QObject *parent = nullptr);

    void start();           // 开始生成数据
    void stop();            // 停止定时器
    void reset();           // 重置所有状态到初始值

signals:
    void vehicleDataGenerated(const VehicleData &data);   // 每100ms发出一次

private slots:
    void generateData();    // 定时器槽函数

private:
    QTimer *m_timer;
    VehicleData m_currentData;

    // 用于正弦波和随机游走的辅助变量
    int m_step;             // 步数计数，用于速度正弦波
};

#endif // SIGNALGENERATOR_H
