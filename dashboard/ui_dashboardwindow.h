/********************************************************************************
** Form generated from reading UI file 'dashboardwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.3.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DASHBOARDWINDOW_H
#define UI_DASHBOARDWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_DashboardWindow
{
public:
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QWidget *centralWidget;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *DashboardWindow)
    {
        if (DashboardWindow->objectName().isEmpty())
            DashboardWindow->setObjectName(QStringLiteral("DashboardWindow"));
        DashboardWindow->resize(400, 300);
        menuBar = new QMenuBar(DashboardWindow);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        DashboardWindow->setMenuBar(menuBar);
        mainToolBar = new QToolBar(DashboardWindow);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        DashboardWindow->addToolBar(mainToolBar);
        centralWidget = new QWidget(DashboardWindow);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        DashboardWindow->setCentralWidget(centralWidget);
        statusBar = new QStatusBar(DashboardWindow);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        DashboardWindow->setStatusBar(statusBar);

        retranslateUi(DashboardWindow);

        QMetaObject::connectSlotsByName(DashboardWindow);
    } // setupUi

    void retranslateUi(QMainWindow *DashboardWindow)
    {
        DashboardWindow->setWindowTitle(QApplication::translate("DashboardWindow", "DashboardWindow", 0));
    } // retranslateUi

};

namespace Ui {
    class DashboardWindow: public Ui_DashboardWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DASHBOARDWINDOW_H
