/********************************************************************************
** Form generated from reading UI file 'simulatorwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.3.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SIMULATORWINDOW_H
#define UI_SIMULATORWINDOW_H

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

class Ui_SimulatorWindow
{
public:
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QWidget *centralWidget;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *SimulatorWindow)
    {
        if (SimulatorWindow->objectName().isEmpty())
            SimulatorWindow->setObjectName(QStringLiteral("SimulatorWindow"));
        SimulatorWindow->resize(400, 300);
        menuBar = new QMenuBar(SimulatorWindow);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        SimulatorWindow->setMenuBar(menuBar);
        mainToolBar = new QToolBar(SimulatorWindow);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        SimulatorWindow->addToolBar(mainToolBar);
        centralWidget = new QWidget(SimulatorWindow);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        SimulatorWindow->setCentralWidget(centralWidget);
        statusBar = new QStatusBar(SimulatorWindow);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        SimulatorWindow->setStatusBar(statusBar);

        retranslateUi(SimulatorWindow);

        QMetaObject::connectSlotsByName(SimulatorWindow);
    } // setupUi

    void retranslateUi(QMainWindow *SimulatorWindow)
    {
        SimulatorWindow->setWindowTitle(QApplication::translate("SimulatorWindow", "SimulatorWindow", 0));
    } // retranslateUi

};

namespace Ui {
    class SimulatorWindow: public Ui_SimulatorWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SIMULATORWINDOW_H
