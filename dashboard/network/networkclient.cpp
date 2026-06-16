#include "networkclient.h"

#include <QJsonDocument>
#include <QJsonObject>

NetworkClient::NetworkClient(QObject *parent)
    : QObject(parent),
      m_socket(new QTcpSocket(this)),
      m_reconnectTimer(new QTimer(this)),
      m_host("127.0.0.1"),
      m_port(5000),
      m_intentionalDisconnect(false)
{
    m_reconnectTimer->setInterval(3000);
    m_reconnectTimer->setSingleShot(false);

    connect(m_socket, SIGNAL(connected()), this, SLOT(onConnected()));
    connect(m_socket, SIGNAL(disconnected()), this, SLOT(onDisconnected()));
    connect(m_socket, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
    connect(m_socket, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(onSocketError(QAbstractSocket::SocketError)));
    connect(m_reconnectTimer, SIGNAL(timeout()), this, SLOT(onReconnectTimeout()));
}

NetworkClient::~NetworkClient()
{
    disconnectFromServer();
}

void NetworkClient::connectToServer(const QString &host, quint16 port)
{
    m_host = host;
    m_port = port;
    m_intentionalDisconnect = false;

    if(m_socket->state() != QAbstractSocket::UnconnectedState)
        m_socket->abort();

    m_socket->connectToHost(m_host, m_port);
}

void NetworkClient::disconnectFromServer()
{
    m_intentionalDisconnect = true;
    m_reconnectTimer->stop();
    m_buffer.clear();

    if(m_socket->state() != QAbstractSocket::UnconnectedState)
        m_socket->disconnectFromHost();
}

void NetworkClient::sendCommand(const QString &command)
{
    if(m_socket->state() != QAbstractSocket::ConnectedState)
    {
        emit networkError("Not connected to simulator; command was not sent.");
        return;
    }

    m_socket->write(command.toUtf8());
    m_socket->write("\n");
    m_socket->flush();
}

void NetworkClient::onConnected()
{
    m_buffer.clear();
    m_reconnectTimer->stop();
    emit connectionStateChanged(true);
}

void NetworkClient::onDisconnected()
{
    m_buffer.clear();
    emit connectionStateChanged(false);

    if(!m_intentionalDisconnect)
        scheduleReconnect();
}

void NetworkClient::onReadyRead()
{
    m_buffer.append(m_socket->readAll());
    processReceivedData();
}

void NetworkClient::onReconnectTimeout()
{
    if(m_socket->state() == QAbstractSocket::UnconnectedState)
        m_socket->connectToHost(m_host, m_port);
}

void NetworkClient::onSocketError(QAbstractSocket::SocketError error)
{
    Q_UNUSED(error);
    emit networkError(m_socket->errorString());

    if(!m_intentionalDisconnect
            && m_socket->state() == QAbstractSocket::UnconnectedState)
        scheduleReconnect();
}

void NetworkClient::scheduleReconnect()
{
    if(!m_reconnectTimer->isActive())
        m_reconnectTimer->start();
}

void NetworkClient::processReceivedData()
{
    while(true)
    {
        const int newlineIndex = m_buffer.indexOf('\n');
        if(newlineIndex < 0)
            break;

        const QByteArray jsonLine = m_buffer.left(newlineIndex).trimmed();
        m_buffer.remove(0, newlineIndex + 1);

        if(jsonLine.isEmpty())
            continue;

        bool ok = false;
        const VehicleData data = parseJsonToVehicleData(jsonLine, &ok);
        if(ok)
            emit vehicleDataChanged(data);
    }
}

VehicleData NetworkClient::parseJsonToVehicleData(const QByteArray &jsonData, bool *ok) const
{
    VehicleData data;
    if(ok != NULL)
        *ok = false;

    QJsonParseError parseError;
    const QJsonDocument document = QJsonDocument::fromJson(jsonData, &parseError);
    if(parseError.error != QJsonParseError::NoError)
    {
        emit const_cast<NetworkClient *>(this)->networkError(
                    "JSON parse failed: " + parseError.errorString());
        return data;
    }

    if(!document.isObject())
    {
        emit const_cast<NetworkClient *>(this)->networkError(
                    "JSON message is not an object.");
        return data;
    }

    const QJsonObject object = document.object();
    data.speed = object.value("speed").toDouble(data.speed);
    data.rpm = object.value("rpm").toInt(data.rpm);
    data.fuel = object.value("fuel").toDouble(data.fuel);
    data.temperature = object.value("temp").toDouble(data.temperature);
    data.turnSignal = object.value("turnSignal").toInt(data.turnSignal);
    data.hazard = object.value("hazard").toInt(data.hazard ? 1 : 0) != 0;
    data.absFault = object.value("absFault").toInt(0) != 0;
    data.airbagFault = object.value("airbagFault").toInt(0) != 0;
    data.batteryFault = object.value("batteryFault").toInt(0) != 0;
    data.brakeFault = object.value("brakeFault").toInt(0) != 0;
    data.speedFault = object.value("speedFault").toInt(0) != 0;

    if(ok != NULL)
        *ok = true;
    return data;
}
