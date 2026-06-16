#include "simulatorwindow.h"

#include "simulatorserver.h"
#include "signalgenerator.h"

#include <QDateTime>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QWidget>

SimulatorWindow::SimulatorWindow(QWidget *parent)
    : QMainWindow(parent),
      m_server(NULL),
      m_generator(NULL),
      m_serverStateLabel(NULL),
      m_clientCountLabel(NULL),
      m_speedLabel(NULL),
      m_rpmLabel(NULL),
      m_fuelLabel(NULL),
      m_tempLabel(NULL),
      m_turnLabel(NULL),
      m_faultLabel(NULL),
      m_startButton(NULL),
      m_stopButton(NULL),
      m_resetButton(NULL),
      m_logEdit(NULL),
      m_serverStarted(false)
{
    setWindowTitle(QString::fromUtf8("汽车信号模拟器 - TCP Server"));
    resize(820, 610);
    setMinimumSize(720, 540);

    createModules();
    createUi();
    applyWindowStyle();
    connectModules();

    onStartClicked();
}

SimulatorWindow::~SimulatorWindow()
{
    if(m_generator != NULL)
        m_generator->stop();

    if(m_server != NULL)
        m_server->stopServer();
}

void SimulatorWindow::createModules()
{
    m_server = new SimulatorServer(this);
    m_generator = new SignalGenerator(this);
}

void SimulatorWindow::createUi()
{
    QWidget *centralWidget = new QWidget(this);
    centralWidget->setObjectName("simulatorCentral");
    setCentralWidget(centralWidget);

    QLabel *titleLabel = new QLabel(
                QString::fromUtf8("VEHICLE SIGNAL SIMULATOR"), centralWidget);
    titleLabel->setObjectName("titleLabel");
    titleLabel->setAlignment(Qt::AlignCenter);

    QGroupBox *serverGroup = new QGroupBox(
                QString::fromUtf8("服务器状态"), centralWidget);

    m_serverStateLabel = new QLabel(
                QString::fromUtf8("● 未启动"), serverGroup);
    m_serverStateLabel->setObjectName("serverState");

    m_clientCountLabel = new QLabel(
                QString::fromUtf8("客户端数量：0"), serverGroup);

    m_startButton = new QPushButton(
                QString::fromUtf8("启动服务器"), serverGroup);
    m_stopButton = new QPushButton(
                QString::fromUtf8("停止服务器"), serverGroup);
    m_resetButton = new QPushButton(
                QString::fromUtf8("重置状态"), serverGroup);

    m_stopButton->setEnabled(false);

    QHBoxLayout *serverLayout = new QHBoxLayout(serverGroup);
    serverLayout->addWidget(m_serverStateLabel);
    serverLayout->addWidget(m_clientCountLabel);
    serverLayout->addStretch();
    serverLayout->addWidget(m_startButton);
    serverLayout->addWidget(m_stopButton);
    serverLayout->addWidget(m_resetButton);

    QGroupBox *dataGroup = new QGroupBox(
                QString::fromUtf8("实时车辆数据"), centralWidget);

    m_speedLabel = new QLabel("0.0 km/h", dataGroup);
    m_rpmLabel = new QLabel("0 RPM", dataGroup);
    m_fuelLabel = new QLabel("60.00 L", dataGroup);
    m_tempLabel = new QLabel(QString::fromUtf8("80.0 ℃"), dataGroup);
    m_turnLabel = new QLabel(QString::fromUtf8("关闭"), dataGroup);
    m_faultLabel = new QLabel(QString::fromUtf8("无故障"), dataGroup);

    QLabel *speedName = new QLabel(QString::fromUtf8("速度"), dataGroup);
    QLabel *rpmName = new QLabel(QString::fromUtf8("转速"), dataGroup);
    QLabel *fuelName = new QLabel(QString::fromUtf8("油量"), dataGroup);
    QLabel *tempName = new QLabel(QString::fromUtf8("水温"), dataGroup);
    QLabel *turnName = new QLabel(QString::fromUtf8("转向状态"), dataGroup);
    QLabel *faultName = new QLabel(QString::fromUtf8("故障状态"), dataGroup);

    speedName->setObjectName("dataName");
    rpmName->setObjectName("dataName");
    fuelName->setObjectName("dataName");
    tempName->setObjectName("dataName");
    turnName->setObjectName("dataName");
    faultName->setObjectName("dataName");

    m_speedLabel->setObjectName("dataValue");
    m_rpmLabel->setObjectName("dataValue");
    m_fuelLabel->setObjectName("dataValue");
    m_tempLabel->setObjectName("dataValue");
    m_turnLabel->setObjectName("dataValue");
    m_faultLabel->setObjectName("dataValue");

    QGridLayout *dataLayout = new QGridLayout(dataGroup);
    dataLayout->setHorizontalSpacing(18);
    dataLayout->setVerticalSpacing(12);

    dataLayout->addWidget(speedName, 0, 0);
    dataLayout->addWidget(m_speedLabel, 0, 1);
    dataLayout->addWidget(rpmName, 0, 2);
    dataLayout->addWidget(m_rpmLabel, 0, 3);

    dataLayout->addWidget(fuelName, 1, 0);
    dataLayout->addWidget(m_fuelLabel, 1, 1);
    dataLayout->addWidget(tempName, 1, 2);
    dataLayout->addWidget(m_tempLabel, 1, 3);

    dataLayout->addWidget(turnName, 2, 0);
    dataLayout->addWidget(m_turnLabel, 2, 1);
    dataLayout->addWidget(faultName, 2, 2);
    dataLayout->addWidget(m_faultLabel, 2, 3);

    QGroupBox *logGroup = new QGroupBox(
                QString::fromUtf8("运行日志"), centralWidget);

    m_logEdit = new QTextEdit(logGroup);
    m_logEdit->setReadOnly(true);
    m_logEdit->setPlaceholderText(
                QString::fromUtf8("服务器运行日志和客户端控制指令将在这里显示。"));

    QVBoxLayout *logLayout = new QVBoxLayout(logGroup);
    logLayout->addWidget(m_logEdit);

    QVBoxLayout *rootLayout = new QVBoxLayout(centralWidget);
    rootLayout->setContentsMargins(22, 16, 22, 18);
    rootLayout->setSpacing(14);
    rootLayout->addWidget(titleLabel);
    rootLayout->addWidget(serverGroup);
    rootLayout->addWidget(dataGroup);
    rootLayout->addWidget(logGroup, 1);
}

