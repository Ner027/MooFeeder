#ifndef LORA_MAC_H
#define LORA_MAC_H
#include "../../../common/inc/oswrap.h"
#include "../../../ptwrap/inc/cthread.h"
#include "../../../rn2483/inc/rn2483_types.h"
#include "../../common/inc/mac_types.h"
#include "../../../phy/inc/phy_types.h"
#include "../../../queue/inc/queue.hpp"

typedef enum
{
    ST_INIT,
    ST_EVAL,
    ST_TX,
    ST_RX,
}mac_state_st;

class CLoRaMac : public CThread
{
public:
    CLoRaMac();
private:
    systick_t m_txTimeBegin;
    systick_t m_txTimeEnd;
    systick_t m_lastTxTime;
    mac_state_st m_currentState, m_nextState;
    nat_table_st m_natTable;
    serial_port_st m_radioPort;
    rn2483_st m_loraRadio;
    phy_frame_st m_syncMsg;
    CQueue<phy_frame_st> m_txQueue;
    CQueue<phy_frame_st> m_rxQueue;

    void* run(void *) override;
    void stateInit();
    void stateEval();
    void stateTx();
    void stateRx();
    void updateTxTime();
    int findDevice(mac_addr_t* natEntry);
};

#endif
