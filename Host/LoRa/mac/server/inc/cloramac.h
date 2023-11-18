#ifndef LORA_MAC_H
#define LORA_MAC_H
#include "../../../common/inc/oswrap.h"
#include "../../../ptwrap/inc/cthread.h"
#include "../../../rn2483/inc/rn2483_types.h"
#include "../../common/inc/mac_types.h"
#include "../../../phy/inc/phy_types.h"
#include "../../../queue/inc/queue.hpp"

class CLoRaMac : public CThread
{
public:
    CLoRaMac();
private:
    systick_t m_txTimeBegin;
    systick_t m_txTimeEnd;
    serial_port_st m_radioPort;
    rn2483_st m_loraRadio;
    phy_frame_st m_syncMsg;
    CQueue<mac_frame_st> m_txQueue;
    CQueue<mac_frame_st> m_rxQueue;

    void* run(void *) override;
    void txFlow();
    void rxFlow();
    void updateTxTime();
};

#endif
