#include "dashboardwindow.h"

#include "network/networkclient.h"
#include "gauges/speedometerwidget.h"
#include "gauges/rpmgaugewidget.h"
#include "gauges/fuelgaugewidget.h"
#include "gauges/tempgaugewidget.h"
#include "indicators/turnindicatorwidget.h"
#include "indicators/warninglightswidget.h"
#include "audio/warningsoundsystem.h"

#include <QAction>
#include <QActionGroup>
#include <QFrame>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QSlider>
#include <QStatusBar>
#include <QVBoxLayout>
#include <QWidget>
#include <QWidgetAction>

DashboardWindow::DashboardWindow(QWidget *parent)
    : QMainWindow(parent),
      m_networkClient(NULL),
      m_speedometer(NULL),
      m_rpmGauge(NULL),
      m_fuelGauge(NULL),
      m_tempGauge(NULL),
      m_turnIndicator(NULL),
      m_warningLights(NULL),
      m_soundSystem(NULL),
      m_connectionLabel(NULL),
      m_dataLabel(NULL),
      m_leftTurnAction(NULL),
      m_rightTurnAction(NULL),
      m_turnOffAction(NULL),
      m_hazardAction(NULL),
      m_muteAction(NULL),
      m_volumeSlider(NULL),
      m_currentSoundWarning(-1),
      m_connected(false)
{
    setWindowTitle(QString::fromUtf8("汽车仪表盘 - Socket 模式"));
    resize(1180, 720);
    setMinimumSize(960, 620);

    createModules();
    createCentralLayout();
    createMenus();
    createStatusBarWidgets();
    applyWindowStyle();
    connectModules();

    statusBar()->showMessage(QString::fromUtf8("正在连接 127.0.0.1:5000..."));
    m_networkClient->connectToServer("127.0.0.1", 5000);
}

DashboardWindow::~DashboardWindow()
{
    if(m_networkClient != NULL)
        m_networkClient->disconnectFromServer();

    if(m_soundSystem != NULL)
        m_soundSystem->stopCurrentSound();
}

void DashboardWindow::createModules()
{
    m_networkClient = new NetworkClient(this);

    m_speedometer = new SpeedometerWidget(this);
    m_rpmGauge = new RpmGaugeWidget(this);
    m_fuelGauge = new FuelGaugeWidget(this);
    m_tempGauge = new TempGaugeWidget(this);
    m_turnIndicator = new TurnIndicatorWidget(this);
    m_warningLights = new WarningLightsWidget(this);

    m_soundSystem = new WarningSoundSystem(this);
    m_soundSystem->setVolume(80);
}

void DashboardWindow::createCentralLayout()
{
    QWidget *centralWidget = new QWidget(this);
    centralWidget->setObjectName("dashboardCentral");
    setCentralWidget(centralWidget);

    QLabel *systemTitle = new QLabel(
                QString::fromUtf8("CAR  DIGITAL  COCKPIT"), centralWidget);
    systemTitle->setObjectName("systemTitle");
    systemTitle->setAlignment(Qt::AlignCenter);

    QLabel *centerTitle = new QLabel(
                QString::fromUtf8("VEHICLE STATUS"), centralWidget);
    centerTitle->setObjectName("sectionTitle");
    centerTitle->setAlignment(Qt::AlignCenter);

    QFrame *centerPanel = new QFrame(centralWidget);
    centerPanel->setObjectName("centerPanel");

    QVBoxLayout *centerPanelLayout = new QVBoxLayout(centerPanel);
    centerPanelLayout->setContentsMargins(14, 14, 14, 14);
    centerPanelLayout->setSpacing(12);
    centerPanelLayout->addWidget(centerTitle);
    centerPanelLayout->addWidget(m_turnIndicator, 1);

    QHBoxLayout *smallGaugeLayout = new QHBoxLayout;
    smallGaugeLayout->setSpacing(12);
    smallGaugeLayout->addWidget(m_fuelGauge, 1);
    smallGaugeLayout->addWidget(m_tempGauge, 1);
    centerPanelLayout->addLayout(smallGaugeLayout, 2);
    centerPanelLayout->addWidget(m_warningLights, 1);

    QGridLayout *dashboardLayout = new QGridLayout;
    dashboardLayout->setHorizontalSpacing(18);
    dashboardLayout->setVerticalSpacing(14);
    dashboardLayout->addWidget(m_speedometer, 0, 0);
    dashboardLayout->addWidget(centerPanel, 0, 1);
    dashboardLayout->addWidget(m_rpmGauge, 0, 2);
    dashboardLayout->setColumnStretch(0, 4);
    dashboardLayout->setColumnStretch(1, 3);
    dashboardLayout->setColumnStretch(2, 4);

    QVBoxLayout *rootLayout = new QVBoxLayout(centralWidget);
    rootLayout->setContentsMargins(22, 16, 22, 16);
    rootLayout->setSpacing(12);
    rootLayout->addWidget(systemTitle);
    rootLayout->addLayout(dashboardLayout, 1);
}

