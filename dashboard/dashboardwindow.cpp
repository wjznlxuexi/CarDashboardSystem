#include "dashboardwindow.h"

#include "audio/warningsoundsystem.h"
#include "gauges/fuelgaugewidget.h"
#include "gauges/rpmgaugewidget.h"
#include "gauges/speedometerwidget.h"
#include "gauges/tempgaugewidget.h"
#include "indicators/turnindicatorwidget.h"
#include "indicators/warninglightswidget.h"
#include "network/networkclient.h"

#include <QAction>
#include <QActionGroup>
#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QPainter>
#include <QPaintEvent>
#include <QPixmap>
#include <QSlider>
#include <QStatusBar>
#include <QStringList>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>
#include <QWidgetAction>
#include <cmath>

class ImageDashboardWidget : public QWidget
{
public:
    explicit ImageDashboardWidget(QWidget *parent = NULL)
        : QWidget(parent)
        , m_connected(false)
        , m_blinkOn(true)
        , m_scaleX(1.0)
        , m_scaleY(1.0)
    {
        setMinimumSize(980, 500);
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

        m_background.load(":/background/dashboard.png");

        const QString appDir = QCoreApplication::applicationDirPath();
        const QString currentDir = QDir::currentPath();
        const QStringList candidates = QStringList()
                << currentDir + "/dashboard.png"
                << appDir + "/../../dashboard.png"
                << appDir + "/../dashboard.png"
                << appDir + "/dashboard.png";

        for(int i = 0; i < candidates.size(); ++i)
        {
            const QFileInfo info(QDir::cleanPath(candidates[i]));
            if(m_background.isNull() && info.exists() && m_background.load(info.absoluteFilePath()))
                break;
        }

        if(m_background.isNull())
            m_background = QPixmap(1920, 864);

        const char *warningPaths[Warning_Count] = {
            ":/images/warning_speed.png",
            ":/images/warning_oil.png",
            ":/images/warning_battery.png",
            ":/images/warning_temp.png",
            ":/images/warning_fuel.png",
            ":/images/warning_brake.png",
            ":/images/warning_airbag.png",
            ":/images/warning_abs.png"
        };

        for(int i = 0; i < Warning_Count; ++i)
            m_warningPixmaps[i] = QPixmap(warningPaths[i]);

        QTimer *blinkTimer = new QTimer(this);
        blinkTimer->setInterval(500);
        connect(blinkTimer, &QTimer::timeout, [this]() {
            m_blinkOn = !m_blinkOn;
            update();
        });
        blinkTimer->start();
    }

    void setVehicleData(const VehicleData &data)
    {
        m_data = data;
        update();
    }

    void setConnected(bool connected)
    {
        m_connected = connected;
        update();
    }

protected:
    void paintEvent(QPaintEvent *event)
    {
        Q_UNUSED(event);

        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.fillRect(rect(), QColor(0, 0, 0));

        QSize imageSize = m_background.size();
        if(imageSize.isEmpty())
            imageSize = QSize(1920, 864);

        QSize targetSize = imageSize.scaled(size(), Qt::KeepAspectRatio);
        QRect targetRect((width() - targetSize.width()) / 2,
                         (height() - targetSize.height()) / 2,
                         targetSize.width(),
                         targetSize.height());

        painter.drawPixmap(targetRect, m_background);

        m_scaleX = targetRect.width() / 1920.0;
        m_scaleY = targetRect.height() / 864.0;
        m_offset = targetRect.topLeft();

        drawNeedle(painter, QPointF(445, 460), 188, rpmToAngle(m_data.rpm), QColor(232, 34, 42), 7);
        drawNeedle(painter, QPointF(1475, 460), 190, speedToAngle(m_data.speed), QColor(232, 34, 42), 7);
        drawNeedle(painter, QPointF(832, 280), 58, tempToAngle(m_data.temperature), QColor(245, 245, 245), 4);
        drawNeedle(painter, QPointF(1082, 280), 58, fuelToAngle(m_data.fuel), QColor(245, 245, 245), 4);

        drawCenterScreen(painter);
        drawTurnHints(painter);
        drawTcpStatus(painter);
    }

private:
    QPointF mapPoint(const QPointF &point) const
    {
        return QPointF(m_offset.x() + point.x() * m_scaleX,
                       m_offset.y() + point.y() * m_scaleY);
    }

