#include "simulatorwindow.h"

#include "signalgenerator.h"
#include "simulatorserver.h"

#include <QCheckBox>
#include <QDateTime>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QList>
#include <QMessageBox>
#include <QPushButton>
#include <QSlider>
#include <QStringList>
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
    setWindowTitle("Vehicle Signal Simulator - TCP Server");
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

    QLabel *titleLabel = new QLabel("VEHICLE SIGNAL SIMULATOR", centralWidget);
    titleLabel->setObjectName("titleLabel");
    titleLabel->setAlignment(Qt::AlignCenter);

    QGroupBox *serverGroup = new QGroupBox("Server", centralWidget);
    m_serverStateLabel = new QLabel("Stopped", serverGroup);
    m_serverStateLabel->setObjectName("serverState");
    m_clientCountLabel = new QLabel("Clients: 0", serverGroup);

    m_startButton = new QPushButton("Start", serverGroup);
    m_stopButton = new QPushButton("Stop", serverGroup);
    m_resetButton = new QPushButton("Reset", serverGroup);
    m_stopButton->setEnabled(false);

    QHBoxLayout *serverLayout = new QHBoxLayout(serverGroup);
    serverLayout->addWidget(m_serverStateLabel);
    serverLayout->addWidget(m_clientCountLabel);
    serverLayout->addStretch();
    serverLayout->addWidget(m_startButton);
    serverLayout->addWidget(m_stopButton);
    serverLayout->addWidget(m_resetButton);

    QGroupBox *dataGroup = new QGroupBox("Live Vehicle Data", centralWidget);
    m_speedLabel = new QLabel("0.0 km/h", dataGroup);
    m_rpmLabel = new QLabel("0 RPM", dataGroup);
    m_fuelLabel = new QLabel("60.00 L", dataGroup);
    m_tempLabel = new QLabel("80.0 C", dataGroup);
    m_turnLabel = new QLabel("Off", dataGroup);
    m_faultLabel = new QLabel("None", dataGroup);

    QLabel *speedName = new QLabel("Speed", dataGroup);
    QLabel *rpmName = new QLabel("RPM", dataGroup);
    QLabel *fuelName = new QLabel("Fuel", dataGroup);
    QLabel *tempName = new QLabel("Temp", dataGroup);
    QLabel *turnName = new QLabel("Turn", dataGroup);
    QLabel *faultName = new QLabel("Faults", dataGroup);

    QList<QLabel *> names;
    names << speedName << rpmName << fuelName << tempName << turnName << faultName;
    for(int i = 0; i < names.size(); ++i)
        names[i]->setObjectName("dataName");

    QList<QLabel *> values;
    values << m_speedLabel << m_rpmLabel << m_fuelLabel << m_tempLabel << m_turnLabel << m_faultLabel;
    for(int i = 0; i < values.size(); ++i)
        values[i]->setObjectName("dataValue");

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

    QGroupBox *controlGroup = new QGroupBox("Manual Control", centralWidget);
    QCheckBox *manualCheckBox = new QCheckBox("Manual mode", controlGroup);
    QLabel *manualHint = new QLabel("Auto mode keeps speed, fuel and coolant changing by itself.", controlGroup);
    manualHint->setObjectName("dataName");

    QLabel *speedControlName = new QLabel("Speed", controlGroup);
    QLabel *fuelControlName = new QLabel("Fuel", controlGroup);
    QLabel *tempControlName = new QLabel("Coolant", controlGroup);
    QSlider *speedSlider = new QSlider(Qt::Horizontal, controlGroup);
    QSlider *fuelSlider = new QSlider(Qt::Horizontal, controlGroup);
    QSlider *tempSlider = new QSlider(Qt::Horizontal, controlGroup);
    QLabel *speedSliderValue = new QLabel("0 km/h", controlGroup);
    QLabel *fuelSliderValue = new QLabel("60 L", controlGroup);
    QLabel *tempSliderValue = new QLabel("80 C", controlGroup);

    speedSlider->setRange(0, 140);
    fuelSlider->setRange(0, 60);
    tempSlider->setRange(80, 100);
    speedSlider->setValue(0);
    fuelSlider->setValue(60);
    tempSlider->setValue(80);
    speedSlider->setEnabled(false);
    fuelSlider->setEnabled(false);
    tempSlider->setEnabled(false);

    QGridLayout *controlLayout = new QGridLayout(controlGroup);
    controlLayout->setHorizontalSpacing(12);
    controlLayout->setVerticalSpacing(8);
    controlLayout->addWidget(manualCheckBox, 0, 0);
    controlLayout->addWidget(manualHint, 0, 1, 1, 2);
    controlLayout->addWidget(speedControlName, 1, 0);
    controlLayout->addWidget(speedSlider, 1, 1);
    controlLayout->addWidget(speedSliderValue, 1, 2);
    controlLayout->addWidget(fuelControlName, 2, 0);
    controlLayout->addWidget(fuelSlider, 2, 1);
    controlLayout->addWidget(fuelSliderValue, 2, 2);
    controlLayout->addWidget(tempControlName, 3, 0);
    controlLayout->addWidget(tempSlider, 3, 1);
    controlLayout->addWidget(tempSliderValue, 3, 2);

    connect(manualCheckBox, &QCheckBox::toggled, [=](bool checked) {
        speedSlider->setEnabled(checked);
        fuelSlider->setEnabled(checked);
        tempSlider->setEnabled(checked);
        m_generator->setManualMode(checked);
        appendLog(checked ? "Manual control enabled." : "Automatic simulation enabled.");
    });
    connect(speedSlider, &QSlider::valueChanged, [=](int value) {
        speedSliderValue->setText(QString("%1 km/h").arg(value));
        m_generator->setManualSpeed(value);
    });
    connect(fuelSlider, &QSlider::valueChanged, [=](int value) {
        fuelSliderValue->setText(QString("%1 L").arg(value));
        m_generator->setManualFuel(value);
    });
    connect(tempSlider, &QSlider::valueChanged, [=](int value) {
        tempSliderValue->setText(QString("%1 C").arg(value));
        m_generator->setManualTemperature(value);
    });

    QGroupBox *logGroup = new QGroupBox("Log", centralWidget);
    m_logEdit = new QTextEdit(logGroup);
    m_logEdit->setReadOnly(true);

    QVBoxLayout *logLayout = new QVBoxLayout(logGroup);
    logLayout->addWidget(m_logEdit);

    QVBoxLayout *rootLayout = new QVBoxLayout(centralWidget);
    rootLayout->setContentsMargins(22, 16, 22, 18);
    rootLayout->setSpacing(14);
    rootLayout->addWidget(titleLabel);
    rootLayout->addWidget(serverGroup);
    rootLayout->addWidget(dataGroup);
    rootLayout->addWidget(controlGroup);
    rootLayout->addWidget(logGroup, 1);
}

