#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSerialPort>
#include <QDebug>
#include <QModbusDataUnit>
#include <QMap>
#include <limits>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
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
    connect(&Emulator, &DataEmulator::addresschanged, this, &MainWindow::onchangeaddress);
    connect(ui->speedEdit, &QLineEdit::textEdited, this, &MainWindow::on_Speed_Change);
    connect(ui->axis_amount, &QLineEdit::textEdited, this, &MainWindow::on_Axis_Change);
    Emulator.setaddress(1); //добавить реакцию на смену данных у 40025 регистра
}

void MainWindow::onchangeaddress()
{
    serv.setServerAddress(Emulator.getaddress());
}

void MainWindow::datarecieved(QModbusDataUnit::RegisterType table, int address, int size)
{
    quint16 n_adr;
    if(serv.data(table, address, &n_adr)) { Emulator.setaddress(n_adr); }
    else { qDebug() << "Not recieved!"; }
}

void MainWindow::on_Speed_Change(const QString &text)
{
    bool ok;
    int speedpar = text.toInt(&ok);
    if (ok)
    {
        speedpar = std::round(928.8/speedpar);
        serv.setData(QModbusDataUnit::HoldingRegisters,40031,speedpar);
        ui->speedEdit->setStyleSheet("border-width: 1px; border-style: solid; border-color: black;");
    }
    else
    {
        ui->speedEdit->setStyleSheet("border-width: 1px; border-style: solid; border-color: red;");
    }
}

void MainWindow::on_Axis_Change(const QString &text)
{
    bool ok;
    int axis = text.toInt(&ok);
    if (ok && axis <= std::numeric_limits<quint16>::max())
    {
        serv.setData(QModbusDataUnit::HoldingRegisters,40030,axis);
        ui->axis_amount->setStyleSheet("border-width: 1px; border-style: solid; border-color: black;");
    }
    else
    {
        ui->axis_amount->setStyleSheet("border-width: 1px; border-style: solid; border-color: red;");
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}