    QRectF mapRect(const QRectF &rect) const
    {
        return QRectF(mapPoint(rect.topLeft()),
                      QSizeF(rect.width() * m_scaleX, rect.height() * m_scaleY));
    }

    double speedToAngle(double speed) const
    {
        return 150.0 + qBound(0.0, speed, 220.0) / 220.0 * 240.0;
    }

    double rpmToAngle(int rpm) const
    {
        return 150.0 + qBound(0, rpm, 8000) / 8000.0 * 240.0;
    }

    double fuelToAngle(double fuel) const
    {
        const double ratio = qBound(0.0, fuel, 60.0) / 60.0;
        return 180.0 + ratio * 165.0;
    }

    double tempToAngle(double temperature) const
    {
        const double ratio = (qBound(80.0, temperature, 100.0) - 80.0) / 20.0;
        return 184.0 + ratio * 160.0;
    }

    void drawNeedle(QPainter &painter, const QPointF &sourceCenter, double sourceLength,
                    double angleDegrees, const QColor &color, int width)
    {
        const QPointF center = mapPoint(sourceCenter);
        const double length = sourceLength * qMin(m_scaleX, m_scaleY);
        const double radians = angleDegrees * 3.14159265358979323846 / 180.0;
        const double scale = qMin(m_scaleX, m_scaleY);
        const QPointF tip(center.x() + std::cos(radians) * length,
                          center.y() + std::sin(radians) * length);
        const QPointF tail(center.x() - std::cos(radians) * length * 0.08,
                           center.y() - std::sin(radians) * length * 0.08);
        const QPointF normal(-std::sin(radians), std::cos(radians));

        QPolygonF needle;
        needle << tip
               << QPointF(center.x() + normal.x() * width * 0.8 * scale,
                          center.y() + normal.y() * width * 0.8 * scale)
               << tail
               << QPointF(center.x() - normal.x() * width * 0.8 * scale,
                          center.y() - normal.y() * width * 0.8 * scale);

        painter.save();
        painter.setPen(QPen(QColor(0, 0, 0, 180), 2 * scale));
        painter.setBrush(QColor(0, 0, 0, 125));
        painter.drawPolygon(needle.translated(2 * scale, 2 * scale));
        painter.setPen(QPen(color.lighter(150), 1 * scale));
        painter.setBrush(color);
        painter.drawPolygon(needle);
        painter.setBrush(QColor(18, 18, 18));
        painter.setPen(QPen(QColor(215, 215, 215), 2 * scale));
        painter.drawEllipse(center, 16 * scale, 16 * scale);
        painter.setBrush(QColor(0, 0, 0, 120));
        painter.setPen(Qt::NoPen);
        painter.drawEllipse(center, 7 * scale, 7 * scale);
        painter.restore();
    }