void DashboardWindow::createMenus()
{
    QMenu *signalMenu = menuBar()->addMenu(QString::fromUtf8("信号控制"));

    QActionGroup *turnGroup = new QActionGroup(this);
    turnGroup->setExclusive(true);

    m_turnOffAction = signalMenu->addAction(QString::fromUtf8("关闭转向灯"));
    m_leftTurnAction = signalMenu->addAction(QString::fromUtf8("左转"));
    m_rightTurnAction = signalMenu->addAction(QString::fromUtf8("右转"));

    m_turnOffAction->setCheckable(true);
    m_leftTurnAction->setCheckable(true);
    m_rightTurnAction->setCheckable(true);
    m_turnOffAction->setChecked(true);

    turnGroup->addAction(m_turnOffAction);
    turnGroup->addAction(m_leftTurnAction);
    turnGroup->addAction(m_rightTurnAction);

    signalMenu->addSeparator();

    m_hazardAction = signalMenu->addAction(QString::fromUtf8("双闪警告"));
    m_hazardAction->setCheckable(true);

    signalMenu->addSeparator();
    QAction *resetAction = signalMenu->addAction(QString::fromUtf8("重置模拟器"));

    QMenu *warningMenu = menuBar()->addMenu(QString::fromUtf8("报警灯测试"));
    const char *warningNames[Warning_Count] = {
        "ENG 超速报警", "OIL 机油报警", "BAT 电瓶报警", "TEMP 水温报警",
        "FUEL 油量报警", "BRAKE 刹车报警", "AIR 安全气囊报警", "ABS 报警"
    };

    for(int index = 0; index < Warning_Count; ++index)
    {
        QAction *action = warningMenu->addAction(
                    QString::fromUtf8(warningNames[index]));
        action->setData(index);
        connect(action, SIGNAL(triggered()),
                this, SLOT(onWarningTestTriggered()));
    }

    QMenu *soundMenu = menuBar()->addMenu(QString::fromUtf8("音效控制"));

    QWidget *volumeWidget = new QWidget(soundMenu);
    QHBoxLayout *volumeLayout = new QHBoxLayout(volumeWidget);
    volumeLayout->setContentsMargins(10, 4, 10, 4);

    QLabel *volumeLabel = new QLabel(QString::fromUtf8("音量"), volumeWidget);
    m_volumeSlider = new QSlider(Qt::Horizontal, volumeWidget);
    m_volumeSlider->setRange(0, 100);
    m_volumeSlider->setValue(80);
    m_volumeSlider->setMinimumWidth(180);

    volumeLayout->addWidget(volumeLabel);
    volumeLayout->addWidget(m_volumeSlider);

    QWidgetAction *volumeAction = new QWidgetAction(soundMenu);
    volumeAction->setDefaultWidget(volumeWidget);
    soundMenu->addAction(volumeAction);

    m_muteAction = soundMenu->addAction(QString::fromUtf8("静音"));
    m_muteAction->setCheckable(true);

    connect(m_turnOffAction, SIGNAL(triggered()),
            this, SLOT(onTurnOffTriggered()));
    connect(m_leftTurnAction, SIGNAL(triggered()),
            this, SLOT(onLeftTurnTriggered()));
    connect(m_rightTurnAction, SIGNAL(triggered()),
            this, SLOT(onRightTurnTriggered()));
    connect(m_hazardAction, SIGNAL(toggled(bool)),
            this, SLOT(onHazardToggled(bool)));
    connect(resetAction, SIGNAL(triggered()),
            this, SLOT(onResetTriggered()));
    connect(m_muteAction, SIGNAL(toggled(bool)),
            this, SLOT(onMuteToggled(bool)));
    connect(m_volumeSlider, SIGNAL(valueChanged(int)),
            this, SLOT(onVolumeChanged(int)));
}

