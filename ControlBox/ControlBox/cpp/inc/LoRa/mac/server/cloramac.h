#ifndef LORA_MAC_H
#define LORA_MAC_H

/***********************************************************************************************************************
 * Includes
 ******************************************************************************************************************++**/
#include <atomic>
#include "ptwrapper/cthread.h"
#include "oswrapper/oswrapper.h"
#include "LoRa/mac/common/mac_types.h"
#include "LoRa/phy/common/rn2483/rn2483_types.h"
#include "LoRa/phy/common/phy_types.h"
#include "util/csafequeue.hpp"


/***********************************************************************************************************************
 * Typedefs
 ******************************************************************************************************************++**/
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
    /// \brief This method allows to return a singleton instance of the LoRa MAC
    /// \return Pointer to LoRa MAC singleton instance
    static CLoRaMac* getInstance();

    /// \brief This method allows to destroy the LoRa MAC instance, stopping all MAC operations beforehand
    void killInstance();

    /// \brief This method waits for a set period of time for the LoRa MAC Layer to be read
    /// \param maxWait Max wait time
    /// \return True if MAC was ready before time expired, otherwise returns false
    bool waitOnReady(duration_t maxWait);

    /// \brief This method allows to push a new MAC message to the MAC transmission queue
    /// \param macFrame MAC Frame to add to transmission queue
    /// \param msgLen Length of the MAC Frame being inserted
    /// \return Returns the length of the physical frame needed for encapsulating this MAC Frame
    int pushMessage(mac_frame_st& macFrame, uint8_t msgLen);

    /// \brief This method tries to retrieve a message from the MAC reception queue
    /// \param macFrame
    /// \return -EINVAL if invalid parameters are passed, -ENODATA if no message is available, returns 0 on sucess
    int popMessage(mac_frame_st* macFrame);
private:
    /// \brief Default constructor
    /// \note Made private to only allow instantiation from singleton
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
