#ifndef TURNINDICATORWIDGET_H
#define TURNINDICATORWIDGET_H

#include <QWidget>

class QTimer;
class QPainter;

class TurnIndicatorWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TurnIndicatorWidget(QWidget *parent = NULL);

public slots:
    void setTurnSignal(int signal);
    void setHazardEnabled(bool enabled);

private slots:
    void updateBlinkState();

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    void drawArrow(QPainter &painter, const QRect &rect, bool left, bool active);

    int m_signal;
    bool m_hazard;
    bool m_blinkOn;
    QTimer *m_blinkTimer;
};

#endif // TURNINDICATORWIDGET_H
