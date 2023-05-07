#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QModbusRtuSerialServer> // в верхнем уровне QModbusRtuSerialClient

#include <QMainWindow>
#include <chrono>
#include <ratio>
#include <QTimer>
#include "scriptdriver.h"
#include <QFileDialog>
#include "train_passing_compiler.h"

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
    std::unique_ptr<ScriptDriver> driver;
    std::unique_ptr<train_passing_compiler> train_passing_comp;
//    void trainPassing(int axis_amo, double v, double t, double a, bool direction);

private slots:
    //начинаются слоты для пользовательского интерфейса
    void on_Speed_Change(const QString &text);
    void on_Axis_Change(const QString &text);
    void on_r32_Request();
    void on_connect_clicked();
    void on_timer_tick();
    void on_initialize_clicked();
    void on_event_occured(std::shared_ptr<event_base> e_data);
    void on_script_finished();
    void on_choose_file_clicked();
    void on_axel_passing(float speed, train_passing_event::direction_type direction);
    void on_train_pass_finished(); // доделать
};
#endif // MAINWINDOW_H
