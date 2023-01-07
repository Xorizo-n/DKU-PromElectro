#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "qcheckbox.h"
#include <QMainWindow>
#include <QModbusRtuSerialClient>
#include <QModbusReply>
#include <QTimer>


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
    QModbusRtuSerialClient client;
    QModbusReply *repl;
    //QTimer timer;
    std::clock_t start_time;
    std::clock_t finish_time;
    int buff_time;
    std::vector<QCheckBox*> checks;

private slots:
    void readrequest();
    void replyread();
};
#endif // MAINWINDOW_H
