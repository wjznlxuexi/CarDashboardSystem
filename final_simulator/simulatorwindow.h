#ifndef SIMULATORWINDOW_H
#define SIMULATORWINDOW_H

#include <QMainWindow>
#include "common/vehicledata.h"

class QLabel;
class QPushButton;
class QTextEdit;

class SimulatorServer;
class SignalGenerator;

/*
 * SimulatorWindow 负责模拟器的界面与模块集成，
 * 不负责具体 TCP 发送算法和信号生成算法。
 */
class SimulatorWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit SimulatorWindow(QWidget *parent = NULL);
    ~SimulatorWindow();

private slots:
    void onVehicleDataGenerated(const VehicleData &data);
    void onClientCountChanged(int count);
    void onCommandReceived(const QString &command);

    void onStartClicked();
    void onStopClicked();
    void onResetClicked();

private:
    void createModules();
    void createUi();
    void applyWindowStyle();
    void connectModules();
    void appendLog(const QString &message);
    void updateDataLabels(const VehicleData &data);
    void processCommand(const QString &command);

private:
    SimulatorServer *m_server;
    SignalGenerator *m_generator;

    QLabel *m_serverStateLabel;
    QLabel *m_clientCountLabel;
    QLabel *m_speedLabel;
    QLabel *m_rpmLabel;
    QLabel *m_fuelLabel;
    QLabel *m_tempLabel;
    QLabel *m_turnLabel;
    QLabel *m_faultLabel;

    QPushButton *m_startButton;
    QPushButton *m_stopButton;
    QPushButton *m_resetButton;

    QTextEdit *m_logEdit;

    bool m_serverStarted;
    VehicleData m_lastData;
};

#endif // SIMULATORWINDOW_H
