#include "networkclient.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>

NetworkClient::NetworkClient(QObject *parent)
    : QObject(parent)
    , m_socket(new QTcpSocket(this))
    , m_reconnectTimer(new QTimer(this))
    , m_host("127.0.0.1")
    , m_port(5000)
    , m_intentionalDisconnect(false)
{
    // 配置重连定时器：每3秒触发一次
    m_reconnectTimer->setInterval(3000);

    // 连接socket信号到对应槽函数
    connect(m_socket, &QTcpSocket::connected,
            this, &NetworkClient::onConnected);
    connect(m_socket, &QTcpSocket::disconnected,
            this, &NetworkClient::onDisconnected);
    connect(m_socket, &QTcpSocket::readyRead,
            this, &NetworkClient::onReadyRead);

    // 连接socket错误信号
    // 连接失败时也会触发自动重连
    connect(m_socket,
            static_cast<void(QAbstractSocket::*)(QAbstractSocket::SocketError)>(
                &QAbstractSocket::error),
            this, [this](QAbstractSocket::SocketError /*error*/) {
                emit networkError(m_socket->errorString());

                // 如果不是主动断开导致的错误，启动自动重连
                if (!m_intentionalDisconnect
                    && m_socket->state() == QAbstractSocket::UnconnectedState) {
                    m_reconnectTimer->start();
                    qDebug() << "NetworkClient: connection failed,"
                             << "auto-reconnect timer started (3s interval)";
                }
            });

    // 连接重连定时器
    connect(m_reconnectTimer, &QTimer::timeout,
            this, &NetworkClient::onReconnectTimeout);
}

NetworkClient::~NetworkClient()
{
    m_reconnectTimer->stop();
    if (m_socket->state() != QAbstractSocket::UnconnectedState) {
        m_socket->disconnectFromHost();
    }
}

void NetworkClient::connectToServer(const QString &host, quint16 port)
{
    m_host = host;
    m_port = port;

    qDebug() << "NetworkClient: connecting to" << m_host << ":" << m_port;

    m_socket->connectToHost(m_host, m_port);
}

void NetworkClient::disconnectFromServer()
{
    qDebug() << "NetworkClient: disconnecting from server";

    // 标记为主动断开，阻止自动重连
    m_intentionalDisconnect = true;

    // 停止重连定时器
    m_reconnectTimer->stop();

    // 断开连接
    if (m_socket->state() != QAbstractSocket::UnconnectedState) {
        m_socket->disconnectFromHost();
    }
}

void NetworkClient::sendCommand(const QString &command)
{
    if (m_socket->state() != QAbstractSocket::ConnectedState) {
        qDebug() << "NetworkClient: cannot send command, not connected";
        emit networkError("未连接到服务器，无法发送指令");
        return;
    }

    // 指令格式：命令 + 换行符
    QByteArray data = command.toUtf8() + "\n";
    m_socket->write(data);
    m_socket->flush();

    qDebug() << "NetworkClient: sent command:" << command;
}

void NetworkClient::onConnected()
{
    qDebug() << "NetworkClient: connected to server";

    // 连接成功，清除主动断开标记，停止重连定时器
    m_intentionalDisconnect = false;
    m_reconnectTimer->stop();

    // 清空缓冲区
    m_buffer.clear();

    emit connectionStateChanged(true);
}

void NetworkClient::onDisconnected()
{
    qDebug() << "NetworkClient: disconnected from server";

    emit connectionStateChanged(false);

    // 清理缓冲区
    m_buffer.clear();

    // 如果是主动断开，不启动自动重连
    if (m_intentionalDisconnect) {
        m_intentionalDisconnect = false;
        return;
    }

    // 非主动断开（服务器关闭/网络故障），启动自动重连
    m_reconnectTimer->start();
    qDebug() << "NetworkClient: auto-reconnect timer started (3s interval)";
}

void NetworkClient::onReadyRead()
{
    // 读取所有可用数据并追加到缓冲区
    QByteArray newData = m_socket->readAll();
    m_buffer.append(newData);

    // 处理缓冲区中的完整消息
    processReceivedData();
}

void NetworkClient::onReconnectTimeout()
{
    qDebug() << "NetworkClient: attempting reconnect to"
             << m_host << ":" << m_port;

    // 尝试重新连接
    m_socket->connectToHost(m_host, m_port);
}

void NetworkClient::processReceivedData()
{
    // 以换行符拆分缓冲区中的完整JSON消息
    while (true) {
        int newlineIndex = m_buffer.indexOf('\n');
        if (newlineIndex == -1) {
            // 没有完整的消息，保留在缓冲区等待更多数据
            break;
        }

        // 提取一条完整的JSON消息（不含换行符）
        QByteArray jsonLine = m_buffer.left(newlineIndex).trimmed();

        // 从缓冲区移除已处理的部分（包括换行符）
        m_buffer.remove(0, newlineIndex + 1);

        if (jsonLine.isEmpty()) {
            // 跳过空行
            continue;
        }

        // 解析JSON并转换为VehicleData
        VehicleData data = parseJsonToVehicleData(jsonLine);

        // 发出信号通知数据已更新
        emit vehicleDataChanged(data);
    }
}

VehicleData NetworkClient::parseJsonToVehicleData(const QByteArray &jsonData)
{
    VehicleData data;

    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(jsonData, &parseError);

    if (parseError.error != QJsonParseError::NoError) {
        qDebug() << "NetworkClient: JSON parse error:" << parseError.errorString();
        qDebug() << "NetworkClient: raw data:" << QString::fromUtf8(jsonData);
        emit networkError("JSON解析错误: " + parseError.errorString());
        return data;
    }

    if (!doc.isObject()) {
        qDebug() << "NetworkClient: JSON is not an object";
        emit networkError("JSON格式错误: 不是有效的对象");
        return data;
    }

    QJsonObject obj = doc.object();

    // 解析速度
    if (obj.contains("speed")) {
        data.speed = obj["speed"].toDouble();
    }

    // 解析转速
    if (obj.contains("rpm")) {
        data.rpm = obj["rpm"].toInt();
    }

    // 解析油量
    if (obj.contains("fuel")) {
        data.fuel = obj["fuel"].toDouble();
    }

    // 解析水温（JSON字段名为 "temp"）
    if (obj.contains("temp")) {
        data.temperature = obj["temp"].toDouble();
    }

    // 解析转向灯信号
    if (obj.contains("turnSignal")) {
        data.turnSignal = obj["turnSignal"].toInt();
    }

    // 解析双闪状态（JSON中为整型 0/1）
    if (obj.contains("hazard")) {
        data.hazard = (obj["hazard"].toInt() != 0);
    }

    // 解析各故障状态（JSON中为整型 0/1）
    if (obj.contains("absFault")) {
        data.absFault = (obj["absFault"].toInt() != 0);
    }

    if (obj.contains("airbagFault")) {
        data.airbagFault = (obj["airbagFault"].toInt() != 0);
    }

    if (obj.contains("batteryFault")) {
        data.batteryFault = (obj["batteryFault"].toInt() != 0);
    }

    if (obj.contains("brakeFault")) {
        data.brakeFault = (obj["brakeFault"].toInt() != 0);
    }

    if (obj.contains("speedFault")) {
        data.speedFault = (obj["speedFault"].toInt() != 0);
    }

    return data;
}
