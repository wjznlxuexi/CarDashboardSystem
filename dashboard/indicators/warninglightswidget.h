#ifndef WARNINGLIGHTSWIDGET_H
#define WARNINGLIGHTSWIDGET_H

#include <QPixmap>
#include <QWidget>

#include "common/warningtype.h"

class QLabel;

class WarningLightsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit WarningLightsWidget(QWidget *parent = NULL);

public slots:
    void setWarningActive(int warningType, bool active);
    void clearAllWarnings();

private:
    void loadWarningImages();
    void updateWarningPixmap(int warningType);
    QPixmap createDimmedPixmap(const QPixmap &source);

    QLabel *m_labels[Warning_Count];
    QPixmap m_originalPixmaps[Warning_Count];
    QPixmap m_dimmedPixmaps[Warning_Count];
    bool m_activeStates[Warning_Count];
};

#endif // WARNINGLIGHTSWIDGET_H
