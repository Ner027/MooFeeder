#ifndef CLORANETWORK_H
#define CLORANETWORK_H

#include "ptwrapper/cthread.h"
#include "LoRa/app/app_types.h"
#include "LoRa/network/common/network_types.h"
#include "LoRa/mac/server/cloramac.h"
#include <atomic>

typedef enum
{
    NETWORK_INIT,
    NETWORK_TX,
    NETWORK_RX
}network_state_et;

class CLoRaNetwork : public CThread
{
public:
    static CLoRaNetwork* getInstance();
    void killInstance();
    void* run(void *) override;
    int sendMessage(app_frame_st& appFrame, uint8_t src, uint8_t dest);
    int receiveMessage(app_frame_st& appFrame, uint8_t src);
    bool waitOnReady();
private:
    CLoRaNetwork();
    static CLoRaNetwork* m_instance;
    CLoRaMac* m_macInstance;
    std::atomic<bool> m_keepRunning;
    network_state_et m_currentState;
    network_state_et m_nextState;
    SemaphoreHandle_st m_networkReady;
    CQueue<network_frame_st> m_networkTxQueues[NR_OF_DEVICES];
    CQueue<network_frame_st> m_networkRxQueues[NR_OF_DEVICES];
    int decodeMacMessage(mac_frame_st& src, uint8_t srcLen);
    int composeMacMessage();
    void stateInit();
    void stateTx();
    void stateRx();
};



#endif
