#ifndef VEHICLEDATA_H
#define VEHICLEDATA_H

struct VehicleData
{
    double speed;
    int rpm;
    double fuel;
    double temperature;

    int turnSignal;
    bool hazard;

    bool absFault;
    bool airbagFault;
    bool batteryFault;
    bool brakeFault;
    bool speedFault;

    VehicleData()
    {
        speed = 0.0;
        rpm = 0;
        fuel = 60.0;
        temperature = 80.0;

        turnSignal = 0;
        hazard = false;

        absFault = false;
        airbagFault = false;
        batteryFault = false;
        brakeFault = false;
        speedFault = false;
    }
};

#endif // VEHICLEDATA_H
