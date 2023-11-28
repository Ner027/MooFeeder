#ifndef CONTROLBOX_CSERIALIZABLEOBJECT_H
#define CONTROLBOX_CSERIALIZABLEOBJECT_H

#include <QJsonObject>

class CSerializableObject
{
public:
    virtual QJsonObject dumpToJson() = 0;
    virtual int loadFromJson(QJsonObject& jsonObject) = 0;
};

#endif
