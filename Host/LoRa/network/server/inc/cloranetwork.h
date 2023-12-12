#ifndef CLORANETWORK_H
#define CLORANETWORK_H
#include "../../../app/common/inc/app_types.h"
#include "../../../mac/common/inc/mac_types.h"
#include "../../../ptwrap/inc/cthread.h"
#include "../../../queue/inc/queue.hpp"


class CLoRaNetwork : public CThread
{
public:
    void* run(void *) override;
    int sendMessage(app_frame_st& appFrame, uint8_t src, uint8_t dest);
    int decodeMacMessage(mac_frame_st& src, uint8_t srcLen);
    int composeMacMessage();
private:
    CQueue<network_frame_st> networkTxQueues[NR_OF_DEVICES];
    CQueue<network_frame_st> networkRxQueues[NR_OF_DEVICES];
};



#endif