void SimulatorWindow::applyWindowStyle()
{
    setStyleSheet(
        "QMainWindow {"
        "  background-color: #08111d;"
        "}"
        "QWidget#simulatorCentral {"
        "  background: qlineargradient(x1:0,y1:0,x2:1,y2:1,"
        "  stop:0 #0b1c2c, stop:0.55 #101a28, stop:1 #070d16);"
        "}"
        "QLabel#titleLabel {"
        "  color: #78e9ff;"
        "  font-family: Arial;"
        "  font-size: 23px;"
        "  font-weight: bold;"
        "  letter-spacing: 4px;"
        "  padding: 10px;"
        "}"
        "QGroupBox {"
        "  color: #b6d9ec;"
        "  background-color: rgba(12, 26, 40, 220);"
        "  border: 1px solid #2c526e;"
        "  border-radius: 10px;"
        "  margin-top: 12px;"
        "  padding-top: 12px;"
        "  font-weight: bold;"
        "}"
        "QGroupBox::title {"
        "  subcontrol-origin: margin;"
        "  left: 14px;"
        "  padding: 0 6px;"
        "}"
        "QLabel {"
        "  color: #c9dce8;"
        "}"
        "QLabel#serverState {"
        "  color: #ff6b6b;"
        "  font-size: 15px;"
        "  font-weight: bold;"
        "}"
        "QLabel#dataName {"
        "  color: #7194aa;"
        "  font-size: 13px;"
        "}"
        "QLabel#dataValue {"
        "  color: #68e7ff;"
        "  font-family: Consolas;"
        "  font-size: 18px;"
        "  font-weight: bold;"
        "  padding: 8px;"
        "  background-color: rgba(3, 12, 22, 180);"
        "  border: 1px solid #25445b;"
        "  border-radius: 6px;"
        "}"
        "QPushButton {"
        "  min-height: 34px;"
        "  padding: 0 14px;"
        "  color: white;"
        "  background-color: #176b91;"
        "  border: 1px solid #3499bf;"
        "  border-radius: 6px;"
        "}"
        "QPushButton:hover {"
        "  background-color: #2187b4;"
        "}"
        "QPushButton:disabled {"
        "  color: #63717c;"
        "  background-color: #1a2833;"
        "  border-color: #2d3d49;"
        "}"
        "QTextEdit {"
        "  color: #8fffb3;"
        "  background-color: #030a10;"
        "  border: 1px solid #284257;"
        "  border-radius: 7px;"
        "  padding: 8px;"
        "  font-family: Consolas;"
        "  font-size: 13px;"
        "}"
    );
}

void SimulatorWindow::connectModules()
{
    connect(m_generator,
            SIGNAL(vehicleDataGenerated(VehicleData)),
            this,
            SLOT(onVehicleDataGenerated(VehicleData)));

    connect(m_generator,
            SIGNAL(vehicleDataGenerated(VehicleData)),
            m_server,
            SLOT(broadcastVehicleData(VehicleData)));

    connect(m_server,
            SIGNAL(clientCountChanged(int)),
            this,
            SLOT(onClientCountChanged(int)));

    connect(m_server,
            SIGNAL(commandReceived(QString)),
            this,
            SLOT(onCommandReceived(QString)));

    connect(m_startButton, SIGNAL(clicked()),
            this, SLOT(onStartClicked()));
    connect(m_stopButton, SIGNAL(clicked()),
            this, SLOT(onStopClicked()));
    connect(m_resetButton, SIGNAL(clicked()),
            this, SLOT(onResetClicked()));
}

