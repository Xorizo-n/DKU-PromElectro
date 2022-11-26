#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSerialPort>
#include <QDebug>
#include <QModbusDataUnit>
#include <QMap>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    serv.setServerAddress(0001);
    serv.setConnectionParameter(QModbusDevice::SerialPortNameParameter,"COM3");
    serv.setConnectionParameter(QModbusDevice::SerialParityParameter,QSerialPort::EvenParity);
    serv.setConnectionParameter(QModbusDevice::SerialBaudRateParameter,38400);
    serv.setConnectionParameter(QModbusDevice::SerialDataBitsParameter,QSerialPort::Data8);
    serv.setConnectionParameter(QModbusDevice::SerialStopBitsParameter,QSerialPort::OneStop);
    QModbusDataUnit checkData(QModbusDataUnit::HoldingRegisters,40025,13);
    QModbusDataUnitMap MyMap({{QModbusDataUnit::HoldingRegisters,checkData}});
    serv.setMap(MyMap);
    for (int i = 40025; i < 40038; i++)
    {
        qDebug() << serv.setData(QModbusDataUnit::HoldingRegisters,i,i);
    }
    if(serv.connectDevice()) { qDebug() << "Connected"; }
    else {  qDebug() << "Device connection error"; }
}

MainWindow::~MainWindow()
{
    delete ui;
}

