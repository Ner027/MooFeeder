#include "mac/server/inc/cloramac.h"
#include "network/server/inc/cloranetwork.h"
#include "mac/client/inc/mac_client.h"

#define SERVER
int main()
{
    CLoRaNetwork* loraNet = CLoRaNetwork::getInstance();

    loraNet->waitOnReady();

    app_frame_st appFrame;

    loraNet->detach();

    std::thread consumerThread([&loraNet]
    {
        int ret;
        while (1)
        {
            for (int i = 2; i < NR_OF_DEVICES + 2; ++i)
            {
                app_frame_st appMessage;

                ret = loraNet->receiveMessage(appMessage, i);
                if (ret < 0)
                    continue;

                switch (appMessage.control.frameType)
                {
                    case BUDGET_REQUEST:
                    {
                        budget_request_st* pBudgetReq = (budget_request_st*) appMessage.payload;
                        printf("Budget request for tag: 0x%.2lx\n", *((uint64_t*)pBudgetReq->rfidTag));

                        budget_response_st* pBudgetResponse = (budget_response_st*) appMessage.payload;
                        float vAllowed = 10.0f;
                        memcpy(pBudgetResponse->rfidTag, pBudgetReq->rfidTag, RFID_TAG_LEN);
                        memcpy(&pBudgetResponse->allowedConsumption, &vAllowed, 4);
                        appMessage.control.frameType = BUDGET_RESPONSE;
                        loraNet->sendMessage(appMessage, SERVER_SLOT, i);

                        break;
                    }
                    case BUDGET_RESPONSE:
                        break;
                    case CONSUMPTION_REPORT:
                    {
                        consumption_report_st* pConsumptionRep = (consumption_report_st*) appMessage.payload;
                        float vConsumed;
                        memcpy(&vConsumed, &pConsumptionRep->volumeConsumed, 4);
                        printf("Consumption Report from tag: 0x%lx with volume: %f\n",
                               *((uint64_t*)pConsumptionRep->rfidTag),
                               vConsumed);
                        break;
                    }
                    case SENSOR_DATA:
                        break;
                    case TEXT_MESSAGE:
                        text_msg_st* pTxtMsg = (text_msg_st*) appMessage.payload;
                        printf("Client at Network Address: %d sent message: %.*s\n", i, pTxtMsg->messageSize, pTxtMsg->textMessage);
                        break;
                }
            }

            THREAD_SLEEP_FOR(SYSTEM_TICK_FROM_MS(10));
        }
    });

    consumerThread.join();
    return 0;
}
