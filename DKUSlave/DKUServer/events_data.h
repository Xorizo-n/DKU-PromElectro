#ifndef EVENTS_DATA_H
#define EVENTS_DATA_H
#include <QString>
#include <QVector>

struct event_base
{
    QString event_type;
    int delay;
};

struct reg_edit_data
{
    quint16 reg;
    quint16 new_value;
};

struct reg_edit_event:public event_base
{
    QVector<reg_edit_data> edited_registers;
};

#endif // EVENTS_DATA_H
