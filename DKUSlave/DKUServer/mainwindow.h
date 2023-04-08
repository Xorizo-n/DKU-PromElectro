#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QModbusRtuSerialServer> // в верхнем уровне QModbusRtuSerialClient

#include <QMainWindow>
#include <chrono>
#include <ratio>
#include <QTimer>
#include "scriptdriver.h"

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
    time_point<system_clock, duration<quint32,std::ratio<1,2000>>> startup_time;
    QTimer timer;
    ScriptDriver *driver;
//    void trainPassing(int axis_amo, double v, double t, double a, bool direction);

private slots:
    //начинаются слоты для пользовательского интерфейса
    void on_Speed_Change(const QString &text);
    void on_Axis_Change(const QString &text);
    void on_r32_Request();
    void on_connect_clicked();
    void on_timer_tick();
    void on_initialize_clicked();
};
#endif // MAINWINDOW_H
