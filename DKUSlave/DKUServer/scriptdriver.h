#ifndef SCRIPTDRIVER_H
#define SCRIPTDRIVER_H

#include <QObject>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include "events_data.h"
#include <memory>

class ScriptDriver : public QObject
{
    Q_OBJECT
public:
    explicit ScriptDriver(QString file_name, QObject *parent = nullptr);
    void run();
private:
    QString file_name;
    QVector<std::shared_ptr<reg_edit_event>> events;
    int counter;
signals:

private slots:
    void emitter();
};

#endif // SCRIPTDRIVER_H
