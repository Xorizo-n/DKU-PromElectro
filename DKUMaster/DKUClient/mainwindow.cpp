#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSerialPort>
#include <QDebug>
#include <QModbusDataUnit>
#include <QModbusReply>
#include <QModbusPdu>
#include <QTimer>
#include <ctime>

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
    if(client.connectDevice()) { qDebug() << "Connected!"; }
    else {  qDebug() << "Device connection error"; }
    //connect(&timer,&QTimer::timeout,this,&MainWindow::readrequest);
    buff_time = 1000;
    start_time = clock();
    QTimer::singleShot(buff_time,this,&MainWindow::readrequest);
    //timer.start(1000);
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
        finish_time = clock();
        //auto vivod = repl->result();
        qDebug() << repl->result().values();
        qDebug() << "Request complete in" << finish_time - start_time - std::clock_t(buff_time) << "ms"; //считает ping?
        QTimer::singleShot(buff_time,this,&MainWindow::readrequest);
        start_time = clock();
    }
    else
    {
        qDebug() << repl->errorString();
        //auto a = repl->rawResult();
        qDebug() << repl->rawResult().exceptionCode();
    }
    repl->deleteLater();
}

