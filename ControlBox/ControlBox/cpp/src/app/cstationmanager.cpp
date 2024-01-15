#include <QJsonArray>
#include "app/cstationmanager.h"
#include "entities/ccalf.h"
#include "util/chttprequest.h"
#include "cloudtypes.h"
#include "entities/ccontrolbox.h"
#include "util/hexutil.h"
#include "entities/cfeedingstation.h"

#define CONVERT_VBAT(x) (x)
#define CONVERT_TEMP(x) (x)

CStationManager* CStationManager::m_instance = nullptr;

CStationManager* CStationManager::getInstance()
{
    if (!m_instance)
        m_instance = new CStationManager;

    return m_instance;
}

void* CStationManager::run(void* args)
{
    int ret;
    app_frame_st appFrame;

    while (m_keepRunning)
    {
        for (int i = 2; i < NR_OF_SLOTS; ++i)
        {
            ret = m_networkInstance->receiveMessage(appFrame, i);
            if (ret < 0)
                continue;

            switch (appFrame.control.frameType)
            {
                case BUDGET_REQUEST:
                {
                    budget_request_st* pBudgetRequest = (budget_request_st*) appFrame.payload;

                    CCalf calf(arrayToHex(pBudgetRequest->rfidTag, RFID_TAG_LEN));

                    if (!calf.isValid())
                    {
                        CLockGuard lock(m_activeStationsMutex);
                        calf.addNewToCloud(m_activeStations[i].phyAddr);
                    }

                    float allowedConsumption = calf.getAllowedConsumption();
                    budget_response_st* pBudgetResponse = (budget_response_st*) appFrame.payload;

                    memcpy(pBudgetResponse->rfidTag, pBudgetRequest->rfidTag, RFID_TAG_LEN);
                    memcpy(&pBudgetResponse->allowedConsumption, &allowedConsumption, 4);
                    appFrame.control.frameType = BUDGET_RESPONSE;

                    m_networkInstance->sendMessage(appFrame, SERVER_SLOT, i);
                    break;
                }
                case CONSUMPTION_REPORT:
                {
                    consumption_report_st* pConsumptionReport = (consumption_report_st*) appFrame.payload;

                    CCalf calf(arrayToHex(pConsumptionReport->rfidTag, RFID_TAG_LEN));
                    float* pConsumedVolume;

                    pConsumedVolume = (float*) &pConsumptionReport->volumeConsumed;

                    MANAGER_LOG("Calf with tag: 0x%lx consumed %f liters!\n", *((uint32_t*)pConsumptionReport->rfidTag), *pConsumedVolume);

                    ret = calf.reportConsumption(*pConsumedVolume);
                    if (ret < 0)
                        printf("Failed to update calf consumption!\n");

                    break;
                }
                case SENSOR_DATA:
                {
                    sensor_data_st* pSensorData = (sensor_data_st*) appFrame.payload;
                    m_activeStations[i].currBattery = CONVERT_VBAT(pSensorData->rawVbat);
                    m_activeStations[i].currTemperature = CONVERT_TEMP(pSensorData->rawTemperature);

                    break;
                }
                default:
                    MANAGER_LOG("Received invalid app frame type! Type: (%d)\n", appFrame.control.frameType);
            }
        }
    }

    return nullptr;
}

CStationManager::CStationManager()
{
    m_networkInstance = CLoRaNetwork::getInstance();
    m_keepRunning = true;
}


int CStationManager::getStationData(QString &phyTag, station_data_st * pStationData)
{
    if (!pStationData)
        return -EINVAL;

    uint32_t hwAddr = strtol(phyTag.toStdString().c_str(), nullptr, 16);

    for (const auto &item: m_activeStations)
    {
        if (item.second.phyAddr == hwAddr)
        {
            memcpy(pStationData, &item.second, sizeof(station_data_st));
            return 0;
        }
    }

    return -ENODEV;
}

int CStationManager::registerStation(station_data_st &newStation)
{
    CLockGuard lock(m_activeStationsMutex);

    if (m_activeStations.find(newStation.netAddr) != m_activeStations.end())
        m_activeStations.erase(newStation.netAddr);

    m_activeStations[newStation.netAddr] = newStation;

    auto stationList = getStationList();

    for (auto &item: stationList)
    {
        if (item.getPhyAddress() == newStation.phyAddr)
        {
            item.setNetworkAddr(newStation.netAddr);
            return 0;
        }
    }

    CFeedingStation::addNewToCloud(arrayToHex((uint8_t*)&newStation.phyAddr, 4));

    return 0;
}

std::vector<CFeedingStation> CStationManager::getStationList()
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
    if (request.getJsonData(jsonObject) < 0)
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