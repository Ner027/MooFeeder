#include <QJsonDocument>
#include <QJsonArray>
#include "../../inc/entities/ccalf.h"
#include "../../inc/entities/ccontrolbox.h"

CCalf::CCalf(const std::string phyTag)
{
    m_phyTag = phyTag;

    QJsonObject calfData;

    if (getCalfFromCloud(&calfData) < 0)
        return;//TODO: Throw exception

    m_maxConsumption = (float) calfData["calfData"].toObject()["maxConsumption"].toDouble();
    m_currentConsumption = (float) calfData["calfData"].toObject()["currentConsumption"].toDouble();
}

int CCalf::getCalfFromCloud(QJsonObject* jsonObject)
{
    if (!jsonObject)
        return -EINVAL;

    CHttpRequest request(ENDPOINT_CALF_DATA, HttpVerb_et::GET);

    //Assemble the request
    request.m_formData
            .addField(FIELD_PHY_TAG, QByteArray(m_phyTag.c_str()))
            .addField(FIELD_TOKEN, QByteArray(CControlBox::getInstance()->getSessionToken().c_str()));

    int ret = request.execute();

    if (request.getStatus() != HttpStatusCode_et::OK)
        return -EFAULT;

    //Check if the request executed properly
    if (ret < 0)
        return -EFAULT;

    //If unable to extract a JSON Object from the request response, it was corrupted
    if (request.getJsonData(jsonObject) < 0)
        return -EFAULT;

    return 0;
}

std::vector<CLog> CCalf::getLatestLogs()
{
    std::vector<CLog> logs;

    QJsonObject calfData;

    if (getCalfFromCloud(&calfData) < 0)
        return logs;

    QJsonArray logArray = calfData["logData"].toArray();

    for (const auto& item : logArray)
    {
        QJsonObject log = item.toObject();

        logs.emplace_back((float)log["volume"].toDouble(), log["timestamp"].toInt());
    }

    return logs;
}

CCalf::CCalf(const std::string phyTag, float maxConsumption) : CCalf(phyTag)
{
    m_maxConsumption = maxConsumption;
}

std::string CCalf::getPhyTag()
{
    return m_phyTag;
}

float CCalf::getMaxConsumption()
{
    return m_maxConsumption;
}

float CCalf::getCurrentConsumption()
{
    return m_currentConsumption;
}

QJsonObject CCalf::dumpToJson()
{
    QJsonObject jsonObject;

    jsonObject["rfidTag"] = m_phyTag.c_str();
    jsonObject["currentConsumption"] = m_currentConsumption;
    jsonObject["maxConsumption"] = m_maxConsumption;

    return jsonObject;
}




