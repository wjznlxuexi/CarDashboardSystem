#include "simulatorserver.h"

#include <QHostAddress>
#include <QJsonDocument>
#include <QJsonObject>
#include <QtGlobal>
#include <cstdlib>

SimulatorServer::SimulatorServer(QObject *parent)
    : QObject(parent),
      m_server(new QTcpServer(this)),
      m_faultTimer(new QTimer(this)),
      m_absFault(false),
      m_airbagFault(false),
      m_batteryFault(false),
      m_brakeFault(false),
      m_absRemain(0),
      m_airbagRemain(0),
      m_batteryRemain(0),
      m_brakeRemain(0)
{
    connect(m_server, SIGNAL(newConnection()), this, SLOT(onNewConnection()));
    connect(m_faultTimer, SIGNAL(timeout()), this, SLOT(updateRandomFaults()));
    m_faultTimer->setInterval(1000);
    m_faultTimer->start();
}

bool SimulatorServer::startServer(quint16 port)
{
    if(m_server->isListening())
        return true;

    return m_server->listen(QHostAddress::Any, port);
}

void SimulatorServer::stopServer()
{
    m_server->close();

    const QList<QTcpSocket *> clients = m_clients.values();
    for(int i = 0; i < clients.size(); ++i)
    {
        clients[i]->disconnectFromHost();
        clients[i]->deleteLater();
    }

    m_clients.clear();
    m_clientBuffers.clear();
    emit clientCountChanged(0);
}

void SimulatorServer::broadcastVehicleData(const VehicleData &data)
{
    VehicleData sendData = data;
    applyFaults(sendData);

    QJsonObject object;
    object["speed"] = sendData.speed;
    object["rpm"] = sendData.rpm;
    object["fuel"] = sendData.fuel;
    object["temp"] = sendData.temperature;
    object["turnSignal"] = sendData.turnSignal;
    object["hazard"] = sendData.hazard ? 1 : 0;
    object["absFault"] = sendData.absFault ? 1 : 0;
    object["airbagFault"] = sendData.airbagFault ? 1 : 0;
    object["batteryFault"] = sendData.batteryFault ? 1 : 0;
    object["brakeFault"] = sendData.brakeFault ? 1 : 0;
    object["speedFault"] = sendData.speedFault ? 1 : 0;

    QByteArray json = QJsonDocument(object).toJson(QJsonDocument::Compact);
    json.append('\n');
    broadcastJson(json);
}

void SimulatorServer::handleCommand(const QString &command)
{
    const QString trimmed = command.trimmed();
    if(!trimmed.isEmpty())
        emit commandReceived(trimmed);
}

void SimulatorServer::broadcastJson(const QByteArray &json)
{
    const QList<QTcpSocket *> clients = m_clients.values();
    for(int i = 0; i < clients.size(); ++i)
    {
        if(clients[i]->state() == QAbstractSocket::ConnectedState)
            clients[i]->write(json);
    }
}

void SimulatorServer::onNewConnection()
{
    while(m_server->hasPendingConnections())
    {
        QTcpSocket *client = m_server->nextPendingConnection();
        m_clients.insert(client);
        m_clientBuffers.insert(client, QByteArray());

        connect(client, SIGNAL(disconnected()), this, SLOT(onClientDisconnected()));
        connect(client, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
    }

    emit clientCountChanged(m_clients.size());
}

void SimulatorServer::onClientDisconnected()
{
    QTcpSocket *client = qobject_cast<QTcpSocket *>(sender());
    if(client == NULL)
        return;

    m_clients.remove(client);
    m_clientBuffers.remove(client);
    client->deleteLater();
    emit clientCountChanged(m_clients.size());
}

void SimulatorServer::onReadyRead()
{
    QTcpSocket *client = qobject_cast<QTcpSocket *>(sender());
    if(client == NULL)
        return;

    QByteArray &buffer = m_clientBuffers[client];
    buffer.append(client->readAll());

    while(true)
    {
        const int newlineIndex = buffer.indexOf('\n');
        if(newlineIndex < 0)
            break;

        const QString command = QString::fromUtf8(buffer.left(newlineIndex)).trimmed();
        buffer.remove(0, newlineIndex + 1);

        if(!command.isEmpty())
            emit commandReceived(command);
    }
}

void SimulatorServer::updateRandomFaults()
{
    auto updateFault = [](bool &fault, int &remain) {
        if(!fault && (std::rand() % 100) < 3)
        {
            fault = true;
            remain = 5 + std::rand() % 11;
        }

        if(fault)
        {
            --remain;
            if(remain <= 0)
                fault = false;
        }
    };

    updateFault(m_absFault, m_absRemain);
    updateFault(m_airbagFault, m_airbagRemain);
    updateFault(m_batteryFault, m_batteryRemain);
    updateFault(m_brakeFault, m_brakeRemain);
}

void SimulatorServer::applyFaults(VehicleData &data) const
{
    data.absFault = m_absFault;
    data.airbagFault = m_airbagFault;
    data.batteryFault = m_batteryFault;
    data.brakeFault = m_brakeFault;
}
