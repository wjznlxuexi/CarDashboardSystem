#ifndef NETWORKCLIENT_H
#define NETWORKCLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QTimer>
#include <QByteArray>
#include "vehicledata.h"

class NetworkClient : public QObject
{
    Q_OBJECT

public:
    explicit NetworkClient(QObject *parent = NULL);
    ~NetworkClient();

    void connectToServer(const QString &host, quint16 port);
    void disconnectFromServer();

public slots:
    void sendCommand(const QString &command);

signals:
    void vehicleDataChanged(const VehicleData &data);
    void connectionStateChanged(bool connected);
    void networkError(const QString &message);

private slots:
    void onConnected();
    void onDisconnected();
    void onReadyRead();
    void onReconnectTimeout();

private:
    VehicleData parseJsonToVehicleData(const QByteArray &jsonData);
    void processReceivedData();

    QTcpSocket *m_socket;
    QTimer *m_reconnectTimer;
    QByteArray m_buffer;

    QString m_host;
    quint16 m_port;
    bool m_intentionalDisconnect;
};

#endif
