#include "fuelgaugewidget.h"

#include <QPainter>
#include <QtGlobal>

FuelGaugeWidget::FuelGaugeWidget(QWidget *parent)
    : QWidget(parent),
      m_fuel(60.0),
      m_lowFuelActive(false)
{
    setMinimumSize(150, 170);
    setAttribute(Qt::WA_TranslucentBackground);
}

void FuelGaugeWidget::setFuelValue(double fuel)
{
    m_fuel = qBound(0.0, fuel, 60.0);

    const bool low = m_fuel < 15.0;
    if(low != m_lowFuelActive)
    {
        m_lowFuelActive = low;
        emit lowFuelStateChanged(low);
    }

    update();
}

double FuelGaugeWidget::valueToAngle(double value) const
{
    return -115.0 + qBound(0.0, value, 60.0) / 60.0 * 230.0;
}

void FuelGaugeWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    const int side = qMin(width(), height());
    painter.translate(width() / 2, height() / 2);
    painter.scale(side / 210.0, side / 210.0);

    drawGauge(painter, 88);
}

void FuelGaugeWidget::drawGauge(QPainter &painter, int radius)
{
    Q_UNUSED(radius);
    painter.save();

    QRectF body(-86, -88, 172, 176);
    painter.setPen(QPen(QColor(36, 75, 100), 1));
    painter.setBrush(QColor(4, 13, 22, 220));
    painter.drawRoundedRect(body, 8, 8);

    painter.setPen(QColor(119, 225, 250));
    painter.setFont(QFont("Arial", 13, QFont::Bold));
    painter.drawText(QRect(-70, -78, 140, 22), Qt::AlignCenter, "FUEL");

    QRectF tank(-58, -48, 34, 104);
    painter.setPen(QPen(QColor(31, 64, 84), 2));
    painter.setBrush(QColor(1, 7, 13));
    painter.drawRoundedRect(tank, 8, 8);

    const int segments = 10;
    const int activeSegments = qBound(0, static_cast<int>(m_fuel / 60.0 * segments + 0.5), segments);
    for(int i = 0; i < segments; ++i)
    {
        const int y = 45 - i * 9;
        QRectF segment(-51, y, 20, 6);
        const bool active = i < activeSegments;
        QColor color = QColor(24, 60, 73);
        if(active)
            color = m_lowFuelActive ? QColor(255, 177, 54) : QColor(57, 231, 143);

        painter.setPen(Qt::NoPen);
        painter.setBrush(color);
        painter.drawRoundedRect(segment, 2, 2);
    }

    painter.setPen(QColor(96, 135, 150));
    painter.setFont(QFont("Arial", 9, QFont::Bold));
    painter.drawText(QRect(-64, -58, 48, 16), Qt::AlignCenter, "FULL");
    painter.drawText(QRect(-64, 56, 48, 16), Qt::AlignCenter, "LOW");

    painter.setPen(m_lowFuelActive ? QColor(255, 177, 54) : QColor(237, 249, 255));
    painter.setFont(QFont("Consolas", 24, QFont::Bold));
    painter.drawText(QRect(-14, -18, 92, 34), Qt::AlignCenter,
                     QString("%1").arg(m_fuel, 0, 'f', 1));

    painter.setPen(QColor(117, 197, 220));
    painter.setFont(QFont("Arial", 11, QFont::Bold));
    painter.drawText(QRect(-14, 17, 92, 22), Qt::AlignCenter, "LITERS");

    painter.setPen(QPen(m_lowFuelActive ? QColor(255, 177, 54) : QColor(55, 231, 143), 2));
    painter.setBrush(Qt::NoBrush);
    painter.drawRoundedRect(QRectF(-73, -63, 146, 136), 7, 7);

    painter.restore();
}
