#include "warninglightswidget.h"

#include <QGridLayout>
#include <QLabel>
#include <QPainter>

WarningLightsWidget::WarningLightsWidget(QWidget *parent)
    : QWidget(parent)
{
    for(int i = 0; i < Warning_Count; ++i)
    {
        m_labels[i] = NULL;
        m_activeStates[i] = false;
    }

    loadWarningImages();

    QGridLayout *layout = new QGridLayout(this);
    layout->setContentsMargins(6, 6, 6, 6);
    layout->setSpacing(8);

    for(int i = 0; i < Warning_Count; ++i)
    {
        m_labels[i] = new QLabel(this);
        m_labels[i]->setAlignment(Qt::AlignCenter);
        m_labels[i]->setFixedSize(54, 54);
        m_labels[i]->setStyleSheet("background-color: rgba(2, 8, 14, 180); border: 1px solid #21384c; border-radius: 6px;");

        layout->addWidget(m_labels[i], i / 4, i % 4);
        updateWarningPixmap(i);
    }
}

void WarningLightsWidget::setWarningActive(int warningType, bool active)
{
    if(warningType < 0 || warningType >= Warning_Count)
        return;

    if(m_activeStates[warningType] == active)
        return;

    m_activeStates[warningType] = active;
    updateWarningPixmap(warningType);
}

void WarningLightsWidget::clearAllWarnings()
{
    for(int i = 0; i < Warning_Count; ++i)
    {
        m_activeStates[i] = false;
        updateWarningPixmap(i);
    }
}

void WarningLightsWidget::loadWarningImages()
{
    const char *resourcePaths[Warning_Count] = {
        ":/images/warning_speed.png",
        ":/images/warning_oil.png",
        ":/images/warning_battery.png",
        ":/images/warning_temp.png",
        ":/images/warning_fuel.png",
        ":/images/warning_brake.png",
        ":/images/warning_airbag.png",
        ":/images/warning_abs.png"
    };

    for(int i = 0; i < Warning_Count; ++i)
    {
        m_originalPixmaps[i] = QPixmap(resourcePaths[i]).scaled(
                    40, 40, Qt::KeepAspectRatio, Qt::SmoothTransformation);

        if(!m_originalPixmaps[i].isNull())
            m_dimmedPixmaps[i] = createDimmedPixmap(m_originalPixmaps[i]);
    }
}

void WarningLightsWidget::updateWarningPixmap(int warningType)
{
    if(m_labels[warningType] == NULL)
        return;

    const QPixmap &pixmap = m_activeStates[warningType]
            ? m_originalPixmaps[warningType]
            : m_dimmedPixmaps[warningType];

    if(!pixmap.isNull())
    {
        m_labels[warningType]->setPixmap(pixmap);
        m_labels[warningType]->setText(QString());
    }
    else
    {
        static const char *names[Warning_Count] = {
            "SPD", "OIL", "BAT", "TMP", "FUL", "BRK", "AIR", "ABS"
        };
        m_labels[warningType]->setPixmap(QPixmap());
        m_labels[warningType]->setText(names[warningType]);
        m_labels[warningType]->setStyleSheet(
                    m_activeStates[warningType]
                    ? "color:#ff6b5f; font-weight:bold; background-color:#240b0b; border:1px solid #ff6b5f; border-radius:6px;"
                    : "color:#425363; font-weight:bold; background-color:rgba(2,8,14,180); border:1px solid #21384c; border-radius:6px;");
    }
}

QPixmap WarningLightsWidget::createDimmedPixmap(const QPixmap &source)
{
    QPixmap dimmed(source.size());
    dimmed.fill(Qt::transparent);

    QPainter painter(&dimmed);
    painter.setOpacity(0.22);
    painter.drawPixmap(0, 0, source);
    painter.end();

    return dimmed;
}