    void drawCenterScreen(QPainter &painter)
    {
        QRectF screen = mapRect(QRectF(786, 382, 350, 250));
        painter.save();
        painter.setPen(QPen(QColor(68, 180, 255, 160), 1));
        painter.setBrush(QColor(0, 18, 58, 135));
        painter.drawRoundedRect(screen.adjusted(10, 10, -10, -10), 10, 10);

        painter.setPen(QColor(190, 235, 255));
        painter.setFont(QFont("Consolas", qMax(13, static_cast<int>(24 * m_scaleY)), QFont::Bold));
        painter.drawText(screen.adjusted(0, 24 * m_scaleY, 0, 0), Qt::AlignHCenter,
                         QString("%1 km/h").arg(m_data.speed, 0, 'f', 1));

        painter.setFont(QFont("Consolas", qMax(9, static_cast<int>(13 * m_scaleY)), QFont::Bold));
        painter.setPen(QColor(117, 210, 255));
        QString line1 = QString("RPM %1    FUEL %2L").arg(m_data.rpm).arg(m_data.fuel, 0, 'f', 1);
        QString line2 = QString("TEMP %1C   %2").arg(m_data.temperature, 0, 'f', 1)
                .arg(m_data.hazard ? "HAZARD" : "DRIVE");
        painter.drawText(screen.adjusted(0, 86 * m_scaleY, 0, 0), Qt::AlignHCenter, line1);
        painter.drawText(screen.adjusted(0, 122 * m_scaleY, 0, 0), Qt::AlignHCenter, line2);

        QStringList warnings;
        if(m_data.speedFault) warnings << "SPEED";
        if(m_data.batteryFault) warnings << "BAT";
        if(m_data.brakeFault) warnings << "BRAKE";
        if(m_data.airbagFault) warnings << "AIRBAG";
        if(m_data.absFault) warnings << "ABS";
        if(m_data.fuel < 15.0) warnings << "LOW FUEL";
        if(m_data.temperature > 95.0) warnings << "HIGH TEMP";

        painter.setPen(warnings.isEmpty() ? QColor(78, 255, 136) : QColor(255, 78, 78));
        painter.drawText(screen.adjusted(0, 172 * m_scaleY, 0, 0), Qt::AlignHCenter,
                         warnings.isEmpty() ? "ALL SYSTEMS NORMAL" : warnings.join("  "));
        drawWarningIcons(painter, screen);
        painter.restore();
    }

    void drawWarningIcons(QPainter &painter, const QRectF &screen)
    {
        const bool active[Warning_Count] = {
            m_data.speedFault,
            false,
            m_data.batteryFault,
            m_data.temperature > 95.0,
            m_data.fuel < 15.0,
            m_data.brakeFault,
            m_data.airbagFault,
            m_data.absFault
        };

        const double iconSize = 28 * qMin(m_scaleX, m_scaleY);
        const double gap = 9 * m_scaleX;
        const double totalWidth = Warning_Count * iconSize + (Warning_Count - 1) * gap;
        double x = screen.center().x() - totalWidth / 2.0;
        const double y = screen.bottom() - 47 * m_scaleY;

        for(int i = 0; i < Warning_Count; ++i)
        {
            QRectF iconRect(x, y, iconSize, iconSize);
            painter.save();
            painter.setPen(QPen(active[i] ? QColor(255, 78, 78) : QColor(38, 86, 115), 1));
            painter.setBrush(active[i] ? QColor(60, 5, 8, 170) : QColor(0, 8, 18, 150));
            painter.drawRoundedRect(iconRect.adjusted(-4, -4, 4, 4), 4, 4);

            if(!m_warningPixmaps[i].isNull())
            {
                painter.setOpacity(active[i] ? 1.0 : 0.25);
                painter.drawPixmap(iconRect.toRect(), m_warningPixmaps[i]);
            }
            else
            {
                static const char *names[Warning_Count] = {
                    "SPD", "OIL", "BAT", "TMP", "FUL", "BRK", "AIR", "ABS"
                };
                painter.setOpacity(active[i] ? 1.0 : 0.35);
                painter.setPen(active[i] ? QColor(255, 78, 78) : QColor(95, 139, 160));
                painter.setFont(QFont("Arial", qMax(7, static_cast<int>(9 * m_scaleY)), QFont::Bold));
                painter.drawText(iconRect, Qt::AlignCenter, names[i]);
            }
            painter.restore();
            x += iconSize + gap;
        }
    }

    void drawTurnHints(QPainter &painter)
    {
        painter.save();
        const bool left = m_blinkOn && (m_data.hazard || m_data.turnSignal == 1);
        const bool right = m_blinkOn && (m_data.hazard || m_data.turnSignal == 2);
        drawArrow(painter, QPointF(742, 585), true, left);
        drawArrow(painter, QPointF(1178, 585), false, right);
        painter.restore();
    }

