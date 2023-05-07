#include "train_passing_compiler.h"
#include <cmath>
#include <QTimer>

train_passing_compiler::train_passing_compiler(std::shared_ptr<train_passing_event> train_passing, QObject *parent)
    : QObject{parent}, train_passing{train_passing}
{

}

void train_passing_compiler::run()
{
    float t;
    float a = train_passing->acceleration;
    float u0 = train_passing->speed;
    float s0 = (train_passing->train_type.wagons[0].l_from_front)/1000;
    t = (std::sqrt(2*a*s0+(u0*u0))-u0)/a;
    if (t < 0) { t = -1*((std::sqrt(2*a*s0+(u0*u0))+u0)/a); }
    u = u0 + a*t;
    QTimer::singleShot(int(std::round(t*3600000)), this, &train_passing_compiler::on_timer_finish);
}

void train_passing_compiler::on_timer_finish()
{
    emit axel_passed(u, train_passing->direction);
}
