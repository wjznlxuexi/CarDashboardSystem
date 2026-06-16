#include "speedometerwidget.h"

#include <QFontMetrics>
#include <QPainter>
#include <QTimer>
#include <QtGlobal>
#include <cmath>

SpeedometerWidget::SpeedometerWidget(QWidget *parent)
    : QWidget(parent),
      m_targetSpeed(0.0),
      m_displaySpeed(0.0),
      m_animationTimer(new QTimer(this))
{
    setMinimumSize(320, 320);
    setAttribute(Qt::WA_TranslucentBackground);
    m_animationTimer->setInterval(16);
    connect(m_animationTimer, SIGNAL(timeout()), this, SLOT(updateAnimation()));
    m_animationTimer->start();
}

void SpeedometerWidget::setTargetSpeed(double speed)
{
    m_targetSpeed = qBound(0.0, speed, 140.0);
}

void SpeedometerWidget::updateAnimation()
{
    const double diff = m_targetSpeed - m_displaySpeed;
    if(std::fabs(diff) < 0.05)
        m_displaySpeed = m_targetSpeed;
    else
        m_displaySpeed += diff * 0.12;

    update();
}

double SpeedometerWidget::valueToAngle(double value) const
{
    return -120.0 + qBound(0.0, value, 140.0) / 140.0 * 240.0;
}

void SpeedometerWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    const int side = qMin(width(), height());
    painter.translate(width() / 2, height() / 2);
    painter.scale(side / 360.0, side / 360.0);

    drawGauge(painter, 150);
}

void SpeedometerWidget::drawGauge(QPainter &painter, int radius)
{
    painter.save();

    painter.setPen(Qt::NoPen);
    QRadialGradient glow(0, 0, radius + 28);
    glow.setColorAt(0.70, QColor(10, 210, 255, 22));
    glow.setColorAt(0.86, QColor(10, 210, 255, 42));
    glow.setColorAt(1.00, QColor(10, 210, 255, 0));
    painter.setBrush(glow);
    painter.drawEllipse(QPoint(0, 0), radius + 28, radius + 28);

    QRadialGradient gradient(0, 0, radius + 6);
    gradient.setColorAt(0.0, QColor(21, 35, 50));
    gradient.setColorAt(0.58, QColor(8, 17, 29));
    gradient.setColorAt(0.86, QColor(2, 7, 14));
    gradient.setColorAt(1.0, QColor(0, 2, 6));
    painter.setBrush(gradient);
    painter.setPen(QPen(QColor(52, 101, 135), 2));
    painter.drawEllipse(QPoint(0, 0), radius, radius);

    painter.setBrush(Qt::NoBrush);
    painter.setPen(QPen(QColor(15, 230, 255, 140), 2));
    painter.drawEllipse(QPoint(0, 0), radius - 10, radius - 10);
    painter.setPen(QPen(QColor(4, 16, 27), 8));
    painter.drawEllipse(QPoint(0, 0), radius - 20, radius - 20);

    QRect arcRect(-radius + 22, -radius + 22, (radius - 22) * 2, (radius - 22) * 2);
    painter.setPen(QPen(QColor(21, 204, 245), 7, Qt::SolidLine, Qt::RoundCap));
    painter.drawArc(arcRect, 210 * 16, -178 * 16);
    painter.setPen(QPen(QColor(255, 76, 76), 7, Qt::SolidLine, Qt::RoundCap));
    painter.drawArc(arcRect, 32 * 16, -62 * 16);

    for(int value = 0; value <= 140; value += 10)
    {
        const bool major = value % 20 == 0;
        const double angle = valueToAngle(value);

        painter.save();
        painter.rotate(angle);

        const QColor tickColor = value >= 120 ? QColor(255, 78, 78) : QColor(125, 230, 255);
        painter.setPen(QPen(tickColor, major ? 4 : 2, Qt::SolidLine, Qt::RoundCap));
        painter.drawLine(0, -radius + 18, 0, -radius + (major ? 42 : 32));
        painter.restore();

        if(major)
        {
            const double radians = (angle - 90.0) * 3.14159265358979323846 / 180.0;
            const int textRadius = radius - 63;
            QPoint textCenter(static_cast<int>(std::cos(radians) * textRadius),
                              static_cast<int>(std::sin(radians) * textRadius));

            painter.setPen(value >= 120 ? QColor(255, 92, 92) : QColor(210, 243, 255));
            painter.setFont(QFont("Arial", 12, QFont::Bold));
            QRect textRect(textCenter.x() - 22, textCenter.y() - 12, 44, 24);
            painter.drawText(textRect, Qt::AlignCenter, QString::number(value));
        }
    }

    painter.save();
    painter.rotate(valueToAngle(m_displaySpeed));
    QPolygon needle;
    needle << QPoint(0, -116) << QPoint(-6, 9) << QPoint(0, 27) << QPoint(6, 9);
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(255, 54, 60));
    painter.drawPolygon(needle);
    painter.setPen(QPen(QColor(255, 156, 156, 150), 2));
    painter.drawLine(0, 22, 0, -112);
    painter.restore();

    painter.setBrush(QColor(235, 245, 255));
    painter.setPen(QPen(QColor(255, 80, 80), 3));
    painter.drawEllipse(QPoint(0, 0), 13, 13);

    QRect lcdRect(-78, 51, 156, 58);
    painter.setPen(QPen(QColor(35, 80, 105), 1));
    painter.setBrush(QColor(1, 8, 14, 190));
    painter.drawRoundedRect(lcdRect, 6, 6);

    painter.setPen(QColor(235, 248, 255));
    painter.setFont(QFont("Consolas", 34, QFont::Bold));
    painter.drawText(QRect(-80, 53, 160, 38), Qt::AlignCenter,
                     QString::number(static_cast<int>(m_displaySpeed + 0.5)));

    painter.setPen(QColor(117, 197, 220));
    painter.setFont(QFont("Arial", 12, QFont::Bold));
    painter.drawText(QRect(-80, 88, 160, 24), Qt::AlignCenter, "km/h");
    painter.setPen(QColor(105, 220, 246));
    painter.setFont(QFont("Arial", 15, QFont::Bold));
    painter.drawText(QRect(-80, -40, 160, 24), Qt::AlignCenter, "SPEED");

    painter.restore();
}