    void drawArrow(QPainter &painter, const QPointF &sourceCenter, bool left, bool active)
    {
        QPointF c = mapPoint(sourceCenter);
        if(!active)
            return;

        double s = 22 * qMin(m_scaleX, m_scaleY);
        QColor color = QColor(47, 255, 105, 220);
        painter.setPen(QPen(QColor(185, 255, 202), 2 * qMin(m_scaleX, m_scaleY)));
        painter.setBrush(color);

        QPolygonF polygon;
        if(left)
        {
            polygon << QPointF(c.x() - s, c.y())
                    << QPointF(c.x() - s * 0.15, c.y() - s * 0.7)
                    << QPointF(c.x() - s * 0.15, c.y() - s * 0.25)
                    << QPointF(c.x() + s, c.y() - s * 0.25)
                    << QPointF(c.x() + s, c.y() + s * 0.25)
                    << QPointF(c.x() - s * 0.15, c.y() + s * 0.25)
                    << QPointF(c.x() - s * 0.15, c.y() + s * 0.7);
        }
        else
        {
            polygon << QPointF(c.x() + s, c.y())
                    << QPointF(c.x() + s * 0.15, c.y() - s * 0.7)
                    << QPointF(c.x() + s * 0.15, c.y() - s * 0.25)
                    << QPointF(c.x() - s, c.y() - s * 0.25)
                    << QPointF(c.x() - s, c.y() + s * 0.25)
                    << QPointF(c.x() + s * 0.15, c.y() + s * 0.25)
                    << QPointF(c.x() + s * 0.15, c.y() + s * 0.7);
        }
        painter.drawPolygon(polygon);
    }

    void drawTcpStatus(QPainter &painter)
    {
        QRectF box = mapRect(QRectF(1510, 772, 250, 38));
        painter.save();
        painter.setPen(QPen(m_connected ? QColor(77, 255, 132) : QColor(255, 88, 88), 1));
        painter.setBrush(QColor(0, 8, 13, 185));
        painter.drawRoundedRect(box, 6, 6);
        painter.setFont(QFont("Consolas", qMax(8, static_cast<int>(13 * m_scaleY)), QFont::Bold));
        painter.setPen(m_connected ? QColor(77, 255, 132) : QColor(255, 88, 88));
        painter.drawText(box, Qt::AlignCenter, m_connected ? "TCP 127.0.0.1:5000 ONLINE" : "TCP RECONNECTING...");
        painter.restore();
    }

    QPixmap m_background;
    QPixmap m_warningPixmaps[Warning_Count];
    VehicleData m_data;
    bool m_connected;
    bool m_blinkOn;
    QPoint m_offset;
    double m_scaleX;
    double m_scaleY;
};

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
      m_imageDashboard(NULL),
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
    setWindowTitle("Car Dashboard System - Dashboard");
    resize(1180, 720);
    setMinimumSize(960, 620);

    createModules();
    createCentralLayout();
    createMenus();
    createStatusBarWidgets();
    applyWindowStyle();
    connectModules();

    statusBar()->showMessage("Connecting to 127.0.0.1:5000...");
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
    m_imageDashboard = new ImageDashboardWidget(this);
    m_soundSystem->setVolume(80);
}

void DashboardWindow::createCentralLayout()
{
    QWidget *centralWidget = new QWidget(this);
    centralWidget->setObjectName("dashboardCentral");
    setCentralWidget(centralWidget);

    QVBoxLayout *rootLayout = new QVBoxLayout(centralWidget);
    rootLayout->setContentsMargins(0, 0, 0, 0);
    rootLayout->setSpacing(0);
    rootLayout->addWidget(m_imageDashboard, 1);
}

