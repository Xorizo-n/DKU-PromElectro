#include "dataemulator.h"
#include <QDebug>

DataEmulator::DataEmulator(QObject *parent)
    :QObject{parent},
     sensor_address{119}

{

}

void DataEmulator::setaddress(quint16 newaddress)
{
    if (newaddress <= 118 && newaddress >= 1)
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
