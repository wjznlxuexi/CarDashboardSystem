#include "rpmgaugewidget.h"

#include <QPaintEvent>
#include <QPen>
#include <QBrush>
#include <QFont>
#include <QtMath>

RpmGaugeWidget::RpmGaugeWidget(QWidget *parent) :
    QWidget(parent)
  , m_targetRpm(0)
  , m_displayRpm(0)
  , m_cx(200)
  , m_cy(200)
{
    //初始化
    m_targetRpm = m_displayRpm;
    DIAL_RADIUS = width() * 0.3;
    NEEDLE_LENGTH = DIAL_RADIUS * 0.8;
    m_cx = width() / 2;
    m_cy = height() / 2 + 20;

    //创建定时器
    m_animationTimer = new QTimer(this);
    connect(m_animationTimer, &QTimer::timeout,this,&RpmGaugeWidget::updateAnimation);
    m_animationTimer->start(10);
}

void RpmGaugeWidget::setTargetRpm(int rpm)
{
    m_targetRpm = rpm;
}

void RpmGaugeWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    //启用抗锯齿
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    //更新
    DIAL_RADIUS = width() * 0.3;
    NEEDLE_LENGTH = DIAL_RADIUS * 0.8;
    m_cx = width() / 2.0;
    m_cy = height() / 2.0 + 20;
    //绘制表盘
    drawDialBackground(painter);//背景
    drawScaleLines(painter);//刻度线
    drawScaleNumbers(painter);//数字标签
    drawNeedle(painter,m_displayRpm);//指针
    drawCenterDot(painter);//中心圆点
    drawRpmLabel(painter);//速度数字标签
}

void RpmGaugeWidget::updateAnimation()
{
    // 平滑步长：每10ms最大变化 15 （可根据需要调整）
    const double STEP = 15.0;

    if (qAbs(m_targetRpm - m_displayRpm) <= STEP) {
        m_displayRpm = m_targetRpm;   // 直接到达目标
    } else if (m_displayRpm < m_targetRpm) {
        m_displayRpm += STEP;            // 增加
    } else {
        m_displayRpm -= STEP;            // 减少
    }

    update();  // 触发重绘
}

void RpmGaugeWidget::drawDialBackground(QPainter &painter)
{
    //设置画笔，深蓝色背景，无边框
    QPen pen;
    pen.setColor(QColor(20,40,80));
    pen.setWidth(30);
    painter.setPen(pen);
    //Qt中角度以1/16度为单位
    int startAngle = DIAL_START_ANGLE * 16;
    int spanAngle = DIAL_SPAN_ANGLE * 16;
    //计算外框矩形
    int x = m_cx - DIAL_RADIUS;
    int y = m_cy - DIAL_RADIUS;
    int size = DIAL_RADIUS * 2;

    //绘制圆弧
    painter.drawArc(x,y,size,size,-startAngle,-spanAngle);
    //绘制内圈装饰线
    pen.setColor(QColor(60,100,160));
    pen.setWidth(2);
    painter.setPen(pen);
    painter.drawArc(x+25,y+25,size-50,size-50,-startAngle,-spanAngle);
}

void RpmGaugeWidget::drawScaleLines(QPainter &painter)
{
    //刻度线数量：17个
    const int totalScales = SCALE_COUNT + 1;
    //角度增量：240度/8
    double angleStep = static_cast<double>(DIAL_SPAN_ANGLE) / (totalScales - 1);
    //设置画笔
    QPen pen;
    pen.setCapStyle(Qt::RoundCap);
    for(int i=0;i<totalScales;i++){
        //当前角度
        double angle = DIAL_START_ANGLE + i * angleStep;
        //是否为主刻度
        bool isMajor = (i % 2 == 0);
        //根据主次刻度设置线条长度和样式
        if(isMajor){
            //主刻度：较长
            pen.setWidth(3);
        }else{
            //次刻度：较短
            pen.setWidth(1);
        }
        if(i<12){
            pen.setColor(QColor("#00ffaa"));
        }else{
            pen.setColor(QColor("#ff4400"));
        }
        painter.setPen(pen);
        double radian = degreesToRadians(angle);
        //计算刻度线的起点和终点
        int innerRadius = DIAL_RADIUS - 20 - (isMajor ? 35 : 20);
        int outerRadius = DIAL_RADIUS - 25;
        //起始坐标
        int x1 = m_cx + outerRadius * qCos(radian);
        int y1 = m_cy + outerRadius * qSin(radian);
        //终点坐标
        int x2 = m_cx + innerRadius * qCos(radian);
        int y2 = m_cy + innerRadius * qSin(radian);
        //绘制刻度线
        painter.drawLine(x1,y1,x2,y2);
    }
}