void DashboardWindow::createMenus()
{
    QMenu *signalMenu = menuBar()->addMenu("Signals");

    QActionGroup *turnGroup = new QActionGroup(this);
    turnGroup->setExclusive(true);

    m_turnOffAction = signalMenu->addAction("Turn Off");
    m_leftTurnAction = signalMenu->addAction("Left Turn");
    m_rightTurnAction = signalMenu->addAction("Right Turn");

    m_turnOffAction->setCheckable(true);
    m_leftTurnAction->setCheckable(true);
    m_rightTurnAction->setCheckable(true);
    m_turnOffAction->setChecked(true);

    turnGroup->addAction(m_turnOffAction);
    turnGroup->addAction(m_leftTurnAction);
    turnGroup->addAction(m_rightTurnAction);

    signalMenu->addSeparator();

    m_hazardAction = signalMenu->addAction("Hazard");
    m_hazardAction->setCheckable(true);

    signalMenu->addSeparator();
    QAction *resetAction = signalMenu->addAction("Reset Simulator");

    QMenu *warningMenu = menuBar()->addMenu("Warning Test");
    const char *warningNames[Warning_Count] = {
        "Speed", "Oil", "Battery", "Temperature",
        "Fuel", "Brake", "Airbag", "ABS"
    };

    for(int index = 0; index < Warning_Count; ++index)
    {
        QAction *action = warningMenu->addAction(warningNames[index]);
        action->setData(index);
        connect(action, SIGNAL(triggered()), this, SLOT(onWarningTestTriggered()));
    }

    QMenu *soundMenu = menuBar()->addMenu("Sound");

    QWidget *volumeWidget = new QWidget(soundMenu);
    QHBoxLayout *volumeLayout = new QHBoxLayout(volumeWidget);
    volumeLayout->setContentsMargins(10, 4, 10, 4);

    QLabel *volumeLabel = new QLabel("Volume", volumeWidget);
    m_volumeSlider = new QSlider(Qt::Horizontal, volumeWidget);
    m_volumeSlider->setRange(0, 100);
    m_volumeSlider->setValue(80);
    m_volumeSlider->setMinimumWidth(180);

    volumeLayout->addWidget(volumeLabel);
    volumeLayout->addWidget(m_volumeSlider);

    QWidgetAction *volumeAction = new QWidgetAction(soundMenu);
    volumeAction->setDefaultWidget(volumeWidget);
    soundMenu->addAction(volumeAction);

    m_muteAction = soundMenu->addAction("Mute");
    m_muteAction->setCheckable(true);

    connect(m_turnOffAction, SIGNAL(triggered()), this, SLOT(onTurnOffTriggered()));
    connect(m_leftTurnAction, SIGNAL(triggered()), this, SLOT(onLeftTurnTriggered()));
    connect(m_rightTurnAction, SIGNAL(triggered()), this, SLOT(onRightTurnTriggered()));
    connect(m_hazardAction, SIGNAL(toggled(bool)), this, SLOT(onHazardToggled(bool)));
    connect(resetAction, SIGNAL(triggered()), this, SLOT(onResetTriggered()));
    connect(m_muteAction, SIGNAL(toggled(bool)), this, SLOT(onMuteToggled(bool)));
    connect(m_volumeSlider, SIGNAL(valueChanged(int)), this, SLOT(onVolumeChanged(int)));
}

void DashboardWindow::createStatusBarWidgets()
{
    m_connectionLabel = new QLabel("[ ] Disconnected", this);
    m_connectionLabel->setObjectName("connectionLabel");

    m_dataLabel = new QLabel("Speed 0.0 km/h  |  RPM 0  |  Fuel 60.00 L  |  Temp 80.0 C", this);
    m_dataLabel->setObjectName("dataLabel");

    statusBar()->addPermanentWidget(m_dataLabel, 1);
    statusBar()->addPermanentWidget(m_connectionLabel);
}

