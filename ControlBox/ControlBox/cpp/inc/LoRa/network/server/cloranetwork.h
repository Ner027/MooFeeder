#ifndef CLORANETWORK_H
#define CLORANETWORK_H

/***********************************************************************************************************************
 * Includes
 ******************************************************************************************************************++**/
#include "ptwrapper/cthread.h"
#include "LoRa/app/app_types.h"
#include "LoRa/network/common/network_types.h"
#include "LoRa/mac/server/cloramac.h"
#include <atomic>

/***********************************************************************************************************************
 * Typedefs
 ******************************************************************************************************************++**/
typedef enum
{
    NETWORK_INIT,
    NETWORK_TX,
    NETWORK_RX
}network_state_et;

class CLoRaNetwork : public CThread
{
public:
    /// \brief This method allows to return a singleton instance of the LoRa Network
    /// \return Pointer to LoRa Network singleton instance
    static CLoRaNetwork* getInstance();

    /// \brief This method allows to destroy the LoRa Network instance, stopping all network operations beforehand
    void killInstance();

    /// \brief This method allows to push a message to the LoRa Network transmission queue
    /// \param appFrame Application level frame containing the message to be sent
    /// \param src Network address of the source device
    /// \param dest  Network address of the destination device
    /// \return -EINVAL if invalid addresses are used, 0 on success
    int sendMessage(app_frame_st& appFrame, uint8_t src, uint8_t dest);

    /// \brief This method tries to receive a message from a specific device on the LoRa Network
    /// \param appFrame Application level frame where data will be returned to
    /// \param src Network address of the source device
    /// \return -EINVAL if invalid addresses are used, -ENODATA if no message is available, returns 0 on success
    int receiveMessage(app_frame_st& appFrame, uint8_t src);
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
    void* run(void *) override;
    int decodeMacMessage(mac_frame_st& src, uint8_t srcLen);
    int composeMacMessage();
    void stateInit();
    void stateTx();
    void stateRx();
};



#endif
