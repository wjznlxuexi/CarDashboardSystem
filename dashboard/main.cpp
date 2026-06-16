#include "dashboardwindow.h"
#include "common/vehicledata.h"

#include <QApplication>
#include <QCoreApplication>
#include <QFont>

int main(int argc, char *argv[])
{
    QApplication application(argc, argv);

    QCoreApplication::setOrganizationName("WuhanTextileUniversity");
    QCoreApplication::setApplicationName("CarDashboardSystem");
    QApplication::setFont(QFont("Microsoft YaHei", 10));

    qRegisterMetaType<VehicleData>("VehicleData");

    DashboardWindow window;
    window.show();

    return application.exec();
}
