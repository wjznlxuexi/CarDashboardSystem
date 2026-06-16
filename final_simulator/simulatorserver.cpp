#include "simulatorserver.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>
#include <cstdlib>

SimulatorServer::SimulatorServer(QObject *parent)
    : QObject(parent), m_server(new QTcpServer(this)),
      m_absFault(false), m_airbagFault(false), m_batteryFault(false), m_brakeFault(false),
      m_absRemain(0), m_airbagRemain(0), m_batteryRemain(0), m_brakeRemain(0),
      m_faultTimer(new QTimer(this))
{
    connect(m_server, &QTcpServer::newConnection, this, &SimulatorServer::onNewConnection);
    connect(m_faultTimer, &QTimer::timeout, this, &SimulatorServer::updateRandomFaults);
    m_faultTimer->start(1000);   // 每秒更新一次随机故障
}

bool SimulatorServer::startServer(quint16 port)
{
    bool ok = m_server->listen(QHostAddress::Any, port);
    if (ok) {
        qDebug() << "SimulatorServer listening on port" << port;
    } else {
        qDebug() << "Failed to start server:" << m_server->errorString();
    }
    return ok;
}

void SimulatorServer::stopServer()
{
    m_server->close();
    foreach (QTcpSocket *socket, m_clients) {
        socket->disconnectFromHost();
        socket->deleteLater();
    }
    m_clients.clear();
}

// 由 SignalGenerator 每100ms调用一次
void SimulatorServer::broadcastVehicleData(const VehicleData &data)
{
    // 保存最新原始数据
    m_latestData = data;

    // 创建一份待发送的数据，并覆盖随机故障字段
    VehicleData sendData = data;
    updateVehicleDataWithFaults(sendData);

    // 构建 JSON
    QJsonObject obj;
    obj["speed"] = sendData.speed;
    obj["rpm"] = sendData.rpm;
    obj["fuel"] = sendData.fuel;
    obj["temp"] = sendData.temperature;
    obj["turnSignal"] = sendData.turnSignal;
    obj["hazard"] = sendData.hazard ? 1 : 0;
    obj["absFault"] = sendData.absFault ? 1 : 0;
    obj["airbagFault"] = sendData.airbagFault ? 1 : 0;
    obj["batteryFault"] = sendData.batteryFault ? 1 : 0;
    obj["brakeFault"] = sendData.brakeFault ? 1 : 0;
    obj["speedFault"] = sendData.speedFault ? 1 : 0;

    QByteArray jsonData = QJsonDocument(obj).toJson(QJsonDocument::Compact) + "\n";
    broadcastJson(jsonData);
}

void SimulatorServer::broadcastJson(const QByteArray &json)
{
    for (QTcpSocket *client : m_clients) {
        if (client->state() == QAbstractSocket::ConnectedState) {
            client->write(json);
        }
    }
}

void SimulatorServer::onNewConnection()
{
    QTcpSocket *client = m_server->nextPendingConnection();
    m_clients.insert(client);
    emit clientCountChanged(m_clients.size());

    connect(client, &QTcpSocket::disconnected, this, &SimulatorServer::onClientDisconnected);
    connect(client, &QTcpSocket::readyRead, this, &SimulatorServer::onReadyRead);

    qDebug() << "New client connected from" << client->peerAddress().toString();
}

void SimulatorServer::onClientDisconnected()
{
    QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender());
    if (socket) {
        m_clients.remove(socket);
        socket->deleteLater();
        emit clientCountChanged(m_clients.size());
        qDebug() << "Client disconnected. Remaining clients:" << m_clients.size();
    }
}

void SimulatorServer::onReadyRead()
{
    QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender());
    if (!socket) return;

    QByteArray data = socket->readAll();
    QString cmd = QString::fromUtf8(data).trimmed();
    if (!cmd.isEmpty()) {
        processClientCommand(cmd, socket);
        emit commandReceived(cmd);
    }
}

void SimulatorServer::processClientCommand(const QString &cmd, QTcpSocket *client)
{
    Q_UNUSED(client);
    // 注意：多个客户端可能发送指令，需要修改全局 m_latestData 中的 turnSignal/hazard，
    // 然后通过 broadcastVehicleData 将新状态广播给所有客户端。
    if (cmd.startsWith("turnSignal:")) {
        int val = cmd.mid(11).toInt();
        if (val >= 0 && val <= 2) {
            m_latestData.turnSignal = val;
            if (val != 0) m_latestData.hazard = false;   // 转向优先，关闭双闪（符合需求）
        }
    }
    else if (cmd.startsWith("hazard:")) {
        int val = cmd.mid(7).toInt();
        m_latestData.hazard = (val == 1);
        if (m_latestData.hazard) {
            m_latestData.turnSignal = 0;    // 双闪打开时关闭转向
        }
    }
    else if (cmd == "reset") {
        // 重置所有状态（SignalGenerator 会重置，但我们也要同步重置本地存储）
        m_latestData = VehicleData();
        m_latestData.speed = 60.0;
        m_latestData.rpm = 2400;
        m_latestData.fuel = 60.0;
        m_latestData.temperature = 90.0;
        m_latestData.turnSignal = 0;
        m_latestData.hazard = false;
        // 注意：随机故障状态也需要重置
        m_absFault = false; m_airbagFault = false;
        m_batteryFault = false; m_brakeFault = false;
        m_absRemain = m_airbagRemain = m_batteryRemain = m_brakeRemain = 0;
        // 通知 SignalGenerator 重置（需要在 main 中建立连接，或通过信号槽）
        // 因为 SignalGenerator 归 main 管理，我们这里发出一个信号，让 main 调用 generator->reset()
        // 简便方法：在 main 中连接 SimulatorServer 的某个信号到 SignalGenerator::reset。
        // 这里我们定义一个内部信号，稍后在 main 中连接。
        // 为了解耦，可以在 SimulatorServer 中添加一个信号 requestReset()，由外部连接处理。
        // 或者直接在 main 中捕获 commandReceived("reset") 后调用 generator->reset()。
        // 我们采用后一种方式，在 main 中处理。
    }
    // 对于其他未知指令，忽略
}

void SimulatorServer::handleCommand(const QString &command)
{
    // 允许外部直接传入指令（例如从控制台或窗口调用）
    // 这里为了方便，模拟一个虚拟客户端，实际上不需要指定 client
    processClientCommand(command, nullptr);
}

// 随机故障更新（每秒调用一次）
void SimulatorServer::updateRandomFaults()
{
    // 辅助 lambda：触发新故障（3%概率），并随机持续时间5~15秒
    auto tryTrigger = [&](bool &fault, int &remain, int minSec=5, int maxSec=15) {
        if (!fault && (rand() % 100) < 3) {
            fault = true;
            remain = minSec + (rand() % (maxSec - minSec + 1));
        }
        if (fault) {
            remain--;
            if (remain <= 0) {
                fault = false;
            }
        }
    };

    tryTrigger(m_absFault, m_absRemain);
    tryTrigger(m_airbagFault, m_airbagRemain);
    tryTrigger(m_batteryFault, m_batteryRemain);
    tryTrigger(m_brakeFault, m_brakeRemain);
}

// 将当前随机故障状态填入 VehicleData 中
void SimulatorServer::updateVehicleDataWithFaults(VehicleData &data)
{
    data.absFault = m_absFault;
    data.airbagFault = m_airbagFault;
    data.batteryFault = m_batteryFault;
    data.brakeFault = m_brakeFault;
    // speedFault 已经由 SignalGenerator 生成，不再覆盖
}
