#include <QJsonArray>
#include "../../inc/gui/cstationmenu.h"

std::vector<CFeedingStation> CStationMenu::getStationList()
{
    QJsonObject jsonObject;
    std::vector<CFeedingStation> stationList;

    CHttpRequest request(ENDPOINT_STATION_LIST, HttpVerb_et::GET);

    //Assemble the request
    request.m_formData
            .addField(FIELD_TOKEN, QByteArray(CControlBox::getInstance()->getSessionToken().c_str()));

    int ret = request.execute();

    if (request.getStatus() != HttpStatusCode_et::OK)
        return stationList;

    //Check if the request executed properly
    if (ret < 0)
        return stationList;

    //If unable to extract a JSON Object from the request response, it was corrupted
    if (request.getJsonData(&jsonObject) < 0)
        return stationList;

    QJsonArray stationArray = jsonObject["list"].toArray();

    for (const auto& item: stationArray)
    {
        QJsonObject stationJson = item.toObject();
        CFeedingStation currentStation;

        if (currentStation.loadFromJson(stationJson) <= 0)
            continue;

        stationList.push_back(currentStation);
    }

    return stationList;
}