void SimulatorWindow::applyWindowStyle()
{
    setStyleSheet(
        "QMainWindow { background-color: #08111d; }"
        "QWidget#simulatorCentral { background-color: #0b1624; }"
        "QLabel#titleLabel { color: #78e9ff; font-family: Arial; font-size: 23px; font-weight: bold; padding: 10px; }"
        "QGroupBox { color: #b6d9ec; background-color: rgba(12, 26, 40, 220); border: 1px solid #2c526e; border-radius: 8px; margin-top: 12px; padding-top: 12px; font-weight: bold; }"
        "QGroupBox::title { subcontrol-origin: margin; left: 14px; padding: 0 6px; }"
        "QLabel { color: #c9dce8; }"
        "QLabel#serverState { color: #ff6b6b; font-size: 15px; font-weight: bold; }"
        "QLabel#dataName { color: #7194aa; font-size: 13px; }"
        "QLabel#dataValue { color: #68e7ff; font-family: Consolas; font-size: 18px; font-weight: bold; padding: 8px; background-color: rgba(3, 12, 22, 180); border: 1px solid #25445b; border-radius: 6px; }"
        "QPushButton { min-height: 34px; padding: 0 14px; color: white; background-color: #176b91; border: 1px solid #3499bf; border-radius: 6px; }"
        "QPushButton:hover { background-color: #2187b4; }"
        "QPushButton:disabled { color: #63717c; background-color: #1a2833; border-color: #2d3d49; }"
        "QCheckBox { color: #d6ecf8; font-weight: bold; }"
        "QSlider::groove:horizontal { height: 6px; background: #223545; border-radius: 3px; }"
        "QSlider::handle:horizontal { width: 16px; margin: -5px 0; background: #45cfff; border-radius: 8px; }"
        "QTextEdit { color: #8fffb3; background-color: #030a10; border: 1px solid #284257; border-radius: 7px; padding: 8px; font-family: Consolas; font-size: 13px; }"
    );
}

