#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSerialPort>
#include <QDebug>
#include <QModbusDataUnit>
#include <QMap>
#include <limits>
#include <bitset>
#include <QSerialPortInfo>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    startup_time = time_point_cast<duration<quint32,std::ratio<1,2000>>>(system_clock::now());
    timer.start(1000);
    auto ports = QSerialPortInfo::availablePorts();
    for (const auto& port: ports)
    {
        ui->comports->addItem(port.portName());
    }
    serv.setConnectionParameter(QModbusDevice::SerialParityParameter,QSerialPort::EvenParity);
    serv.setConnectionParameter(QModbusDevice::SerialBaudRateParameter,38400);
    serv.setConnectionParameter(QModbusDevice::SerialDataBitsParameter,QSerialPort::Data8);
    serv.setConnectionParameter(QModbusDevice::SerialStopBitsParameter,QSerialPort::OneStop);
    QModbusDataUnit checkData(QModbusDataUnit::HoldingRegisters,29,8);
    QModbusDataUnitMap MyMap({{QModbusDataUnit::HoldingRegisters,checkData}});
    serv.setMap(MyMap);
    connect(ui->speedEdit, &QLineEdit::textEdited, this, &MainWindow::on_Speed_Change);
    connect(ui->axis_amount, &QLineEdit::textEdited, this, &MainWindow::on_Axis_Change);
    connect(ui->r32_0, &QCheckBox::stateChanged, this, &MainWindow::on_r32_Request);
    connect(ui->r32_1, &QCheckBox::stateChanged, this, &MainWindow::on_r32_Request);
    connect(ui->r32_2, &QCheckBox::stateChanged, this, &MainWindow::on_r32_Request);
    connect(ui->r32_3, &QCheckBox::stateChanged, this, &MainWindow::on_r32_Request);
    connect(ui->r32_4, &QCheckBox::stateChanged, this, &MainWindow::on_r32_Request);
    connect(ui->r32_8, &QCheckBox::stateChanged, this, &MainWindow::on_r32_Request);
    connect(ui->r32_9, &QCheckBox::stateChanged, this, &MainWindow::on_r32_Request);
    connect(ui->r32_10, &QCheckBox::stateChanged, this, &MainWindow::on_r32_Request);
    connect(ui->r32_11, &QCheckBox::stateChanged, this, &MainWindow::on_r32_Request);
    connect(ui->r32_12, &QCheckBox::stateChanged, this, &MainWindow::on_r32_Request);
    connect(ui->r32_13, &QCheckBox::stateChanged, this, &MainWindow::on_r32_Request);
    connect(ui->r32_14, &QCheckBox::stateChanged, this, &MainWindow::on_r32_Request);
    connect(ui->r32_15, &QCheckBox::stateChanged, this, &MainWindow::on_r32_Request);
    connect(&timer, &QTimer::timeout, this, &MainWindow::on_timer_tick);
}

void MainWindow::on_Speed_Change(const QString &text)
{
    bool ok;
    int speedpar = text.toInt(&ok);
    if (ok)
    {
        speedpar = std::round(928.8/speedpar);
        serv.setData(QModbusDataUnit::HoldingRegisters,30,speedpar);
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
        serv.setData(QModbusDataUnit::HoldingRegisters,29,axis);
        ui->axis_amount->setStyleSheet("border-width: 1px; border-style: solid; border-color: black;");
    }
    else
    {
        ui->axis_amount->setStyleSheet("border-width: 1px; border-style: solid; border-color: red;");
    }
}

void MainWindow::on_r32_Request()
{
    std::bitset<16> n_r32;
    if (ui->r32_0->isChecked()) { n_r32.set(0); }
    if (ui->r32_1->isChecked()) { n_r32.set(1); }
    if (ui->r32_2->isChecked()) { n_r32.set(2); }
    if (ui->r32_3->isChecked()) { n_r32.set(3); }
    if (ui->r32_4->isChecked()) { n_r32.set(4); }
    if (ui->r32_8->isChecked()) { n_r32.set(8); }
    if (ui->r32_9->isChecked()) { n_r32.set(9); }
    if (ui->r32_10->isChecked()) { n_r32.set(10); }
    if (ui->r32_11->isChecked()) { n_r32.set(11); }
    if (ui->r32_12->isChecked()) { n_r32.set(12); }
    if (ui->r32_13->isChecked()) { n_r32.set(13); }
    if (ui->r32_14->isChecked()) { n_r32.set(14); }
    if (ui->r32_15->isChecked()) { n_r32.set(15); }
    serv.setData(QModbusDataUnit::HoldingRegisters,31,n_r32.to_ulong());
}

MainWindow::~MainWindow()
{
    delete ui;
}




void MainWindow::on_connect_clicked()
{
    serv.setConnectionParameter(QModbusDevice::SerialPortNameParameter,ui->comports->currentText());
    serv.setServerAddress(ui->sen_adr->text().toInt());
    if(serv.connectDevice()) { qDebug() << "Connected"; }
    else { qDebug() << "Device connection error"; }
}

void MainWindow::on_timer_tick()
{
    time_point<system_clock, duration<quint32,std::ratio<1,2000>>> now = time_point_cast<duration<quint32,std::ratio<1,2000>>>(system_clock::now());
    auto active_time = now - startup_time;
    decltype(active_time)::rep temp = active_time.count();
    serv.setData(QModbusDataUnit::HoldingRegisters,35,temp & 0xffff);
    serv.setData(QModbusDataUnit::HoldingRegisters,36,temp>>16);
}

