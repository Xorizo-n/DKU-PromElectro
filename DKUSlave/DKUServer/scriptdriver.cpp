#include "scriptdriver.h"
#include <QFile>
#include <QByteArray>
#include <QDebug>
#include <QJsonParseError>
#include "events_data.h"
#include <QTimer>

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
    QJsonObject root = file_content.object();
    QJsonValue events_temp = root.value("events");
    QJsonArray objects = events_temp.toArray();
    for (QJsonValue object:objects)
    {
        QString event_type = object.toObject().value("event_type").toString();
        if (event_type == "reg_edit")
        {
            events.push_back(std::make_shared<reg_edit_event>());
            std::shared_ptr<reg_edit_event> E = events.last(); // unique_ptr не работает (создает копии), понять почему
            E->delay = object.toObject().value("delay").toInt();
            E->event_type = event_type;
            QJsonValue ed_reg_temp = object.toObject().value("edited_registers");
            QJsonArray edited_registers = ed_reg_temp.toArray();
            for (QJsonValue data:edited_registers)
            {
                reg_edit_data T;
                T.reg = data.toObject().value("register").toInt(); // добавить контроль
                T.new_value = data.toObject().value("new_value").toInt(); // добавить контроль
                E->edited_registers.append(T);
            }
        }
    }
}

void ScriptDriver::run()
{
    if (events.length() != 0)
    {
        counter = 0; // уверен что нужно перенести в конструктор
        QTimer::singleShot(events[counter]->delay, this, &ScriptDriver::emitter);
    }
}

void ScriptDriver::emitter()
{
    counter++;
}
