#include "mac/server/inc/cloramac.h"
#include "network/server/inc/cloranetwork.h"
#include "mac/client/inc/mac_client.h"

#define SERVER
int main()
{
    /*
#ifdef SERVER
    CLoRaMac::getInstance()->join();
#else
    mac_init();
#endif
*/
    CLoRaNetwork* loraNet = CLoRaNetwork::getInstance();

    loraNet->waitOnReady();

    app_frame_st appFrame;

    loraNet->detach();

    while (1)
    {
        budget_request_st* pReq = (budget_request_st*) appFrame.payload;

        pReq->rfidTag[0] = 0xDE;
        pReq->rfidTag[1] = 0xAD;
        pReq->rfidTag[2] = 0xBE;
        pReq->rfidTag[3] = 0xEF;
        pReq->rfidTag[4] = 0xFA;
        pReq->rfidTag[5] = 0xFE;
        pReq->rfidTag[6] = 0xDE;
        pReq->rfidTag[7] = 0xAD;
        appFrame.control.frameType = BUDGET_REQUEST;
        loraNet->sendMessage(appFrame, SERVER_SLOT, 2);
        pReq->rfidTag[7] = 0x00;
        loraNet->sendMessage(appFrame, SERVER_SLOT, 3);
        pReq->rfidTag[7] = 0x10;
        loraNet->sendMessage(appFrame, SERVER_SLOT, 4);

        budget_response_st* pRep = (budget_response_st*) pReq;
        pRep->allowedConsumption = 0xAAC0FFEE;
        appFrame.control.frameType = BUDGET_RESPONSE;
        loraNet->sendMessage(appFrame, SERVER_SLOT, 5);

        THREAD_SLEEP_FOR(SYSTEM_TICK_FROM_MS(5000));
    }
    return 0;
}
