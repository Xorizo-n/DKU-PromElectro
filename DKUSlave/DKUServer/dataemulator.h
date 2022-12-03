#ifndef DATAEMULATOR_H
#define DATAEMULATOR_H

#include <QObject>

class DataEmulator : public QObject
{
    Q_OBJECT
public:
    explicit DataEmulator(QObject *parent = nullptr);
    Q_PROPERTY(quint16 sensor_adr READ getaddress WRITE setaddress NOTIFY addresschanged) //возможны проблемы!
    void setaddress(quint16 newaddress);
    quint16 getaddress();
signals:
    void addresschanged();
private:
    quint16 sensor_address; //дописать код для остальных параметров из документации
};

#endif // DATAEMULATOR_H