void DashboardWindow::applyWindowStyle()
{
    setStyleSheet(
        "QMainWindow { background-color: #03060b; }"
        "QWidget#dashboardCentral {"
        "  background-color: #000000;"
        "}"
        "QLabel#systemTitle {"
        "  color: #c8fbff;"
        "  font-family: Arial;"
        "  font-size: 26px;"
        "  font-weight: bold;"
        "  padding: 8px;"
        "}"
        "QLabel#sectionTitle { color: #51eaff; font-family: Arial; font-size: 13px; font-weight: bold; }"
        "QLabel#driveInfoLabel {"
        "  color: #7fa6ba;"
        "  font-family: Consolas;"
        "  font-size: 12px;"
        "  padding: 6px 8px;"
        "  background-color: rgba(3, 10, 18, 180);"
        "  border: 1px solid #1f3b52;"
        "  border-radius: 6px;"
        "}"
        "QFrame#centerPanel {"
        "  background: qlineargradient(x1:0,y1:0,x2:0,y2:1, stop:0 #0a1726, stop:0.55 #040b13, stop:1 #02060c);"
        "  border: 1px solid #27516c;"
        "  border-radius: 8px;"
        "}"
        "QMenuBar { background-color: #050b14; color: #dcecff; padding: 4px; border-bottom: 1px solid #172b3c; }"
        "QMenuBar::item:selected { background-color: #12324a; border-radius: 4px; }"
        "QMenu { background-color: #101b2a; color: #e5f4ff; border: 1px solid #2d4b68; }"
        "QMenu::item { padding: 8px 28px 8px 14px; }"
        "QMenu::item:selected { background-color: #1d5275; }"
        "QStatusBar { background-color: #02060b; color: #92b6d0; border-top: 1px solid #1a3c55; }"
        "QLabel#connectionLabel { color: #ff6b6b; font-weight: bold; padding: 2px 10px; }"
        "QLabel#dataLabel { color: #83dcff; font-family: Consolas; padding: 2px 10px; }"
        "QSlider::groove:horizontal { height: 5px; background: #26384a; border-radius: 2px; }"
        "QSlider::handle:horizontal { width: 14px; margin: -5px 0; border-radius: 7px; background: #42c8ff; }"
    );
}

void DashboardWindow::connectModules()
{
    connect(m_networkClient, SIGNAL(vehicleDataChanged(VehicleData)),
            this, SLOT(onVehicleDataChanged(VehicleData)));
    connect(m_networkClient, SIGNAL(connectionStateChanged(bool)),
            this, SLOT(onConnectionStateChanged(bool)));
    connect(m_networkClient, SIGNAL(networkError(QString)),
            this, SLOT(onNetworkError(QString)));
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
    m_imageDashboard->setVehicleData(data);

    updateWarningStates(data);
    updateWarningSound(data);

    m_dataLabel->setText(QString("Speed %1 km/h  |  RPM %2  |  Fuel %3 L  |  Temp %4 C")
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
    if(m_imageDashboard != NULL)
        m_imageDashboard->setConnected(connected);
    setConnectionVisualState(connected);

    statusBar()->showMessage(connected
                             ? "Connected to vehicle signal simulator."
                             : "Disconnected. Reconnecting every 3 seconds.",
                             3000);
}

void DashboardWindow::setConnectionVisualState(bool connected)
{
    if(connected)
    {
        m_connectionLabel->setText("[OK] Connected");
        m_connectionLabel->setStyleSheet("color:#51e58b;font-weight:bold;padding:2px 10px;");
    }
    else
    {
        m_connectionLabel->setText("[ ] Disconnected");
        m_connectionLabel->setStyleSheet("color:#ff6b6b;font-weight:bold;padding:2px 10px;");
    }
}

void DashboardWindow::onNetworkError(const QString &message)
{
    statusBar()->showMessage("Network: " + message, 5000);
}

void DashboardWindow::sendCommand(const QString &command)
{
    if(!m_connected)
    {
        QMessageBox::information(this, "Network", "Dashboard is not connected to the simulator.");
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
    if(checked)
        m_turnOffAction->setChecked(true);

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
    statusBar()->showMessage(checked ? "Warning sound muted." : "Warning sound enabled.", 2500);
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
    statusBar()->showMessage("Testing warning: " + action->text(), 3000);
}
