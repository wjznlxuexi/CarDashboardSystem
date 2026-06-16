#include "warninglightswidget.h"
#include <QGridLayout>
#include <QPainter>

WarningLightsWidget::WarningLightsWidget(QWidget *parent)
    : QWidget(parent)
{
    for (int i = 0; i < 8; ++i)
        m_activeStates[i] = false;

    loadWarningImages();

    QGridLayout *layout = new QGridLayout(this);
    layout->setSpacing(8);

    for (int i = 0; i < 8; ++i)
    {
        m_labels[i] = new QLabel(this);
        m_labels[i]->setAlignment(Qt::AlignCenter);
        m_labels[i]->setMinimumSize(64, 64);
        m_labels[i]->setScaledContents(false);

        int row = i / 4;
        int col = i % 4;
        layout->addWidget(m_labels[i], row, col);

        if (!m_dimmedPixmaps[i].isNull())
            m_labels[i]->setPixmap(m_dimmedPixmaps[i]);
    }

    setLayout(layout);
}

void WarningLightsWidget::setWarningActive(int warningType, bool active)
{
    if (warningType < 0 || warningType > 7)
        return;

    m_activeStates[warningType] = active;

    if (active && !m_originalPixmaps[warningType].isNull())
        m_labels[warningType]->setPixmap(m_originalPixmaps[warningType]);
    else if (!m_dimmedPixmaps[warningType].isNull())
        m_labels[warningType]->setPixmap(m_dimmedPixmaps[warningType]);
}

void WarningLightsWidget::clearAllWarnings()
{
    for (int i = 0; i < 8; ++i)
    {
        m_activeStates[i] = false;
        if (!m_dimmedPixmaps[i].isNull())
            m_labels[i]->setPixmap(m_dimmedPixmaps[i]);
    }
}

void WarningLightsWidget::loadWarningImages()
{
    const char *resourcePaths[8] = {
        ":/images/warning_speed.png",
        ":/images/warning_oil.png",
        ":/images/warning_battery.png",
        ":/images/warning_temp.png",
        ":/images/warning_fuel.png",
        ":/images/warning_brake.png",
        ":/images/warning_airbag.png",
        ":/images/warning_abs.png"
    };

    for (int i = 0; i < 8; ++i)
    {
        m_originalPixmaps[i] = QPixmap(resourcePaths[i]);
        if (!m_originalPixmaps[i].isNull())
            m_dimmedPixmaps[i] = createDimmedPixmap(m_originalPixmaps[i]);
    }
}

QPixmap WarningLightsWidget::createDimmedPixmap(const QPixmap &source)
{
    QPixmap dimmed(source.size());
    dimmed.fill(Qt::transparent);

    QPainter painter(&dimmed);
    painter.setOpacity(0.25);
    painter.drawPixmap(0, 0, source);
    painter.end();

    return dimmed;
}
