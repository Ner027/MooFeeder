#ifndef CONTROLBOX_CFEEDINGSTATION_H
#define CONTROLBOX_CFEEDINGSTATION_H

#include <string>
#include <QJsonObject>
#include "../entities/cserializableobject.h"

class CFeedingStation : public CSerializableObject
{
public:
    CFeedingStation() = default;
    explicit CFeedingStation(const std::string& hwId);
    int loadFromJson(QJsonObject& jsonObject) override;
    QJsonObject dumpToJson() override;
private:
    std::string m_hwId;
    std::string m_stationName;
};

#endif