void SimulatorWindow::connectModules()
{
    connect(m_generator, SIGNAL(vehicleDataGenerated(VehicleData)),
            this, SLOT(onVehicleDataGenerated(VehicleData)));
    connect(m_generator, SIGNAL(vehicleDataGenerated(VehicleData)),
            m_server, SLOT(broadcastVehicleData(VehicleData)));
    connect(m_server, SIGNAL(clientCountChanged(int)),
            this, SLOT(onClientCountChanged(int)));
    connect(m_server, SIGNAL(commandReceived(QString)),
            this, SLOT(onCommandReceived(QString)));

    connect(m_startButton, SIGNAL(clicked()), this, SLOT(onStartClicked()));
    connect(m_stopButton, SIGNAL(clicked()), this, SLOT(onStopClicked()));
    connect(m_resetButton, SIGNAL(clicked()), this, SLOT(onResetClicked()));
}

void SimulatorWindow::onStartClicked()
{
    if(m_serverStarted)
        return;

    if(!m_server->startServer(5000))
    {
        QMessageBox::critical(this, "Start Failed", "Could not listen on TCP port 5000.");
        appendLog("Server start failed: port 5000 is unavailable.");
        return;
    }

    m_serverStarted = true;
    m_generator->start();
    m_serverStateLabel->setText("Running on port 5000");
    m_serverStateLabel->setStyleSheet("color:#51e58b;font-size:15px;font-weight:bold;");
    m_startButton->setEnabled(false);
    m_stopButton->setEnabled(true);
    appendLog("Server started on 0.0.0.0:5000.");
}

void SimulatorWindow::onStopClicked()
{
    if(!m_serverStarted)
        return;

    m_generator->stop();
    m_server->stopServer();
    m_serverStarted = false;
    m_serverStateLabel->setText("Stopped");
    m_serverStateLabel->setStyleSheet("color:#ff6b6b;font-size:15px;font-weight:bold;");
    m_startButton->setEnabled(true);
    m_stopButton->setEnabled(false);
    appendLog("Server stopped.");
}

void SimulatorWindow::onResetClicked()
{
    m_generator->reset();
    appendLog("Vehicle data reset locally.");
}

void SimulatorWindow::onVehicleDataGenerated(const VehicleData &data)
{
    m_lastData = data;
    updateDataLabels(data);
}

void SimulatorWindow::updateDataLabels(const VehicleData &data)
{
    m_speedLabel->setText(QString("%1 km/h").arg(data.speed, 0, 'f', 1));
    m_rpmLabel->setText(QString("%1 RPM").arg(data.rpm));
    m_fuelLabel->setText(QString("%1 L").arg(data.fuel, 0, 'f', 2));
    m_tempLabel->setText(QString("%1 C").arg(data.temperature, 0, 'f', 1));

    QString turnText = "Off";
    if(data.hazard)
        turnText = "Hazard";
    else if(data.turnSignal == 1)
        turnText = "Left";
    else if(data.turnSignal == 2)
        turnText = "Right";
    m_turnLabel->setText(turnText);

    QStringList faults;
    if(data.speedFault) faults << "SPD";
    if(data.batteryFault) faults << "BAT";
    if(data.brakeFault) faults << "BRK";
    if(data.airbagFault) faults << "AIR";
    if(data.absFault) faults << "ABS";
    if(data.fuel < 15.0) faults << "FUEL";
    if(data.temperature > 95.0) faults << "TEMP";
    m_faultLabel->setText(faults.isEmpty() ? "None" : faults.join(" / "));
}

void SimulatorWindow::onClientCountChanged(int count)
{
    m_clientCountLabel->setText(QString("Clients: %1").arg(count));
    appendLog(QString("Client count changed: %1.").arg(count));
}

void SimulatorWindow::onCommandReceived(const QString &command)
{
    appendLog("Command received: " + command);
    processCommand(command.trimmed());
}

void SimulatorWindow::processCommand(const QString &command)
{
    if(command == "turnSignal:0")
        m_generator->setTurnSignal(0);
    else if(command == "turnSignal:1")
        m_generator->setTurnSignal(1);
    else if(command == "turnSignal:2")
        m_generator->setTurnSignal(2);
    else if(command == "hazard:0")
        m_generator->setHazardEnabled(false);
    else if(command == "hazard:1")
        m_generator->setHazardEnabled(true);
    else if(command == "reset")
    {
        m_generator->reset();
        appendLog("Remote reset executed.");
    }
    else
    {
        appendLog("Unknown command: " + command);
    }
}

void SimulatorWindow::appendLog(const QString &message)
{
    const QString time = QDateTime::currentDateTime().toString("hh:mm:ss.zzz");
    m_logEdit->append("[" + time + "] " + message);
}
