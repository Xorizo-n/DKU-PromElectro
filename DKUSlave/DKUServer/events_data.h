#ifndef EVENTS_DATA_H
#define EVENTS_DATA_H
#include <QString>
#include <QVector>

struct wagon
{
    int id;
    QVector<float> axis;
    float l_from_front;
    float l_from_rear;
};

struct train
{
    int id;
    QVector<wagon> wagons;
};

struct event_base
{
    QString event_type;
    int delay;
    virtual ~event_base();
};

struct reg_edit_data
{
    quint16 reg;
    quint16 new_value;
    virtual ~reg_edit_data();
};

struct reg_edit_event:public event_base
{
    QVector<reg_edit_data> edited_registers;
    virtual ~reg_edit_event();
};

struct train_passing_event:public event_base
{
    train train_type;
    float speed;
    float acceleration;
    float speed_lim;
    enum direction_type{forward,backward};
    direction_type direction;
    ~train_passing_event();
};

struct sensor
{
    float zone0_start;
    float zone0_finish;
    float zone1_start;
    float zone1_finish;
    float sen_len;
};

#endif // EVENTS_DATA_H
