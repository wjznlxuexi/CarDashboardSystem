#ifndef RPMGAUGEWIDGET_H
#define RPMGAUGEWIDGET_H

#include <QWidget>

class QTimer;
class QPainter;

class RpmGaugeWidget : public QWidget
{
    Q_OBJECT

public:
    explicit RpmGaugeWidget(QWidget *parent = NULL);

public slots:
    void setTargetRpm(int rpm);

protected:
    void paintEvent(QPaintEvent *event) override;

private slots:
    void updateAnimation();

private:
    double valueToAngle(double value) const;
    void drawGauge(QPainter &painter, int radius);

    int m_targetRpm;
    double m_displayRpm;
    QTimer *m_animationTimer;
};

#endif // RPMGAUGEWIDGET_H
