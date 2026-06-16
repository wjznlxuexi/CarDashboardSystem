#ifndef RPMGAUGEWIDGET_H
#define RPMGAUGEWIDGET_H

#include <QPainter>
#include <QWidget>
#include <QTimer>
#include <QLabel>

class RpmGaugeWidget : public QWidget
{
    Q_OBJECT
public:
    explicit RpmGaugeWidget(QWidget *parent = NULL);

public slots:
    void setTargetRpm(int rpm);

protected:
    void paintEvent(QPaintEvent *event);

private slots:
    void updateAnimation();

private:
    void drawDialBackground(QPainter &painter);//背景
    void drawScaleLines(QPainter &painter);//刻度线
    void drawScaleNumbers(QPainter &painter);//数字标签
    void drawNeedle(QPainter &painter,int rpm);//指针
    void drawCenterDot(QPainter &painter);//中心圆点
    void drawRpmLabel(QPainter &painter);//转速数字标签
    double rpmToAngle(int rpm) const;//转速转为角度
    double degreesToRadians(double degrees) const;//角度转为弧度

    int m_targetRpm;
    double m_displayRpm;
    QTimer *m_animationTimer;

    double DIAL_RADIUS;//表盘半径
    double NEEDLE_LENGTH;//指针长度
    static const int DIAL_START_ANGLE = -210;//表盘起始角度
    static const int DIAL_SPAN_ANGLE = 240;//表盘跨度
    static const int SCALE_COUNT = 16;//刻度数量，每10一格
    //中心点坐标
    double m_cx;
    double m_cy;
};

#endif // RPMGAUGEWIDGET_H
