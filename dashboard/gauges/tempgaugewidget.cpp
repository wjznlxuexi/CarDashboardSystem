#include "tempgaugewidget.h"
#include <QPainter>
#include <QtMath>

TempGaugeWidget::TempGaugeWidget(QWidget *parent)
    : QWidget(parent), m_temperature(40.0)
{
    setMinimumSize(300, 350);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

void TempGaugeWidget::setTemperature(double temp)
{
    if (temp < 40) temp = 40;
    if (temp > 120) temp = 120;
    m_temperature = temp;
    emit highTemperatureStateChanged(m_temperature > 95.0);
    update();
}

void TempGaugeWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    int side = qMin(width(), height() - 30);
    painter.setViewport((width() - side) / 2, (height() - side - 30) / 2, side, side);
    painter.setWindow(0, 0, 200, 200);

    painter.setBrush(QColor(20,20,30));
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(10,10,180,180);

    painter.setPen(QPen(QColor(80,80,100),2));
    painter.setBrush(Qt::NoBrush);
    painter.drawEllipse(10,10,180,180);

    // 角度定义：40℃=135°，120℃=405°
    const double startAngle = 135.0;
    const double endAngle   = 405.0;
    const double range = endAngle - startAngle;

    QFont font = painter.font();
    font.setPointSize(11);
    painter.setFont(font);
    painter.setPen(QPen(Qt::white, 2));

    int mainTicks[] = {40, 60, 80, 100, 120};
    for (int temp : mainTicks)
    {
        double ratio = (temp - 40.0) / 80.0;
        double angle = startAngle + ratio * range;
        double normAngle = fmod(angle, 360.0);
        double rad = normAngle * M_PI / 180.0;
        double dx = cos(rad);
        double dy = sin(rad);

        int inner = 80;
        int outer = 94;
        int x1 = 100 + inner * dx;
        int y1 = 100 + inner * dy;
        int x2 = 100 + outer * dx;
        int y2 = 100 + outer * dy;
        painter.drawLine(x1, y1, x2, y2);

        int textRadius = 62;
        int xText = 100 + textRadius * dx;
        int yText = 100 + textRadius * dy;
        QString text = QString::number(temp);
        QRect rect(xText - 13, yText - 10, 26, 20);
        painter.drawText(rect, Qt::AlignCenter, text);
    }

    int subTicks[] = {50,70,90,110};
    painter.setPen(QPen(Qt::white,1));
    for (int temp : subTicks)
    {
        double ratio = (temp - 40.0) / 80.0;
        double angle = startAngle + ratio * range;
        double normAngle = fmod(angle, 360.0);
        double rad = normAngle * M_PI / 180.0;
        double dx = cos(rad);
        double dy = sin(rad);
        int inner = 80;
        int outer = 88;
        int x1 = 100 + inner * dx;
        int y1 = 100 + inner * dy;
        int x2 = 100 + outer * dx;
        int y2 = 100 + outer * dy;
        painter.drawLine(x1, y1, x2, y2);
    }

    // 指针
    double ratio = (m_temperature - 40.0) / 80.0;
    double angle = startAngle + ratio * range;
    double normAngle = fmod(angle, 360.0);
    double rad = normAngle * M_PI / 180.0;
    double dx = cos(rad);
    double dy = sin(rad);
    int tipX = 100 + 50 * dx;
    int tipY = 100 + 50 * dy;
    painter.setPen(QPen(Qt::red,4));
    painter.drawLine(100,100,tipX,tipY);
    painter.setBrush(Qt::white);
    painter.drawEllipse(96,96,8,8);
    painter.setBrush(QColor(60,60,80));
    painter.drawEllipse(94,94,12,12);

    // 在表盘内部六点钟方向绘制数值框（缩小版）
    painter.setPen(QPen(Qt::white, 1));
    painter.setBrush(Qt::white);
    QRectF textRect(83, 162, 34, 20);
    painter.drawRoundedRect(textRect, 3, 3);
    painter.setPen(Qt::black);
    QFont valueFont;
    valueFont.setPointSize(8);
    painter.setFont(valueFont);
    painter.drawText(textRect, Qt::AlignCenter, QString::number(m_temperature, 'f', 0) + "℃");
}
