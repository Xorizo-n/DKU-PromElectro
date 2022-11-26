#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSerialPort>
#include <QDebug>
#include <QModbusDataUnit>
#include <QModbusReply>
#include <QModbusPdu>
#include <QTimer>

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
    connect(&timer,&QTimer::timeout,this,&MainWindow::readrequest);
    timer.start(1000); // реализовать singleshot таймер, привязанный в получению ответа от сервера
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::readrequest()
{
    repl = client.sendReadRequest(QModbusDataUnit(QModbusDataUnit::HoldingRegisters,40025,13),0001);
    connect(repl,&QModbusReply::finished,this,&MainWindow::replyread);
}

void MainWindow::replyread()
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

