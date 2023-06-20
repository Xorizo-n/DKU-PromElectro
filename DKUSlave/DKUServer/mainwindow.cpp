#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSerialPort>
#include <QDebug>
#include <QModbusDataUnit>
#include <QMap>
#include <limits>
#include <bitset>
#include <QSerialPortInfo>
#include "events_data.h"
#include <QMessageBox>

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
    ui->initialize->setEnabled(0);
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
    if (ok && axis <= 1023 && axis >= 0)
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


void MainWindow::on_initialize_clicked()
{
    driver.reset(new ScriptDriver(ui->script_address->text()));
    connect(driver.get(), &ScriptDriver::event_occured, this, &MainWindow::on_event_occured);
    connect(driver.get(), &ScriptDriver::script_finished, this, &MainWindow::on_script_finished);
    driver -> run();
    ui->initialize->setEnabled(0);
    ui->choose_file->setEnabled(0);
}

void MainWindow::on_event_occured(std::shared_ptr<event_base> e_data)
{
    if (e_data->event_type == "reg_edit")
    {
        auto temp = std::static_pointer_cast<reg_edit_event>(e_data);
        for(auto t:temp->edited_registers)
        {
            serv.setData(QModbusDataUnit::HoldingRegisters,t.reg,t.new_value); // визуальный контроль добавить
        }
    }
    else if (e_data->event_type == "train_passing")
    {
        if (!train_passing_comp)
        {
            auto temp = std::static_pointer_cast<train_passing_event>(e_data);
            train_passing_comp.reset(new train_passing_compiler(temp));
            connect(train_passing_comp.get(), &train_passing_compiler::axel_passed, this, &MainWindow::on_axel_passing);
            connect(train_passing_comp.get(), &train_passing_compiler::train_pass_finished, this, &MainWindow::on_train_pass_finished);
            connect(train_passing_comp.get(), &train_passing_compiler::wheel_zone0_enter, this, &MainWindow::wheel_zone0_enter);
            connect(train_passing_comp.get(), &train_passing_compiler::wheel_zone1_enter, this, &MainWindow::wheel_zone1_enter);
            connect(train_passing_comp.get(), &train_passing_compiler::wheel_zone0_exit, this, &MainWindow::wheel_zone0_exit);
            connect(train_passing_comp.get(), &train_passing_compiler::wheel_zone1_exit, this, &MainWindow::wheel_zone1_exit);
            train_passing_comp -> run();
        }
        else
        {
            train_passing_comp.reset();
            driver.reset();
            QMessageBox::warning(this, "Ошибка!", "Произошло наложение событий прохождения поезда.");
        }
    }
}

void MainWindow::on_script_finished()
{
    driver.reset();
    ui->initialize->setEnabled(1);
    ui->choose_file->setEnabled(1);
}


void MainWindow::on_choose_file_clicked()
{
    ui->script_address->setText(QFileDialog::getOpenFileName(this, "Выберите файл скрипта...", QDir::currentPath()));
    if (ui->script_address->text() != "") { ui->initialize->setEnabled(1); }
    else { ui->initialize->setEnabled(0); }
}

void MainWindow::on_axel_passing(float speed, train_passing_event::direction_type direction)
{
    quint16 speed_parm = std::round(928.8/speed);
    serv.setData(QModbusDataUnit::HoldingRegisters,30,speed_parm);
    quint16 current_axis_amo;
    serv.data(QModbusDataUnit::HoldingRegisters,29,&current_axis_amo);
    quint16 r32_value;
    serv.data(QModbusDataUnit::HoldingRegisters,31,&r32_value);
    std::bitset<16> n_r32(r32_value);
    if (direction == train_passing_event::forward)
    {
        n_r32.set(3);
        n_r32.reset(4);
        if (current_axis_amo == 1023) { serv.setData(QModbusDataUnit::HoldingRegisters,29,0); }
        else { serv.setData(QModbusDataUnit::HoldingRegisters,29,current_axis_amo+1); }
    }
    else
    {
        n_r32.set(4);
        n_r32.reset(3);
        if (current_axis_amo == 0) { serv.setData(QModbusDataUnit::HoldingRegisters,29,1023); }
        else { serv.setData(QModbusDataUnit::HoldingRegisters,29,current_axis_amo-1); }
    }
    serv.setData(QModbusDataUnit::HoldingRegisters,31,n_r32.to_ulong());
}

void MainWindow::on_train_pass_finished()
{
    train_passing_comp.reset();
}

void MainWindow::wheel_zone1_enter()
{
    quint16 r32_value;
    serv.data(QModbusDataUnit::HoldingRegisters,31,&r32_value);
    std::bitset<16> n_r32(r32_value);
    n_r32.set(1);
    serv.setData(QModbusDataUnit::HoldingRegisters,31,n_r32.to_ulong());
}

void MainWindow::wheel_zone1_exit()
{
    quint16 r32_value;
    serv.data(QModbusDataUnit::HoldingRegisters,31,&r32_value);
    std::bitset<16> n_r32(r32_value);
    n_r32.reset(1);
    serv.setData(QModbusDataUnit::HoldingRegisters,31,n_r32.to_ulong());
}

void MainWindow::wheel_zone0_enter()
{
    quint16 r32_value;
    serv.data(QModbusDataUnit::HoldingRegisters,31,&r32_value);
    std::bitset<16> n_r32(r32_value);
    n_r32.set(0);
    serv.setData(QModbusDataUnit::HoldingRegisters,31,n_r32.to_ulong());
}

void MainWindow::wheel_zone0_exit()
{
    quint16 r32_value;
    serv.data(QModbusDataUnit::HoldingRegisters,31,&r32_value);
    std::bitset<16> n_r32(r32_value);
    n_r32.reset(0);
    serv.setData(QModbusDataUnit::HoldingRegisters,31,n_r32.to_ulong());
} // доделать остальные зоны


