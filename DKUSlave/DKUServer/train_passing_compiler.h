#ifndef TRAIN_PASSING_COMPILER_H
#define TRAIN_PASSING_COMPILER_H

#include <QObject>
#include "events_data.h"
#include <memory>

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
signals:
    void axel_passed(float speed, train_passing_event::direction_type direction);
    void train_pass_finished(); // сигнал прохода поезда против наложения событий
private slots:
    void on_timer_finish();
};

#endif // TRAIN_PASSING_COMPILER_H
