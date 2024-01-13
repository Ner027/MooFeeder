#include <QJsonArray>
#include "../../inc/gui/cmonitormenu.h"

std::vector<CCalf> CMonitorMenu::getCalfList()
{
    std::vector<CCalf> calfList;
    QJsonObject jsonObject;
    CHttpRequest request(ENDPOINT_CALF_LIST, HttpVerb_et::GET);

    //Assemble the request
    request.m_formData
        .addField(FIELD_TOKEN, QByteArray(CControlBox::getInstance()->getSessionToken().c_str()));

    int ret = request.execute();

    if (request.getStatus() != HttpStatusCode_et::OK)
        return calfList;

    //Check if the request executed properly
    if (ret < 0)
        return calfList;

    //If unable to extract a JSON Object from the request response, it was corrupted
    if (request.getJsonData(jsonObject) < 0)
        return calfList;

    QJsonArray calfArray = jsonObject["list"].toArray();

    for (const auto& item: calfArray)
    {
        QJsonObject calf = item.toObject();
        calfList.emplace_back(calf["phyTag"].toString().toStdString(),
                              (float)calf["maxConsumption"].toDouble());
    }

    return calfList;
}
