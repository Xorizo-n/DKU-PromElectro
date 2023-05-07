#include "scriptdriver.h"
#include <QFile>
#include <QByteArray>
#include <QDebug>
#include <QJsonParseError>
#include "events_data.h"
#include <QTimer>
#include <algorithm>

ScriptDriver::ScriptDriver(QString file_name, QObject *parent)
    : QObject{parent}
    , file_name(file_name)
{
    QFile file(file_name);
    file.open(QIODevice::ReadOnly);
    QByteArray script_data = file.readAll();
    qDebug() << script_data;
    QJsonParseError parse_error; // добавить обработку ошибки парсинга
    QJsonDocument file_content = QJsonDocument::fromJson(script_data, &parse_error);
    qDebug() << parse_error.errorString();
    QJsonObject root = file_content.object();
    QJsonValue events_temp = root.value("events");
    QJsonArray objects = events_temp.toArray();
    QJsonObject data_temp = root.value("data").toObject();
    QJsonArray wagons_temp = data_temp.value("wagons").toArray();
    QVector<wagon> wagons;
    for (QJsonValue const &wagon:wagons_temp)
    {
        struct wagon w;
        w.id = wagon.toObject().value("id").toInt();
        w.l_from_front = wagon.toObject().value("l_from_front").toDouble();
        w.l_from_rear = wagon.toObject().value("l_from_rear").toDouble();
        QJsonArray axis = wagon.toObject().value("axis").toArray();
        for (QJsonValue const &axel:axis)
        {
            w.axis.push_back(axel.toDouble());
        }
        wagons.push_back(w);
    }
    QJsonArray trains_temp = data_temp.value("trains").toArray();
    for (QJsonValue const &train:trains_temp)
    {
        struct train t;
        t.id = train.toObject().value("id").toInt();
        QJsonArray t_wagons = train.toObject().value("wagons").toArray();
        for (QJsonValue const &wagon:t_wagons)
        {
            int id = wagon.toInt();
            auto is_matching = [id](struct wagon i) { return i.id == id; };
            auto temp = std::find_if(wagons.begin(), wagons.end(), is_matching);
            if (temp != wagons.end()) { t.wagons.push_back(*temp); }
        }
        trains.push_back(t);
    }
    for (QJsonValue const &object:objects)
    {
        QString event_type = object.toObject().value("event_type").toString();
        if (event_type == "reg_edit")
        {
            auto E = std::make_shared<reg_edit_event>(); // unique_ptr не работает (создает копии), понять почему
            E->delay = object.toObject().value("delay").toInt();
            E->event_type = event_type;
            QJsonValue ed_reg_temp = object.toObject().value("edited_registers");
            QJsonArray edited_registers = ed_reg_temp.toArray();
            for (QJsonValue const &data:edited_registers)
            {
                reg_edit_data T;
                T.reg = data.toObject().value("register").toInt(); // добавить контроль
                T.new_value = data.toObject().value("new_value").toInt(); // добавить контроль
                E->edited_registers.append(T);
            }
            events.push_back(E);
        }
        else if (event_type == "train_passing" )
        {
            auto E = std::make_shared<train_passing_event>();
            E->speed = object.toObject().value("speed").toDouble();
            E->acceleration = object.toObject().value("acceleration").toDouble();
            E->speed_lim = object.toObject().value("speed_lim").toDouble();
            int t_id = object.toObject().value("train_type").toInt();
            auto is_matching = [t_id](struct train i) { return i.id == t_id; };
            auto temp = std::find_if(trains.begin(), trains.end(), is_matching);
            QString dir = object.toObject().value("direction").toString();
            if (dir == "forward") E->direction = train_passing_event::forward;
            else E->direction = train_passing_event::backward;
            E->delay = object.toObject().value("delay").toInt();
            E->event_type = event_type;
            if (temp != trains.end())
            {
                E->train_type = *temp;
                events.push_back(E);
            }
        }
    }
}

void ScriptDriver::run()
{
    if (events.length() != 0)
    {
        current_event_num = 0;
        QTimer::singleShot(events[current_event_num]->delay, this, &ScriptDriver::emitter);
    }
}

void ScriptDriver::emitter()
{
    emit event_occured(events[current_event_num]);
    current_event_num++;
    if (current_event_num < events.length())
    {
        QTimer::singleShot(events[current_event_num]->delay, this, &ScriptDriver::emitter);
    }
    else { emit script_finished(); }
}
