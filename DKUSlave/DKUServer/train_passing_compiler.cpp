#include "train_passing_compiler.h"
#include <cmath>
#include <QTimer>

train_passing_compiler::train_passing_compiler(std::shared_ptr<train_passing_event> train_passing, QObject *parent)
    : QObject{parent}, train_passing{train_passing},
      sensor
{
       0,
       0.2,
       0.1,
       0.3,
       0.3 // метры
}
{

}

void train_passing_compiler::run()
{
    axis_count = -1;
    wagon_count = 0;
    if (train_passing->train_type.wagons.length() == 0)
    {
        emit train_pass_finished();
    }
    else
    {
        u = train_passing->speed;
        par_calc();
    }
}

float train_passing_compiler::time_calc(float s0)
{
    float a = train_passing->acceleration;
    float u0 = u;
    float t = (std::sqrt(2*a*s0+(u0*u0))-u0)/a;
    if (t < 0) { t = -1*((std::sqrt(2*a*s0+(u0*u0))+u0)/a); }
    u = u0 + a*t;
    return(t);
}

void train_passing_compiler::par_calc()
{
    float t;
    float s0;
    float u0_for_sen = u;
    float a_for_sen = train_passing->acceleration;
    if (axis_count == -1 && wagon_count == 0)
    {
        s0 = (train_passing->train_type.wagons[wagon_count].l_from_front)/1000;
        axis_count++;
    }
    else if (axis_count == train_passing->train_type.wagons[wagon_count].axis.length())
    {
        if (wagon_count+1 == train_passing->train_type.wagons.length())
        {
            s0 = train_passing->train_type.wagons[wagon_count].l_from_rear/1000;
            t = time_calc(s0);
            QTimer::singleShot(int(std::round(t*3600000)), this, &train_passing_compiler::train_pass_finished);
            return;
        }
        else
        {
            axis_count = 0;
            s0 = train_passing->train_type.wagons[wagon_count].l_from_rear/1000 + train_passing->train_type.wagons[wagon_count+1].l_from_front/1000;
            wagon_count++;
        }
    }
    else
    {
        s0 = train_passing->train_type.wagons[wagon_count].axis[axis_count]/1000;
        axis_count++;
    }
    float len_to_zone = (s0 - 0.5 * (sensor.sen_len/1000)) + sensor.zone0_start/1000;
    t = time_calc(s0);
    float t_for_sen = (std::sqrt(2*a_for_sen*len_to_zone+(u0_for_sen*u0_for_sen))-u0_for_sen)/a_for_sen;
    if (t_for_sen < 0) { t_for_sen = -1*((std::sqrt(2*a_for_sen*len_to_zone+(u0_for_sen*u0_for_sen))+u0_for_sen)/a_for_sen); } // дает отрицательные корни
    auto func_obj = std::bind(&train_passing_compiler::on_zone_passing, this, 0, a_for_sen, true);
    QTimer::singleShot(int(std::round(t_for_sen*3600000)), this, func_obj);
    QTimer::singleShot(int(std::round(t*3600000)), this, &train_passing_compiler::on_timer_finish);
}

void train_passing_compiler::on_timer_finish()
{
    emit axel_passed(u, train_passing->direction);
    par_calc();
}

void train_passing_compiler::on_zone_passing(int curr_zone, float a, bool entering)
{
    // на данный момент не учитываем обратный проход
    // при прохождении габаритов датчика мы не пересчитываем скорость
    if (curr_zone == 0 && entering)
    {
        emit wheel_zone0_enter();
        float s0 = (sensor.zone1_start - sensor.zone0_start)/1000;
        float t = (std::sqrt(2*a*s0+(u*u))-u)/a;
        if (t < 0) { t = -1*((std::sqrt(2*a*s0+(u*u))+u)/a); }
        auto func_obj = std::bind(&train_passing_compiler::on_zone_passing, this, 1, a, true);
        QTimer::singleShot(int(std::round(t*3600000)), this, func_obj);
    }
    else
    if (curr_zone == 1 && entering)
    {
        float s0 = (sensor.zone0_finish - sensor.zone1_start)/1000;
        float t = (std::sqrt(2*a*s0+(u*u))-u)/a;
        if (t < 0) { t = -1*((std::sqrt(2*a*s0+(u*u))+u)/a); }
        auto func_obj = std::bind(&train_passing_compiler::on_zone_passing, this, 0, a, false);
        QTimer::singleShot(int(std::round(t*3600000)), this, func_obj);
        emit wheel_zone1_enter();
    }
    else
    if (curr_zone == 0 && !entering)
    {
        emit wheel_zone0_exit();
        float s0 = (sensor.zone1_finish - sensor.zone0_finish)/1000;
        float t = (std::sqrt(2*a*s0+(u*u))-u)/a;
        if (t < 0) { t = -1*((std::sqrt(2*a*s0+(u*u))+u)/a); }
        auto func_obj = std::bind(&train_passing_compiler::on_zone_passing, this, 1, a, false);
        QTimer::singleShot(int(std::round(t*3600000)), this, func_obj);
    }
    else
    if (curr_zone == 1 && !entering)
    {
        emit wheel_zone1_exit();
    }

}
