#include "fuelgaugewidget.h"
#include <QPainter>
#include <QtMath>

FuelGaugeWidget::FuelGaugeWidget(QWidget *parent)
    : QWidget(parent), m_fuel(0.0)
{
    setMinimumSize(300, 350);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

void FuelGaugeWidget::setFuelValue(double fuel)
{
    if (fuel < 0) fuel = 0;
    if (fuel > 60) fuel = 60;
    m_fuel = fuel;
    emit lowFuelStateChanged(m_fuel < 15.0);
    update();
}

void FuelGaugeWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    int side = qMin(width(), height() - 40);
    painter.setViewport((width() - side) / 2, 10, side, side);
    painter.setWindow(0, 0, 200, 200);

    painter.setBrush(QColor(20, 20, 30));
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(10, 10, 180, 180);

    painter.setPen(QPen(QColor(80, 80, 100), 2));
    painter.setBrush(Qt::NoBrush);
    painter.drawEllipse(10, 10, 180, 180);

    // 角度定义：0°=三点钟，顺时针增加。0L=135°，60L=405°
    const double startAngle = 135.0;
    const double endAngle   = 405.0;
    const double range = endAngle - startAngle; // 270°

    QFont font = painter.font();
    font.setPointSize(11);
    painter.setFont(font);
    painter.setPen(QPen(Qt::white, 2));

    for (int fuel = 0; fuel <= 60; fuel += 5)
    {
        double ratio = fuel / 60.0;
        double angle = startAngle + ratio * range;
        double normAngle = fmod(angle, 360.0);
        double rad = normAngle * M_PI / 180.0;
        double dx = cos(rad);
        double dy = sin(rad);

        int inner = 80;
        int outer = 86;
        if (fuel % 20 == 0) {
            outer = 92;
            // 数字半径68
            int xText = 100 + 68 * dx;
            int yText = 100 + 68 * dy;
            QString text = QString::number(fuel);
            QRect rect(xText - 10, yText - 10, 20, 20);
            painter.drawText(rect, Qt::AlignCenter, text);
        } else if (fuel % 10 == 0) {
            outer = 89;
        }
        int x1 = 100 + inner * dx;
        int y1 = 100 + inner * dy;
        int x2 = 100 + outer * dx;
        int y2 = 100 + outer * dy;
        painter.drawLine(x1, y1, x2, y2);
    }

    // 指针
    double ratio = m_fuel / 60.0;
    double angle = startAngle + ratio * range;
    double normAngle = fmod(angle, 360.0);
    double rad = normAngle * M_PI / 180.0;
    double dx = cos(rad);
    double dy = sin(rad);
    int tipX = 100 + 58 * dx;
    int tipY = 100 + 58 * dy;
    painter.setPen(QPen(Qt::red, 3));
    painter.drawLine(100, 100, tipX, tipY);
    painter.setBrush(Qt::white);
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(96, 96, 8, 8);
    painter.setBrush(QColor(60, 60, 80));
    painter.drawEllipse(94, 94, 12, 12);

    // 在表盘内部六点钟方向绘制数值框（缩小版）
    painter.setPen(QPen(Qt::white, 1));
    painter.setBrush(Qt::white);
    QRectF textRect(83, 162, 34, 20);   // 更小
    painter.drawRoundedRect(textRect, 3, 3);
    painter.setPen(Qt::black);
    QFont valueFont;
    valueFont.setPointSize(8);           // 字体更小
    painter.setFont(valueFont);
    painter.drawText(textRect, Qt::AlignCenter, QString::number(m_fuel, 'f', 0) + "L");
}
