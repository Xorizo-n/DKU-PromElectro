#include "train_passing_compiler.h"
#include <cmath>
#include <QTimer>

train_passing_compiler::train_passing_compiler(std::shared_ptr<train_passing_event> train_passing, QObject *parent)
    : QObject{parent}, train_passing{train_passing}
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

void train_passing_compiler::par_calc()
{
    float s0;
    float t;
    float a = train_passing->acceleration;
    float u0 = u;
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
            t = (std::sqrt(2*a*s0+(u0*u0))-u0)/a;
            if (t < 0) { t = -1*((std::sqrt(2*a*s0+(u0*u0))+u0)/a); }
            u = u0 + a*t; // вынести в отдельную функцию повторяющийся код!
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
    t = (std::sqrt(2*a*s0+(u0*u0))-u0)/a;
    if (t < 0) { t = -1*((std::sqrt(2*a*s0+(u0*u0))+u0)/a); }
    u = u0 + a*t;
    QTimer::singleShot(int(std::round(t*3600000)), this, &train_passing_compiler::on_timer_finish);
}

void train_passing_compiler::on_timer_finish()
{
    emit axel_passed(u, train_passing->direction);
    par_calc();
}
