#ifndef DASHBOARDWINDOW_H
#define DASHBOARDWINDOW_H

#include <QMainWindow>
#include "common/vehicledata.h"
#include "common/warningtype.h"

class QLabel;
class QAction;
class QSlider;
class QWidget;
class QTimer;

class NetworkClient;
class SpeedometerWidget;
class RpmGaugeWidget;
class FuelGaugeWidget;
class TempGaugeWidget;
class TurnIndicatorWidget;
class WarningLightsWidget;
class WarningSoundSystem;
class ImageDashboardWidget;

/*
 * DashboardWindow 仅负责：
 * 1. 创建和布局各个功能模块；
 * 2. 建立模块之间的信号槽连接；
 * 3. 将 VehicleData 分发给对应控件；
 * 4. 创建控制菜单、状态栏和音效菜单。
 *
 * 不在本类中编写具体表盘绘制、TCP解析或音频播放算法。
 */
class DashboardWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit DashboardWindow(QWidget *parent = NULL);
    ~DashboardWindow();

private slots:
    void onVehicleDataChanged(const VehicleData &data);
    void onConnectionStateChanged(bool connected);
    void onNetworkError(const QString &message);

    void onLeftTurnTriggered();
    void onRightTurnTriggered();
    void onTurnOffTriggered();
    void onHazardToggled(bool checked);
    void onResetTriggered();

    void onMuteToggled(bool checked);
    void onVolumeChanged(int value);
    void onWarningTestTriggered();
    void onWarningTestFinished();

private:
    void createModules();
    void createCentralLayout();
    void createMenus();
    void createStatusBarWidgets();
    void applyWindowStyle();
    void connectModules();

    void updateWarningStates(const VehicleData &data);
    void updateWarningSound(const VehicleData &data);
    int highestPriorityWarning(const VehicleData &data) const;
    void sendCommand(const QString &command);
    void setConnectionVisualState(bool connected);

private:
    NetworkClient *m_networkClient;

    SpeedometerWidget *m_speedometer;
    RpmGaugeWidget *m_rpmGauge;
    FuelGaugeWidget *m_fuelGauge;
    TempGaugeWidget *m_tempGauge;
    TurnIndicatorWidget *m_turnIndicator;
    WarningLightsWidget *m_warningLights;
    WarningSoundSystem *m_soundSystem;
    ImageDashboardWidget *m_imageDashboard;

    QLabel *m_connectionLabel;
    QLabel *m_dataLabel;

    QAction *m_leftTurnAction;
    QAction *m_rightTurnAction;
    QAction *m_turnOffAction;
    QAction *m_hazardAction;
    QAction *m_muteAction;

    QSlider *m_volumeSlider;
    QTimer *m_warningTestTimer;

    VehicleData m_lastData;
    int m_currentSoundWarning;
    bool m_connected;
    bool m_warningTestActive;
};

#endif // DASHBOARDWINDOW_H
