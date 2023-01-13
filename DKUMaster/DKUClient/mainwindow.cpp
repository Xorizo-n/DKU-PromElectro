#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSerialPort>
#include <QDebug>
#include <QModbusDataUnit>
#include <QModbusReply>
#include <QModbusPdu>
#include <QTimer>
#include <ctime>
#include <bitset>
#include <vector>
#include <QSerialPortInfo>
#include <qcheckbox.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    auto ports = QSerialPortInfo::availablePorts();
    for (const auto& port: ports)
    {
        ui->comports->addItem(port.portName());
    }
    client.setConnectionParameter(QModbusDevice::SerialParityParameter,QSerialPort::EvenParity);
    client.setConnectionParameter(QModbusDevice::SerialBaudRateParameter,38400);
    client.setConnectionParameter(QModbusDevice::SerialDataBitsParameter,QSerialPort::Data8);
    client.setConnectionParameter(QModbusDevice::SerialStopBitsParameter,QSerialPort::OneStop);
    buff_time = 1000;
    checks.resize(16);
    checks[0] = ui->r32_0;
    checks[1] = ui->r32_1;
    checks[2] = ui->r32_2;
    checks[3] = ui->r32_3;
    checks[4] = ui->r32_4;
    checks[8] = ui->r32_8;
    checks[9] = ui->r32_9;
    checks[10] = ui->r32_10;
    checks[11] = ui->r32_11;
    checks[12] = ui->r32_12;
    checks[13] = ui->r32_13;
    checks[14] = ui->r32_14;
    checks[15] = ui->r32_15;
    for (int i = 0; i<5; i++)
    {
        checks[i]->setCheckable(0);
    }
    for (int i = 8; i<16; i++)
    {
        checks[i]->setCheckable(0);
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::readrequest()
{
    repl = client.sendReadRequest(QModbusDataUnit(QModbusDataUnit::HoldingRegisters,29,4),119);
    connect(repl, &QModbusReply::finished, this, &MainWindow::replyread);
}

void MainWindow::replyread()
{
    if (repl->error() == QModbusDevice::NoError)
    {
        finish_time = clock();
        QList registr = repl->result().values();
        qDebug() << "Request complete in" << finish_time - start_time - std::clock_t(buff_time) << "ms";
        QTimer::singleShot(buff_time, this, &MainWindow::readrequest);
        start_time = clock();
        quint16 axis = registr[0]; // rg30
        ui->axis_counter->setText(QString::number(axis));
        quint16 speed = registr[1]; // rg31
        float realspeed = 928.8/speed;
        ui->Speed_in->setText(QString::number(realspeed));
        ui->Speedom->setValue(realspeed);
        std::bitset<16> r32bits = std::bitset<16>(registr[2]); // rg32
        for (int i = 0; i<5; i++)
        {
            if (r32bits[i] == 1) { checks[i]->setChecked(1); }
            else { checks[i]->setChecked(0); }
        }
        for (int i = 8; i<16; i++)
        {
            if (r32bits[i] == 1) { checks[i]->setChecked(1); }
            else { checks[i]->setChecked(0); }
        }
    }
    else
    {
        qDebug() << repl->errorString();
        qDebug() << repl->rawResult().exceptionCode();
        QTimer::singleShot(buff_time, this, &MainWindow::readrequest);
    }
    repl->deleteLater();
}


void MainWindow::on_connect_clicked()
{
    client.setConnectionParameter(QModbusDevice::SerialPortNameParameter,ui->comports->currentText());
    if(client.connectDevice())
    {
        qDebug() << "Connected";
        start_time = clock();
        QTimer::singleShot(buff_time, this, &MainWindow::readrequest);
    }
    else {  qDebug() << "Device connection error"; }
}

