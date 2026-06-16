#ifndef SIMULATORSERVER_H
#define SIMULATORSERVER_H

#include <QByteArray>
#include <QHash>
#include <QObject>
#include <QSet>
#include <QTcpServer>
#include <QTcpSocket>
#include <QTimer>

#include "common/vehicledata.h"

class SimulatorServer : public QObject
{
    Q_OBJECT

public:
    explicit SimulatorServer(QObject *parent = NULL);

    bool startServer(quint16 port);
    void stopServer();

public slots:
    void broadcastVehicleData(const VehicleData &data);
    void handleCommand(const QString &command);

signals:
    void clientCountChanged(int count);
    void commandReceived(const QString &command);

private slots:
    void onNewConnection();
    void onClientDisconnected();
    void onReadyRead();
    void updateRandomFaults();

private:
    void broadcastJson(const QByteArray &json);
    void applyFaults(VehicleData &data) const;

    QTcpServer *m_server;
    QSet<QTcpSocket *> m_clients;
    QHash<QTcpSocket *, QByteArray> m_clientBuffers;
    QTimer *m_faultTimer;

    bool m_absFault;
    bool m_airbagFault;
    bool m_batteryFault;
    bool m_brakeFault;
    int m_absRemain;
    int m_airbagRemain;
    int m_batteryRemain;
    int m_brakeRemain;
};

#endif // SIMULATORSERVER_H