void DashboardWindow::createStatusBarWidgets()
{
    m_connectionLabel = new QLabel(
                QString::fromUtf8("● 未连接"), this);
    m_connectionLabel->setObjectName("connectionLabel");

    m_dataLabel = new QLabel(
                QString::fromUtf8("速度 0.0 km/h  |  转速 0 RPM  |  油量 60.00 L  |  水温 80.0 ℃"),
                this);
    m_dataLabel->setObjectName("dataLabel");

    statusBar()->addPermanentWidget(m_dataLabel, 1);
    statusBar()->addPermanentWidget(m_connectionLabel);
}

void DashboardWindow::applyWindowStyle()
{
    setStyleSheet(
        "QMainWindow {"
        "  background-color: #070b14;"
        "}"
        "QWidget#dashboardCentral {"
        "  background: qradialgradient(cx:0.5, cy:0.45, radius:0.9,"
        "  stop:0 #16253b, stop:0.55 #0b1422, stop:1 #05070d);"
        "}"
        "QLabel#systemTitle {"
        "  color: #b9f7ff;"
        "  font-family: Arial;"
        "  font-size: 25px;"
        "  font-weight: bold;"
        "  letter-spacing: 5px;"
        "  padding: 8px;"
        "}"
        "QLabel#sectionTitle {"
        "  color: #63dfff;"
        "  font-family: Arial;"
        "  font-size: 13px;"
        "  font-weight: bold;"
        "  letter-spacing: 2px;"
        "}"
        "QFrame#centerPanel {"
        "  background-color: rgba(6, 15, 28, 215);"
        "  border: 1px solid #27435f;"
        "  border-radius: 16px;"
        "}"
        "QMenuBar {"
        "  background-color: #0b1422;"
        "  color: #dcecff;"
        "  padding: 4px;"
        "}"
        "QMenuBar::item:selected {"
        "  background-color: #183a57;"
        "  border-radius: 4px;"
        "}"
        "QMenu {"
        "  background-color: #101b2a;"
        "  color: #e5f4ff;"
        "  border: 1px solid #2d4b68;"
        "}"
        "QMenu::item {"
        "  padding: 8px 28px 8px 14px;"
        "}"
        "QMenu::item:selected {"
        "  background-color: #1d5275;"
        "}"
        "QStatusBar {"
        "  background-color: #09111d;"
        "  color: #92b6d0;"
        "  border-top: 1px solid #20364b;"
        "}"
        "QLabel#connectionLabel {"
        "  color: #ff6b6b;"
        "  font-weight: bold;"
        "  padding: 2px 10px;"
        "}"
        "QLabel#dataLabel {"
        "  color: #83dcff;"
        "  padding: 2px 10px;"
        "}"
        "QSlider::groove:horizontal {"
        "  height: 5px;"
        "  background: #26384a;"
        "  border-radius: 2px;"
        "}"
        "QSlider::handle:horizontal {"
        "  width: 14px;"
        "  margin: -5px 0;"
        "  border-radius: 7px;"
        "  background: #42c8ff;"
        "}"
    );
}

void DashboardWindow::connectModules()
{
    connect(m_networkClient,
            SIGNAL(vehicleDataChanged(VehicleData)),
            this,
            SLOT(onVehicleDataChanged(VehicleData)));

    connect(m_networkClient,
            SIGNAL(connectionStateChanged(bool)),
            this,
            SLOT(onConnectionStateChanged(bool)));

    connect(m_networkClient,
            SIGNAL(networkError(QString)),
            this,
            SLOT(onNetworkError(QString)));
}

void DashboardWindow::onVehicleDataChanged(const VehicleData &data)
{
    m_lastData = data;

    m_speedometer->setTargetSpeed(data.speed);
    m_rpmGauge->setTargetRpm(data.rpm);
    m_fuelGauge->setFuelValue(data.fuel);
    m_tempGauge->setTemperature(data.temperature);

    m_turnIndicator->setTurnSignal(data.turnSignal);
    m_turnIndicator->setHazardEnabled(data.hazard);

    updateWarningStates(data);
    updateWarningSound(data);

    m_dataLabel->setText(
        QString::fromUtf8("速度 %1 km/h  |  转速 %2 RPM  |  油量 %3 L  |  水温 %4 ℃")
        .arg(data.speed, 0, 'f', 1)
        .arg(data.rpm)
        .arg(data.fuel, 0, 'f', 2)
        .arg(data.temperature, 0, 'f', 1));
}

