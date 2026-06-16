#ifndef SIMULATORSERVER_H
#define SIMULATORSERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QTimer>
#include <QSet>
#include "common/vehicledata.h"

class SimulatorServer : public QObject
{
    Q_OBJECT
public:
    explicit SimulatorServer(QObject *parent = nullptr);

    bool startServer(quint16 port);
    void stopServer();

public slots:
    void broadcastVehicleData(const VehicleData &data);   // 接收来自 SignalGenerator 的数据，添加随机故障后广播
    void handleCommand(const QString &command);           // 处理控制指令（可由外部调用）

signals:
    void clientCountChanged(int count);
    void commandReceived(const QString &command);         // 可选，用于调试

private slots:
    void onNewConnection();
    void onClientDisconnected();
    void onReadyRead();
    void updateRandomFaults();     // 每秒更新一次随机故障状态

private:
    void broadcastJson(const QByteArray &json);
    void processClientCommand(const QString &cmd, QTcpSocket *client); // 处理单个客户端指令
    void updateVehicleDataWithFaults(VehicleData &data);   // 将当前随机故障状态填入 data

    QTcpServer *m_server;
    QSet<QTcpSocket*> m_clients;

    // 随机故障状态和持续时间（单位：秒）
    bool m_absFault;
    bool m_airbagFault;
    bool m_batteryFault;
    bool m_brakeFault;
    int m_absRemain;
    int m_airbagRemain;
    int m_batteryRemain;
    int m_brakeRemain;

    QTimer *m_faultTimer;   // 1秒定时器，用于更新故障

    // 存储当前从 SignalGenerator 收到的最新数据（用于叠加故障）
    VehicleData m_latestData;
};

#endif // SIMULATORSERVER_H
