#include "dashboardwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    DashboardWindow w;
    w.show();

    return a.exec();
}
