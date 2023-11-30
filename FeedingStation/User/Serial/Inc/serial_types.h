#ifndef DD_SERIAL_TYPES_H
#define DD_SERIAL_TYPES_H

#include <stdio.h>
#include "portmacro.h"
#include "../../Core/Inc/usart.h"
#include "FreeRTOS.h"
#include "semphr.h"

#define BUFFER_SIZE 128
#define TX_BUFFER_SIZE BUFFER_SIZE
#define RX_BUFFER_SIZE BUFFER_SIZE

typedef struct __attribute__((packed))
{
    uint8_t rxByte;
    uint8_t txRunning;
    size_t txWrIndex;
    size_t txRdIndex;
    size_t rxWrIndex;
    size_t rxRdIndex;
    UART_HandleTypeDef* uartHandle;
    uint8_t txBuffer[TX_BUFFER_SIZE];
    uint8_t rxBuffer[RX_BUFFER_SIZE];
#ifdef THREAD_SAFE
    SemaphoreHandle_t txMutex;
    SemaphoreHandle_t rxMutex;
#endif
}serial_port_st;

/// \brief Initializes a serial port driver, this function assumes the lower level initializations have already
/// been done
/// \param serialPort Serial port handler
/// \param uartHandle Low level UART handler
/// \return -EINVAL if invalid parameters where received, -ENOSPC if unable to register more serial ports, otherwise
/// returns 0 if everything is OK
int serial_init(serial_port_st* serialPort, UART_HandleTypeDef* uartHandle);

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
ssize_t serial_tx_async(serial_port_st* serialPort, const uint8_t* txBuffer, size_t bufferLen);

/// \brief Tries to received from a serial port
/// \param serialPort Serial port handler
/// \param rxBuffer Reception buffer
/// \param bufferLen Max amount of characters to receive
/// \return -EINVAL if invalid parameters were received, -ETIMEDOUT if the serial port is busy and a predefined
/// max waiting time was exceeded, if able to received, returns the number of character received
ssize_t serial_rx_async(serial_port_st* serialPort, uint8_t* rxBuffer, size_t bufferLen);

/// \brief Read a line from a serial port waiting for a set amount of time to receive said line
/// \param serialPort Serial Port handler
/// \param rxBuffer Buffer to write the line to
/// \param bufferLen Max length of the line
/// \param maxWait Max amount of time to wait for receiving a line
/// \return -EINVAL if invalid parameters were received, -ETIMEDOUT if unable to receive a full line in the set
// time, otherwise returns the amount of characters read
ssize_t serial_readline(serial_port_st* serialPort, uint8_t* rxBuffer, size_t bufferLen, TickType_t maxWait);

#endif
