#include "simulatorwindow.h"
#include "common/vehicledata.h"

#include <QApplication>
#include <QCoreApplication>
#include <QFont>

int main(int argc, char *argv[])
{
    QApplication application(argc, argv);

    QCoreApplication::setOrganizationName("WuhanTextileUniversity");
    QCoreApplication::setApplicationName("VehicleSignalSimulator");
    QApplication::setFont(QFont("Microsoft YaHei", 10));

    qRegisterMetaType<VehicleData>("VehicleData");

    SimulatorWindow window;
    window.show();

    return application.exec();
}