void SimulatorWindow::onStartClicked()
{
    if(m_serverStarted)
        return;

    if(!m_server->startServer(5000))
    {
        QMessageBox::critical(
                    this,
                    QString::fromUtf8("启动失败"),
                    QString::fromUtf8("无法监听5000端口，请检查端口是否被占用。"));

        appendLog(QString::fromUtf8("服务器启动失败：端口5000不可用。"));
        return;
    }

    m_serverStarted = true;
    m_generator->start();

    m_serverStateLabel->setText(QString::fromUtf8("● 运行中  端口 5000"));
    m_serverStateLabel->setStyleSheet(
                "color:#51e58b;font-size:15px;font-weight:bold;");

    m_startButton->setEnabled(false);
    m_stopButton->setEnabled(true);

    appendLog(QString::fromUtf8("服务器启动成功，正在监听0.0.0.0:5000。"));
}

void SimulatorWindow::onStopClicked()
{
    if(!m_serverStarted)
        return;

    m_generator->stop();
    m_server->stopServer();
    m_serverStarted = false;

    m_serverStateLabel->setText(QString::fromUtf8("● 已停止"));
    m_serverStateLabel->setStyleSheet(
                "color:#ff6b6b;font-size:15px;font-weight:bold;");

    m_startButton->setEnabled(true);
    m_stopButton->setEnabled(false);

    appendLog(QString::fromUtf8("服务器和数据模拟已停止。"));
}

void SimulatorWindow::onResetClicked()
{
    m_generator->reset();
    appendLog(QString::fromUtf8("车辆状态已由本地按钮重置。"));
}

void SimulatorWindow::onVehicleDataGenerated(const VehicleData &data)
{
    m_lastData = data;
    updateDataLabels(data);
}

void SimulatorWindow::updateDataLabels(const VehicleData &data)
{
    m_speedLabel->setText(
                QString("%1 km/h").arg(data.speed, 0, 'f', 1));
    m_rpmLabel->setText(
                QString("%1 RPM").arg(data.rpm));
    m_fuelLabel->setText(
                QString("%1 L").arg(data.fuel, 0, 'f', 2));
    m_tempLabel->setText(
                QString::fromUtf8("%1 ℃").arg(data.temperature, 0, 'f', 1));

    QString turnText = QString::fromUtf8("关闭");
    if(data.hazard)
        turnText = QString::fromUtf8("双闪");
    else if(data.turnSignal == 1)
        turnText = QString::fromUtf8("左转");
    else if(data.turnSignal == 2)
        turnText = QString::fromUtf8("右转");

    m_turnLabel->setText(turnText);

    QStringList faults;
    if(data.speedFault) faults << "ENG";
    if(data.batteryFault) faults << "BAT";
    if(data.brakeFault) faults << "BRAKE";
    if(data.airbagFault) faults << "AIR";
    if(data.absFault) faults << "ABS";
    if(data.fuel < 15.0) faults << "FUEL";
    if(data.temperature > 95.0) faults << "TEMP";

    m_faultLabel->setText(
                faults.isEmpty() ? QString::fromUtf8("无故障")
                                 : faults.join(" / "));
}

void SimulatorWindow::onClientCountChanged(int count)
{
    m_clientCountLabel->setText(
                QString::fromUtf8("客户端数量：%1").arg(count));

    appendLog(
        QString::fromUtf8("当前已连接客户端数量更新为 %1。").arg(count));
}

void SimulatorWindow::onCommandReceived(const QString &command)
{
    appendLog(QString::fromUtf8("收到客户端指令：") + command);
    processCommand(command.trimmed());
}

void SimulatorWindow::processCommand(const QString &command)
{
    if(command == "turnSignal:0")
    {
        m_generator->setTurnSignal(0);
    }
    else if(command == "turnSignal:1")
    {
        m_generator->setTurnSignal(1);
    }
    else if(command == "turnSignal:2")
    {
        m_generator->setTurnSignal(2);
    }
    else if(command == "hazard:0")
    {
        m_generator->setHazardEnabled(false);
    }
    else if(command == "hazard:1")
    {
        m_generator->setHazardEnabled(true);
    }
    else if(command == "reset")
    {
        m_generator->reset();
        appendLog(QString::fromUtf8("已执行远程重置指令。"));
    }
    else
    {
        appendLog(QString::fromUtf8("无法识别的控制指令：") + command);
    }
}

void SimulatorWindow::appendLog(const QString &message)
{
    const QString time =
            QDateTime::currentDateTime().toString("hh:mm:ss.zzz");

    m_logEdit->append("[" + time + "] " + message);
}