void DashboardWindow::updateWarningStates(const VehicleData &data)
{
    m_warningLights->setWarningActive(Warning_Engine, data.speedFault);
    m_warningLights->setWarningActive(Warning_Oil, false);
    m_warningLights->setWarningActive(Warning_Battery, data.batteryFault);
    m_warningLights->setWarningActive(Warning_Temp, data.temperature > 95.0);
    m_warningLights->setWarningActive(Warning_Fuel, data.fuel < 15.0);
    m_warningLights->setWarningActive(Warning_Brake, data.brakeFault);
    m_warningLights->setWarningActive(Warning_Airbag, data.airbagFault);
    m_warningLights->setWarningActive(Warning_ABS, data.absFault);
}

int DashboardWindow::highestPriorityWarning(const VehicleData &data) const
{
    if(data.brakeFault)
        return Warning_Brake;
    if(data.speedFault)
        return Warning_Engine;
    if(data.temperature > 95.0)
        return Warning_Temp;
    if(data.fuel < 15.0)
        return Warning_Fuel;
    if(data.batteryFault)
        return Warning_Battery;
    if(data.airbagFault)
        return Warning_Airbag;
    if(data.absFault)
        return Warning_ABS;

    return -1;
}

void DashboardWindow::updateWarningSound(const VehicleData &data)
{
    const int warning = highestPriorityWarning(data);

    if(warning == m_currentSoundWarning)
        return;

    m_currentSoundWarning = warning;

    if(warning < 0)
        m_soundSystem->stopCurrentSound();
    else
        m_soundSystem->playWarningSound(warning);
}

void DashboardWindow::onConnectionStateChanged(bool connected)
{
    m_connected = connected;
    setConnectionVisualState(connected);

    if(connected)
        statusBar()->showMessage(QString::fromUtf8("已连接车辆信号模拟器"), 3000);
    else
        statusBar()->showMessage(QString::fromUtf8("连接已断开，客户端将自动重连"));
}

void DashboardWindow::setConnectionVisualState(bool connected)
{
    if(connected)
    {
        m_connectionLabel->setText(QString::fromUtf8("● 已连接"));
        m_connectionLabel->setStyleSheet(
                    "color:#51e58b;font-weight:bold;padding:2px 10px;");
    }
    else
    {
        m_connectionLabel->setText(QString::fromUtf8("● 未连接"));
        m_connectionLabel->setStyleSheet(
                    "color:#ff6b6b;font-weight:bold;padding:2px 10px;");
    }
}

void DashboardWindow::onNetworkError(const QString &message)
{
    statusBar()->showMessage(
                QString::fromUtf8("网络提示：") + message, 5000);
}

void DashboardWindow::sendCommand(const QString &command)
{
    if(!m_connected)
    {
        QMessageBox::information(
                    this,
                    QString::fromUtf8("网络未连接"),
                    QString::fromUtf8("仪表盘尚未连接模拟器，暂时无法发送控制指令。"));
        return;
    }

    m_networkClient->sendCommand(command);
}

void DashboardWindow::onLeftTurnTriggered()
{
    if(m_hazardAction->isChecked())
        m_hazardAction->setChecked(false);

    sendCommand("turnSignal:1");
}

void DashboardWindow::onRightTurnTriggered()
{
    if(m_hazardAction->isChecked())
        m_hazardAction->setChecked(false);

    sendCommand("turnSignal:2");
}

void DashboardWindow::onTurnOffTriggered()
{
    sendCommand("turnSignal:0");
}

void DashboardWindow::onHazardToggled(bool checked)
{
    sendCommand(checked ? "hazard:1" : "hazard:0");
}

void DashboardWindow::onResetTriggered()
{
    sendCommand("reset");
    m_turnOffAction->setChecked(true);
    m_hazardAction->setChecked(false);
}

void DashboardWindow::onMuteToggled(bool checked)
{
    m_soundSystem->setMuted(checked);
    statusBar()->showMessage(
                checked ? QString::fromUtf8("报警音效已静音")
                        : QString::fromUtf8("报警音效已恢复"),
                2500);
}

void DashboardWindow::onVolumeChanged(int value)
{
    m_soundSystem->setVolume(value);
}

void DashboardWindow::onWarningTestTriggered()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if(action == NULL)
        return;

    const int warningType = action->data().toInt();

    m_warningLights->setWarningActive(warningType, true);
    m_soundSystem->testSound(warningType);

    statusBar()->showMessage(
        QString::fromUtf8("正在测试报警项：") + action->text(),
        3000);
}
