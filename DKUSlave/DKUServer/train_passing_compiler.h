#ifndef TRAIN_PASSING_COMPILER_H
#define TRAIN_PASSING_COMPILER_H

#include <QObject>
#include "events_data.h"
#include <memory>
#include <functional>

class train_passing_compiler : public QObject
{
    Q_OBJECT
public:
    explicit train_passing_compiler(std::shared_ptr<train_passing_event> train_passing, QObject *parent = nullptr);
    void run();
private:
    std::shared_ptr<train_passing_event> train_passing;
    float u;
    int wagon_count;
    int axis_count;
    void par_calc();
    float time_calc(float s0);
    sensor sensor;
signals:
    void axel_passed(float speed, train_passing_event::direction_type direction);
    void train_pass_finished(); // сигнал прохода поезда против наложения событий
    void wheel_zone1_enter();
    void wheel_zone1_exit();
    void wheel_zone0_enter();
    void wheel_zone0_exit();
private slots:
    void on_timer_finish();
    void on_zone_passing(int curr_zone, float a, bool entering);
};

#endif // TRAIN_PASSING_COMPILER_H
