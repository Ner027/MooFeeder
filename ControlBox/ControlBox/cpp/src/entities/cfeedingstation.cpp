#include "../../inc/gui/cfeedingstation.h"

CFeedingStation::CFeedingStation(const std::string& hwId)
{
    m_hwId = hwId;
}

QJsonObject CFeedingStation::dumpToJson()
{
    QJsonObject jsonObject;

    jsonObject["name"] = QString::fromStdString(m_stationName);
    jsonObject["hw_id"] = QString::fromStdString(m_hwId);

    return jsonObject;
}

int CFeedingStation::loadFromJson(QJsonObject& jsonObject)
{
    int ret = 0;

    if (jsonObject.contains("hw_id"))
    {
        m_hwId = jsonObject["hw_id"].toString().toStdString();
        ret++;
    }

    if (jsonObject.contains("name"))
    {
        m_stationName = jsonObject["name"].toString().toStdString();
        ret++;
    }

    return ret;
}

