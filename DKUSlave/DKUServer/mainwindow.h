#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QModbusRtuSerialServer> // в верхнем уровне QModbusRtuSerialClient

#include <QMainWindow>
#include "dataemulator.h"
#include <chrono>
#include <ratio>
#include <QTimer>

using namespace std::chrono;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QModbusRtuSerialServer serv;
    DataEmulator Emulator;
    time_point<system_clock, duration<quint32,std::ratio<1,2000>>> startup_time;
    QTimer timer;

private slots:
    void onchangeaddress();
    void datarecieved(QModbusDataUnit::RegisterType table, int address, int size);
    //начинаются слоты для пользовательского интерфейса
    void on_Speed_Change(const QString &text);
    void on_Axis_Change(const QString &text);
    void on_r32_Request();
    void on_connect_clicked();
    void on_timer_tick();
};
#endif // MAINWINDOW_H
