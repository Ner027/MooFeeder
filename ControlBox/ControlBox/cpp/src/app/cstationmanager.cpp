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
        //Iterate through all the network addresses
        for (int i = 2; i < NR_OF_SLOTS; ++i)
        {
            ret = m_networkInstance->receiveMessage(appFrame, i);
            if (ret < 0)
                continue;

            //When a message is received check its type
            switch (appFrame.control.frameType)
            {
                case BUDGET_REQUEST:
                {
                    budget_request_st* pBudgetRequest = (budget_request_st*) appFrame.payload;

                    //Try to load a Calf based on the received RFID tag
                    CCalf calf(arrayToHex(pBudgetRequest->rfidTag, RFID_TAG_LEN));

                    //If there is yet no data for this calf, it means it was not registered yet, and must be inserted
                    //in the cloud system, before further changes
                    if (!calf.isValid())
                    {
                        CLockGuard lock(m_activeStationsMutex);
                        calf.addNewToCloud(m_activeStations[i].phyAddr);
                    }

                    //Respond to the budget request with a budget response message
                    float allowedConsumption = calf.getAllowedConsumption();
                    budget_response_st* pBudgetResponse = (budget_response_st*) appFrame.payload;

                    //RFID Tag must match the one from the request
                    memcpy(pBudgetResponse->rfidTag, pBudgetRequest->rfidTag, RFID_TAG_LEN);
                    memcpy(&pBudgetResponse->allowedConsumption, &allowedConsumption, 4);
                    appFrame.control.frameType = BUDGET_RESPONSE;

                    m_networkInstance->sendMessage(appFrame, SERVER_SLOT, i);
                    break;
                }
                case CONSUMPTION_REPORT:
                {
                    //Consumption reports are received when a calf finishes being fed, or finishes its consumption budget
                    consumption_report_st* pConsumptionReport = (consumption_report_st*) appFrame.payload;

                    //Load the calf based on is RFID Tag
                    CCalf calf(arrayToHex(pConsumptionReport->rfidTag, RFID_TAG_LEN));

                    //Extract consumed volume from payload
                    float* pConsumedVolume;
                    pConsumedVolume = (float*) &pConsumptionReport->volumeConsumed;

                    MANAGER_LOG("Calf with tag: 0x%lx consumed %f liters!\n", *((uint32_t*)pConsumptionReport->rfidTag), *pConsumedVolume);

                    //Update the consumption on the cloud system
                    ret = calf.reportConsumption(*pConsumedVolume);
                    if (ret < 0)
                        printf("Failed to update calf consumption!\n");

                    break;
                }
                case SENSOR_DATA:
                {
                    //Update the sensor values on the station data
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
    int ret = 0;
    CLockGuard lock(m_activeStationsMutex);

    if (m_activeStations.find(newStation.netAddr) != m_activeStations.end())
    {
        ret = EALREADY;
        m_activeStations.erase(newStation.netAddr);
    }

    m_activeStations[newStation.netAddr] = newStation;

    auto stationList = getStationList();

    for (auto &item: stationList)
    {
        if (item.getPhyAddress() == newStation.phyAddr)
        {
            item.setNetworkAddr(newStation.netAddr);
            return ret;
        }
    }

    CFeedingStation::addNewToCloud(arrayToHex((uint8_t*)&newStation.phyAddr, 4));

    return ret;
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