void RpmGaugeWidget::drawScaleNumbers(QPainter &painter)
{
    //设置字体
    QFont font;
    font.setFamily("Arial");
    font.setPixelSize(36);
    font.setBold(true);
    painter.setFont(font);
    //设置画笔颜色
    QPen pen;
    pen.setColor(QColor("#00ff88"));
    pen.style();
    painter.setPen(pen);

    const int numbers[] = {0,1,2,3,4,5,6,7,8};
    const int count = sizeof(numbers) / sizeof(numbers[0]);
    //每个数字间的角度间隔
    double angleStep = static_cast<double>(DIAL_SPAN_ANGLE) / (count - 1);
    for(int i=0;i<count;i++){
        double angle = DIAL_START_ANGLE + i * angleStep;
        double radian = degreesToRadians(angle);
        //计算数字中心位置
        double textRadius,tx,ty;
        textRadius = DIAL_RADIUS - 70;
        tx = m_cx + textRadius * qCos(radian) - 10;
        ty = m_cy + textRadius * qSin(radian) - 15;
        //绘制数字
        QString text = QString::number(numbers[i]);
        painter.drawText(tx-15,ty+5,60,30,Qt::AlignCenter,text);
    }
}

void RpmGaugeWidget::drawNeedle(QPainter &painter, int rpm)
{
    double angle = rpmToAngle(rpm);
    QPen pen(QColor("#ff2200"));//红色
    pen.setWidth(3);
    pen.setCapStyle(Qt::RoundCap);
    painter.setPen(pen);
    //使用数学计算坐标直接划线
    double radian = degreesToRadians(angle);
    int tipX = m_cx + NEEDLE_LENGTH * qCos(radian);
    int tipY = m_cy + NEEDLE_LENGTH * qSin(radian);
    painter.drawLine(m_cx,m_cy,tipX,tipY);

    //绘制指针尾部
    pen.setWidth(2);
    pen.setColor(QColor("#ff2200"));
    painter.setPen(pen);
    //尾部画一小段
    int tailX = m_cx - (NEEDLE_LENGTH * 0.15) * qCos(radian);
    int tailY = m_cy - (NEEDLE_LENGTH * 0.15) * qSin(radian);
    painter.drawLine(m_cx,m_cy,tailX,tailY);
}

void RpmGaugeWidget::drawCenterDot(QPainter &painter)
{
    //绘制中心圆点（黑色圆底）
    QBrush brush(QColor(30,30,30));
    painter.setBrush(brush);
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(m_cx - 6,m_cy - 6,12,12);
    //绘制中心圆点（红色圆圈）
    QPen pen(QColor(255,80,80));
    pen.setWidth(3);
    painter.setPen(pen);
    painter.setBrush(Qt::NoBrush);
    painter.drawEllipse(m_cx - 6,m_cy - 6,12,12);
    //绘制中心小圆点（白色高光）
    brush.setColor(Qt::white);
    painter.setBrush(brush);
    painter.drawEllipse(m_cx - 2,m_cy - 2,4,4);
}

void RpmGaugeWidget::drawRpmLabel(QPainter &painter)
{
    //表盘下方显示当前速度
    QFont font;
    font.setFamily("Arial");
    font.setPixelSize(36);
    font.setBold(true);
    painter.setFont(font);
    QPen pen(QColor(255,255,255));
    painter.setPen(pen);
    //绘制速度数字
    QString rpmText = QString::number(m_displayRpm);
    painter.drawText(m_cx-40,m_cy+10,80,40,Qt::AlignCenter,rpmText);
    //绘制单位
    font.setPixelSize(12);
    painter.setFont(font);
    pen.setColor(QColor(150,150,150));
    painter.drawText(m_cx-20,m_cy+40,50,20,Qt::AlignCenter,"RPM");
}

double RpmGaugeWidget::rpmToAngle(int rpm) const
{
    return (rpm / 8000.0) * DIAL_SPAN_ANGLE + DIAL_START_ANGLE;
}

double RpmGaugeWidget::degreesToRadians(double degrees) const
{
    return degrees * M_PI / 180.0;
}
