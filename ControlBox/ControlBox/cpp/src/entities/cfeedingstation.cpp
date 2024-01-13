#include "entities/cfeedingstation.h"
#include "util/chttprequest.h"
#include "cloudtypes.h"
#include "entities/ccontrolbox.h"

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

uint32_t CFeedingStation::getPhyAddress() const
{
    return strtol(m_hwId.c_str(), nullptr, 16);
}

void CFeedingStation::setNetworkAddr(uint8_t addr)
{
    m_networkAddr = addr;
}

int CFeedingStation::addNewToCloud(const std::string &phyAddr)
{
    CHttpRequest request(ENDPOINT_STATION_CREATE, HttpVerb_et::POST);

    //Assemble the request
    request.m_formData
            .addField(FIELD_TOKEN, CControlBox::getInstance()->getSessionToken().c_str())
            .addField(FIELD_HW_ID, phyAddr.c_str())
            .addField(FIELD_NAME, "Station");

    int ret = request.execute();

    //Check if the request executed properly
    if (ret < 0)
        return -EFAULT;

    if (request.getStatus() != HttpStatusCode_et::Created)
        return -EFAULT;

    return 0;
}
