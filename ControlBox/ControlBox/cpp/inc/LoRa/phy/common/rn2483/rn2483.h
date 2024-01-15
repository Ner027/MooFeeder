#ifndef LORA_RN2483_DRIVER_H
#define LORA_RN2483_DRIVER_H

/***********************************************************************************************************************
*  Includes
**********************************************************************************************************************/
#include "rn2483_types.h"

/***********************************************************************************************************************
*  Defines
**********************************************************************************************************************/
#define MAX_WDT_TIME 4294967295

/***********************************************************************************************************************
*  RN2483 APIs
**********************************************************************************************************************/
/// \brief This function performs the initialization process of a RN2483 Radio
/// \param dev Handler to the RN2483 device
/// \param serialPort Serial port to be used for communicating with the device
/// \return -EINVAL if invalid parameters are passed, -EFAULT if an initialization command fails to execute, otherwise
// returns 0 if everything is ok
int rn2483_init(rn2483_st* dev, serial_port_st* serialPort);

/// \brief This function puts the device in transmission mode and waits for the response to the TX command
/// \param dev Handler to the RN2483 device
/// \param pSrc Buffer that the device should transmit
/// \param srcLen Size of the buffer to transmit
/// \return -EINVAL if invalid parameters are passed, -EFAULT if an internal error occurs, -EBADMSG if a wrong response
// from the radio is received, otherwise returns 0 on success
int rn2483_tx(rn2483_st* dev, const uint8_t* pSrc, uint8_t srcLen);

/// \brief This function puts the device in reception mode and wait for a message to be received
/// \param dev Handler to the RN2483 device
/// \param pDst Buffer to write data received to
/// \param destLen Size of the buffer
/// \return -EINVAL if invalid parameters are passed, -EBADMSG if a wrong response from the radio is received, -EFAULT
// if an internal error occurs, -ETIMEDOUT if the device watchdog was triggered and no data was received, otherwise
// returns the length of data received
int rn2483_rx(rn2483_st* dev, uint8_t* pDst, uint8_t destLen);

/// \brief This functions allows to change the watchdog timeout of an RN2483 device
/// \param dev Handler to the RN2483 device
/// \param timeout New timeout to set
/// \return -EINVAL if invalid parameters are passed, -EFAULT if failed to set the new timeout, returns 0 on success
int rn2483_set_wdt(rn2483_st* dev, uint64_t timeout);

#endif
