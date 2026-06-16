#ifndef WARNINGLIGHTSWIDGET_H
#define WARNINGLIGHTSWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QPixmap>

enum WarningType
{
    Warning_Engine = 0,
    Warning_Oil,
    Warning_Battery,
    Warning_Temp,
    Warning_Fuel,
    Warning_Brake,
    Warning_Airbag,
    Warning_ABS
};

class WarningLightsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit WarningLightsWidget(QWidget *parent = NULL);

public slots:
    void setWarningActive(int warningType, bool active);
    void clearAllWarnings();

private:
    QLabel *m_labels[8];
    QPixmap m_originalPixmaps[8];
    QPixmap m_dimmedPixmaps[8];
    bool m_activeStates[8];

    void loadWarningImages();
    QPixmap createDimmedPixmap(const QPixmap &source);
};

#endif
