#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSerialPort>
#include <QDebug>
#include <QModbusDataUnit>
#include <QModbusReply>
#include <QModbusPdu>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    client.setConnectionParameter(QModbusDevice::SerialPortNameParameter,"COM4");
    client.setConnectionParameter(QModbusDevice::SerialParityParameter,QSerialPort::EvenParity);
    client.setConnectionParameter(QModbusDevice::SerialBaudRateParameter,38400);
    client.setConnectionParameter(QModbusDevice::SerialDataBitsParameter,QSerialPort::Data8);
    client.setConnectionParameter(QModbusDevice::SerialStopBitsParameter,QSerialPort::OneStop);
    if(client.connectDevice()) { qDebug() << "Connected"; }
    else {  qDebug() << "Device connection error"; }
    repl = client.sendReadRequest(QModbusDataUnit(QModbusDataUnit::HoldingRegisters,40030,1),0001);
    connect(repl,&QModbusReply::finished,this,&MainWindow::check);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::check()
{
    if (repl->error() == QModbusDevice::NoError)
    {
        auto vivod = repl->result();
        qDebug() << vivod.values();
    }
    else
    {
        qDebug() << repl->errorString();
        auto a = repl->rawResult();
        qDebug() << a.exceptionCode();
    }
    repl->deleteLater();
}

