#ifndef LORA_MAC_H
#define LORA_MAC_H

#include <atomic>
#include "ptwrapper/cthread.h"
#include "oswrapper/oswrapper.h"
#include "LoRa/mac/common/mac_types.h"
#include "LoRa/phy/common/rn2483/rn2483_types.h"
#include "LoRa/phy/common/phy_types.h"
#include "util/csafequeue.hpp"

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
    static CLoRaMac* getInstance();
    static void killInstance();
    bool waitOnReady(duration_t maxWait);
    int pushMessage(mac_frame_st& macFrame, uint8_t msgLen);
    int popMessage(mac_frame_st* macFrame);
private:
    CLoRaMac();
    static CLoRaMac* m_instance;
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
    std::atomic<bool> m_keepRunning;
    SemaphoreHandle_st m_macReady;
    void* run(void *) override;
    void stateInit();
    void stateEval();
    void stateTx();
    void stateRx();
    void updateTxTime();
    int findDevice(mac_addr_t* natEntry);
    int checkDestination(mac_addr_t* pAddr);
    [[nodiscard]] int getFreeSlot() const;
};

#endif
