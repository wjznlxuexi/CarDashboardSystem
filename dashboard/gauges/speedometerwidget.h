#ifndef SPEEDOMETERWIDGET_H
#define SPEEDOMETERWIDGET_H

#include <QWidget>

class QTimer;
class QPainter;

class SpeedometerWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SpeedometerWidget(QWidget *parent = NULL);

public slots:
    void setTargetSpeed(double speed);

protected:
    void paintEvent(QPaintEvent *event) override;

private slots:
    void updateAnimation();

private:
    double valueToAngle(double value) const;
    void drawGauge(QPainter &painter, int radius);

    double m_targetSpeed;
    double m_displaySpeed;
    QTimer *m_animationTimer;
};

#endif // SPEEDOMETERWIDGET_H
