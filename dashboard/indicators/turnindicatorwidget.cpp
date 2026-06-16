#include "turnindicatorwidget.h"

#include <QPainter>
#include <QTimer>
#include <QtGlobal>

TurnIndicatorWidget::TurnIndicatorWidget(QWidget *parent)
    : QWidget(parent),
      m_signal(0),
      m_hazard(false),
      m_blinkOn(false),
      m_blinkTimer(new QTimer(this))
{
    setMinimumSize(180, 74);
    m_blinkTimer->setInterval(500);
    connect(m_blinkTimer, SIGNAL(timeout()), this, SLOT(updateBlinkState()));
    m_blinkTimer->start();
}

void TurnIndicatorWidget::setTurnSignal(int signal)
{
    m_signal = qBound(0, signal, 2);
    update();
}

void TurnIndicatorWidget::setHazardEnabled(bool enabled)
{
    m_hazard = enabled;
    update();
}

void TurnIndicatorWidget::updateBlinkState()
{
    m_blinkOn = !m_blinkOn;
    update();
}

void TurnIndicatorWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillRect(rect(), QColor(5, 10, 18));

    const QRect leftRect(8, 6, width() / 2 - 14, height() - 12);
    const QRect rightRect(width() / 2 + 6, 6, width() / 2 - 14, height() - 12);
    const bool leftActive = m_blinkOn && (m_hazard || m_signal == 1);
    const bool rightActive = m_blinkOn && (m_hazard || m_signal == 2);

    drawArrow(painter, leftRect, true, leftActive);
    drawArrow(painter, rightRect, false, rightActive);
}

void TurnIndicatorWidget::drawArrow(QPainter &painter, const QRect &rect, bool left, bool active)
{
    painter.save();

    const QColor fill = active ? QColor(76, 255, 118) : QColor(32, 67, 45);
    const QColor border = active ? QColor(180, 255, 199) : QColor(50, 96, 67);
    painter.setPen(QPen(border, 2));
    painter.setBrush(fill);

    QRect r = rect.adjusted(6, 8, -6, -8);
    if(left)
    {
        QPolygon arrow;
        arrow << QPoint(r.left(), r.center().y())
              << QPoint(r.left() + r.width() / 3, r.top())
              << QPoint(r.left() + r.width() / 3, r.top() + r.height() / 3)
              << QPoint(r.right(), r.top() + r.height() / 3)
              << QPoint(r.right(), r.bottom() - r.height() / 3)
              << QPoint(r.left() + r.width() / 3, r.bottom() - r.height() / 3)
              << QPoint(r.left() + r.width() / 3, r.bottom());
        painter.drawPolygon(arrow);
    }
    else
    {
        QPolygon arrow;
        arrow << QPoint(r.right(), r.center().y())
              << QPoint(r.right() - r.width() / 3, r.top())
              << QPoint(r.right() - r.width() / 3, r.top() + r.height() / 3)
              << QPoint(r.left(), r.top() + r.height() / 3)
              << QPoint(r.left(), r.bottom() - r.height() / 3)
              << QPoint(r.right() - r.width() / 3, r.bottom() - r.height() / 3)
              << QPoint(r.right() - r.width() / 3, r.bottom());
        painter.drawPolygon(arrow);
    }

    painter.restore();
}
