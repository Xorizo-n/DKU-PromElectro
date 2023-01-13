#include "dataemulator.h"
#include <QDebug>

DataEmulator::DataEmulator(QObject *parent)
    :QObject{parent},
      sensor_address{119}, axis{0}

{

}

void DataEmulator::setaddress(quint16 newaddress)
{
    if (newaddress <= 119 && newaddress >= 1)
    {
        sensor_address = newaddress;
        emit addresschanged();
    }
    else { qDebug() << "Address out of bounds"; }
}

quint16 DataEmulator::getaddress()
{
    return sensor_address;
}

quint16 DataEmulator::getaxis()
{
    return axis;
}

quint16 DataEmulator::getlastspeed()
{
    return last_speed;
}
