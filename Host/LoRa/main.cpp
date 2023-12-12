#include "mac/server/inc/cloramac.h"
#include "network/server/inc/cloranetwork.h"

#define SERVER
int main()
{
#ifdef SERVER
    //CLoRaMac loraMac;
    //loraMac.join();
#else
    mac_init();
#endif

    CLoRaNetwork loraNet;
    app_frame_st appFrame;

    budget_request_st* pReq = (budget_request_st*) appFrame.payload;
    pReq->rfidTag[0] = 0xDE;
    pReq->rfidTag[1] = 0xAD;
    pReq->rfidTag[2] = 0xBE;
    pReq->rfidTag[3] = 0xEF;
    pReq->rfidTag[4] = 0xFA;
    pReq->rfidTag[5] = 0xFE;
    pReq->rfidTag[6] = 0xDE;
    pReq->rfidTag[7] = 0xAD;
    appFrame.control.len = sizeof(budget_request_st);

    loraNet.sendMessage(appFrame, SERVER_SLOT, 0);
    pReq->rfidTag[7] = 0x00;
    loraNet.sendMessage(appFrame, SERVER_SLOT, 1);
    pReq->rfidTag[7] = 0x10;
    loraNet.sendMessage(appFrame, SERVER_SLOT, 2);

    loraNet.composeMacMessage();

    while (1) {}
    return 0;
}
