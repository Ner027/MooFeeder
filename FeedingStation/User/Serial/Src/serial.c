/***********************************************************************************************************************
 * Includes
 ***********************************************************************************************************************/
#include <errno.h>
#include <string.h>
#include "../Inc/serial.h"
#include "../Inc/serial_types.h"
#include "FreeRTOS.h"
#include "task.h"

/***********************************************************************************************************************
 * Defines
 ***********************************************************************************************************************/
#define MAX_SERIAL_PORTS 4
#define MAX_WAIT_TICKS 2
#define LINE_TERMINATOR '\n'

/***********************************************************************************************************************
 * Private Variables
 ***********************************************************************************************************************/

/***********************************************************************************************************************
 * Private Prototypes
 ***********************************************************************************************************************/

/***********************************************************************************************************************
 * Callbacks
 ***********************************************************************************************************************/
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *uartHandle)
{
    int i;
    serial_port_st* serialPort;

    if (!uartHandle)
        return;

    serialPort = (serial_port_st*) uartHandle->pRxBuffPtr;

    if ((serialPort->txWrIndex - serialPort->txRdIndex) <= 0)
    {
        serialPort->txRunning = 0;
    }
    else
    {
        HAL_UART_Transmit_IT(uartHandle,
                             &serialPort->txBuffer[serialPort->txRdIndex++ & (BUFFER_SIZE - 1)],
                             1);
    }
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef* uartHandle)
{
    serial_port_st* serialPort;

    if (!uartHandle)
        return;

    serialPort = (serial_port_st *) uartHandle->pRxBuffPtr;

    serialPort->rxBuffer[serialPort->rxWrIndex++ & (BUFFER_SIZE - 1)] = serialPort->rxByte;

    HAL_UART_Receive_IT(serialPort->uartHandle, &serialPort->rxByte, 1);
}

/***********************************************************************************************************************
 * Public Functions
 ***********************************************************************************************************************/
int serial_init(serial_port_st *serialPort, UART_HandleTypeDef *uartHandle)
{
    if (!serialPort || !uartHandle)
        return -EINVAL;

    serialPort->uartHandle = uartHandle;

    serialPort->txWrIndex = 0;
    serialPort->txRdIndex = 0;
    serialPort->rxWrIndex = 0;
    serialPort->rxRdIndex = 0;

    serialPort->txRunning = 0;

#ifdef THREAD_SAFE
    serialPort->txMutex = xSemaphoreCreateMutex();
    serialPort->rxMutex = xSemaphoreCreateMutex();
#endif

    HAL_UART_Receive_IT(uartHandle, &serialPort->rxByte, 1);

    return 0;
}

int serial_close(serial_port_st *serialPort)
{
    if (!serialPort)
        return -EINVAL;

    if (HAL_UART_DeInit(serialPort->uartHandle) != HAL_OK)
        return -EFAULT;

    return 0;
}

ssize_t serial_tx_async(serial_port_st* serialPort, const uint8_t* txBuffer, size_t bufferLen)
{
    size_t i;

    if (!serialPort || !txBuffer)
        return -EINVAL;
#ifdef THREAD_SAFE
    if (xSemaphoreTake(serialPort->txMutex, MAX_WAIT_TICKS) != pdTRUE)
        return -ETIMEDOUT;
#endif
    if ((BUFFER_SIZE - (serialPort->txWrIndex - serialPort->txRdIndex)) < bufferLen)
    {
#ifdef THREAD_SAFE
        xSemaphoreGive(serialPort->txMutex);
#endif
        return -ENOSPC;
    }

    for (i = 0; i < bufferLen; ++i)
        serialPort->txBuffer[serialPort->txWrIndex++ & (BUFFER_SIZE - 1)] = txBuffer[i];

    if (!serialPort->txRunning)
    {
        serialPort->txRunning = 1;
        HAL_UART_Transmit_IT(serialPort->uartHandle,
                             &serialPort->txBuffer[serialPort->txRdIndex++ & (BUFFER_SIZE - 1)], 1);
    }

#ifdef THREAD_SAFE
    xSemaphoreGive(serialPort->txMutex);
#endif
    return 0;
}

ssize_t serial_rx_async(serial_port_st* serialPort, uint8_t* rxBuffer, size_t bufferLen)
{
    size_t i, len;

    if (!serialPort || !rxBuffer)
        return -EINVAL;
#ifdef THREAD_SAFE
    if (xSemaphoreTake(serialPort->rxMutex, MAX_WAIT_TICKS) != pdTRUE)
        return -ETIMEDOUT;
#endif
    len = (serialPort->rxWrIndex - serialPort->rxRdIndex);

    if (len > bufferLen)
        len = bufferLen;

    for (i = 0; i < len; ++i)
        rxBuffer[i] = serialPort->rxBuffer[serialPort->rxRdIndex++ & (BUFFER_SIZE - 1)];
#ifdef THREAD_SAFE
    xSemaphoreGive(serialPort->rxMutex);
#endif
    return (ssize_t) len;
}

ssize_t serial_readline(serial_port_st* serialPort, uint8_t* rxBuffer, size_t bufferLen, TickType_t maxWait)
{
    int ret;
    uint8_t rx;
    uint8_t* pStart = rxBuffer;

    size_t readBytes;

    if (!serialPort || !rxBuffer)
        return -EINVAL;

    TickType_t endTime = xTaskGetTickCount() + maxWait;

    readBytes = 0;
    do
    {
        ret = serial_rx_async(serialPort, &rx, 1);

        if (ret > 0)
        {
            *rxBuffer++ = rx;
            readBytes++;
        }
        else vTaskDelay(5/portTICK_PERIOD_MS);
    }
    while ((rx != LINE_TERMINATOR) && ((endTime - xTaskGetTickCount()) > 0) && (readBytes < bufferLen));

    if (readBytes >= bufferLen)
        return -ENOSPC;

    if (rx != LINE_TERMINATOR)
        return -ETIMEDOUT;

    *rxBuffer = '\0';

    printf("%s", pStart);

    return (ssize_t) (readBytes);
}

int serial_flush(serial_port_st* serialPort)
{
    if (!serialPort)
        return -EINVAL;
#ifdef THREAD_SAFE
    xSemaphoreTake(serialPort->txMutex, portMAX_DELAY);
    xSemaphoreTake(serialPort->rxMutex, portMAX_DELAY);
#endif
    serialPort->txWrIndex = 0;
    serialPort->txRdIndex = 0;
    serialPort->rxWrIndex = 0;
    serialPort->rxRdIndex = 0;

    serialPort->txRunning = 0;
#ifdef THREAD_SAFE
    xSemaphoreGive(serialPort->txMutex);
    xSemaphoreGive(serialPort->rxMutex);
#endif
    return 0;
}
