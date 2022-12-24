#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QModbusRtuSerialServer> // в верхнем уровне QModbusRtuSerialClient

#include <QMainWindow>
#include "dataemulator.h"

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

private slots:
    void onchangeaddress();
    void datarecieved(QModbusDataUnit::RegisterType table, int address, int size);
    //начинаются слоты для пользовательского интерфейса
    void on_Speed_Change(const QString &text);
    void on_Axis_Change(const QString &text);
};
#endif // MAINWINDOW_H
