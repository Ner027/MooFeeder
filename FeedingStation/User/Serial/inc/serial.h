#ifndef DD_SERIAL_H
#define DD_SERIAL_H

/***********************************************************************************************************************
 * Includes
 ***********************************************************************************************************************/
#include <stdio.h>
#include "FreeRTOS.h"
#include "stm32l4xx_hal.h"
#include "../../oswrapper/inc/oswrapper.h"

/***********************************************************************************************************************
 * Defines
 ***********************************************************************************************************************/
#define BUFFER_SIZE 512
#define MAX_TERMINATOR_LEN 4
//#define THREAD_SAFE

/***********************************************************************************************************************
 * Typedefs
 ***********************************************************************************************************************/
typedef struct
{
    UART_HandleTypeDef* uartHandle;
#ifdef THREAD_SAFE
    SemaphoreHandle_t txMutex;
    SemaphoreHandle_t rxMutex;
#endif
    uint8_t rxByte;
    uint8_t txRunning;
    uint8_t initialized;
    uint8_t terminatorSize;
    size_t txWrIndex;
    size_t txRdIndex;
    size_t rxWrIndex;
    size_t rxRdIndex;
    uint8_t txBuffer[BUFFER_SIZE];
    uint8_t rxBuffer[BUFFER_SIZE];
    char terminator[MAX_TERMINATOR_LEN];
}serial_port_st;

/***********************************************************************************************************************
 * Public Prototypes
 ***********************************************************************************************************************/

/// \brief Initializes a serial port driver, this function assumes the lower level initializations have already
/// been done
/// \param serialPort Serial port handler
/// \param uartHandle Low level UART handler
/// \return -EINVAL if invalid parameters where received, -ENOSPC if unable to register more serial ports, otherwise
/// returns 0 if everything is OK
int serial_open(serial_port_st *serialPort, UART_HandleTypeDef *uartHandle);

/// \brief Closes a serial port freeing all allocated resources
/// \param serialPort Serial port handler
/// \return -EINVAL if invalid parameters where received, -ENODEV if passed handler was not registered, -EFAULT if
/// unable to de-init the UART device correctly, otherwise returns 0 if everything is OK
int serial_close(serial_port_st *serialPort);

/// \brief Flush all the pending characters of a serial port
/// \param serialPort Serial port handler
/// \return -EINVAL if invalid parameters where received, otherwise returns 0 on success
int serial_flush(serial_port_st* serialPort);

/// \brief Sends a message to a serial port in an asynchronous way
/// \param serialPort Serial port handler
/// \param txBuffer Buffer to transmit
/// \param bufferLen Size of the buffer to transmit
/// \return -EINVAL if invalid parameters where received, -ETIMEDOUT if the serial port is busy and a predefined
/// max waiting time was exceeded, -ENOSPC if unable to transmit all the passed transmission buffer, returns 0
/// if everything is OK
ssize_t serial_write(serial_port_st* serialPort, const uint8_t* txBuffer, size_t bufferLen);

/// \brief Tries to received from a serial port
/// \param serialPort Serial port handler
/// \param rxBuffer Reception buffer
/// \param bufferLen Max amount of characters to receive
/// \return -EINVAL if invalid parameters were received, -ETIMEDOUT if the serial port is busy and a predefined
/// max waiting time was exceeded, if able to received, returns the number of character received
ssize_t serial_read(serial_port_st* serialPort, uint8_t* rxBuffer, size_t bufferLen);

/// \brief Read a line from a serial port waiting for a set amount of time to receive said line
/// \param serialPort Serial Port handler
/// \param rxBuffer Buffer to write the line to
/// \param bufferSize Max length of the line
/// \param maxWait Max amount of time to wait for receiving a line
/// \return -EINVAL if invalid parameters were received, -ETIMEDOUT if unable to receive a full line in the set
// time, otherwise returns the amount of characters read
int serial_readline(serial_port_st* serialPort, uint8_t* rxBuffer, size_t bufferSize, duration_t maxWait);

int serial_set_terminator(serial_port_st* serialPort, const char* newTerminator, uint8_t terminatorSize);

#endif
