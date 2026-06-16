#ifndef TURNINDICATORWIDGET_H
#define TURNINDICATORWIDGET_H

#include <QWidget>
#include <QTimer>
#include <QPixmap>

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
    int m_signal;
    bool m_hazard;
    bool m_blinkOn;
    QTimer *m_blinkTimer;
    QPixmap m_leftPixmap;
    QPixmap m_rightPixmap;
    QPixmap m_leftPixmapDimmed;
    QPixmap m_rightPixmapDimmed;
};

#endif

