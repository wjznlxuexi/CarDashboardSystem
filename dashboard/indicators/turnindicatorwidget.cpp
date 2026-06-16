#include "turnindicatorwidget.h"
#include <QPainter>

TurnIndicatorWidget::TurnIndicatorWidget(QWidget *parent)
    : QWidget(parent)
    , m_signal(0)
    , m_hazard(false)
    , m_blinkOn(false)
    , m_blinkTimer(new QTimer(this))
{
    m_leftPixmap = QPixmap(":/images/left.png");
    m_rightPixmap = QPixmap(":/images/right.png");

    if (!m_leftPixmap.isNull())
    {
        QPixmap dimmed(m_leftPixmap.size());
        dimmed.fill(Qt::transparent);
        QPainter p(&dimmed);
        p.setOpacity(0.25);
        p.drawPixmap(0, 0, m_leftPixmap);
        p.end();
        m_leftPixmapDimmed = dimmed;
    }

    if (!m_rightPixmap.isNull())
    {
        QPixmap dimmed(m_rightPixmap.size());
        dimmed.fill(Qt::transparent);
        QPainter p(&dimmed);
        p.setOpacity(0.25);
        p.drawPixmap(0, 0, m_rightPixmap);
        p.end();
        m_rightPixmapDimmed = dimmed;
    }

    m_blinkTimer->setInterval(500);
    connect(m_blinkTimer, SIGNAL(timeout()), this, SLOT(updateBlinkState()));
    m_blinkTimer->start();

    setMinimumSize(120, 60);
}

void TurnIndicatorWidget::setTurnSignal(int signal)
{
    if (signal < 0 || signal > 2)
        return;
    m_signal = signal;
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

void TurnIndicatorWidget::paintEvent(QPaintEvent * /*event*/)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);

    int w = width();
    int h = height();
    int halfW = w / 2;

    bool leftActive = m_blinkOn && (m_signal == 1 || m_hazard);
    bool rightActive = m_blinkOn && (m_signal == 2 || m_hazard);

    QRect leftRect(0, 0, halfW, h);
    QRect rightRect(halfW, 0, halfW, h);

    if (!m_leftPixmap.isNull())
    {
        QPixmap &leftImg = leftActive ? m_leftPixmap : m_leftPixmapDimmed;
        QSize imgSize = leftImg.size().scaled(leftRect.size(), Qt::KeepAspectRatio);
        int x = leftRect.x() + (leftRect.width() - imgSize.width()) / 2;
        int y = leftRect.y() + (leftRect.height() - imgSize.height()) / 2;
        painter.drawPixmap(x, y, imgSize.width(), imgSize.height(), leftImg);
    }

    if (!m_rightPixmap.isNull())
    {
        QPixmap &rightImg = rightActive ? m_rightPixmap : m_rightPixmapDimmed;
        QSize imgSize = rightImg.size().scaled(rightRect.size(), Qt::KeepAspectRatio);
        int x = rightRect.x() + (rightRect.width() - imgSize.width()) / 2;
        int y = rightRect.y() + (rightRect.height() - imgSize.height()) / 2;
        painter.drawPixmap(x, y, imgSize.width(), imgSize.height(), rightImg);
    }
}
