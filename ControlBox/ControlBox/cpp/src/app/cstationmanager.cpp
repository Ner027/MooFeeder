#include "app/cstationmanager.h"
#include "entities/ccalf.h"

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

                    CCalf calf(QByteArray((char*)pBudgetRequest->rfidTag).toHex().toStdString());

                    if (!calf.isValid())
                        calf.addNewToCloud(m_activeStations[i].phyAddr);

                    float allowedConsumption = calf.getAllowedConsumption();
                    budget_response_st* pBudgetResponse = (budget_response_st*) appFrame.payload;

                    memcpy(pBudgetResponse->rfidTag, pBudgetRequest->rfidTag, RFID_TAG_LEN);
                    memcpy(&pBudgetResponse->allowedConsumption, &allowedConsumption, 4);
                    appFrame.control.frameType = BUDGET_RESPONSE;

                    m_networkInstance->sendMessage(appFrame, i, SERVER_SLOT);
                    break;
                }
                case CONSUMPTION_REPORT:
                {

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
                    printf("Received invalid app frame type! Type: (%d)\n", appFrame.control.frameType);
            }
        }
    }

    return nullptr;
}

CStationManager::CStationManager()
{
    m_networkInstance = CLoRaNetwork::getInstance();
}
