/***********************************************************************************************************************
 * Includes
 ***********************************************************************************************************************/
#include <errno.h>
#include <string.h>
#include "FreeRTOS.h"
#include "task.h"
#include "stm32l4xx_hal.h"
#include "../inc/serial.h"

/***********************************************************************************************************************
 * Defines
 ***********************************************************************************************************************/
#define MAX_SERIAL_PORTS 4
#define MAX_WAIT_TICKS 2
#define DEFAULT_TERMINATOR '\n'
//#define ENABLE_LOG

/***********************************************************************************************************************
 * Private Variables
 ***********************************************************************************************************************/
static serial_port_st *registeredPorts[MAX_SERIAL_PORTS] = {0};

/***********************************************************************************************************************
 * Private Prototypes
 ***********************************************************************************************************************/
static int getPortIndex(UART_HandleTypeDef* uartHandle);

/***********************************************************************************************************************
 * Callbacks
 ***********************************************************************************************************************/
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *uartHandle)
{
    int i;
    serial_port_st* serialPort;

    if (!uartHandle)
        return;

    i = getPortIndex(uartHandle);

    if (i < 0)
        return;

    serialPort = registeredPorts[i];

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

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *uartHandle)
{
    int i;
    serial_port_st* serialPort;

    if (!uartHandle)
        return;

    i = getPortIndex(uartHandle);

    if (i < 0)
        return;

    serialPort = registeredPorts[i];

    serialPort->rxBuffer[serialPort->rxWrIndex++ & (BUFFER_SIZE - 1)] = serialPort->rxByte;

    HAL_UART_Receive_IT(serialPort->uartHandle, &serialPort->rxByte, 1);
}

/***********************************************************************************************************************
 * Public Functions
 ***********************************************************************************************************************/
int serial_open(serial_port_st *serialPort, UART_HandleTypeDef *uartHandle)
{
    uint8_t i;

    if (!serialPort || !uartHandle)
        return -EINVAL;

    if (serialPort->initialized)
        return -EALREADY;

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
    for (i = 0; i < MAX_SERIAL_PORTS; ++i)
    {
        if (registeredPorts[i] == NULL)
        {
            registeredPorts[i] = serialPort;
            break;
        }
    }

    if (i == MAX_SERIAL_PORTS)
        return -ENOSPC;

    serialPort->initialized = 1;
    serialPort->terminator[0] = DEFAULT_TERMINATOR;
    serialPort->terminatorSize = 1;

    HAL_UART_Receive_IT(uartHandle, &serialPort->rxByte, 1);

    return 0;
}

int serial_close(serial_port_st *serialPort)
{
    uint8_t i;

    if (!serialPort)
        return -EINVAL;

    for (i = 0; i < MAX_SERIAL_PORTS; ++i)
    {
        if (registeredPorts[i]->uartHandle->Instance == serialPort->uartHandle->Instance)
        {
#ifdef THREAD_SAFE
            vSemaphoreDelete(serialPort->txMutex);
            vSemaphoreDelete(serialPort->rxMutex);
#endif
            registeredPorts[i] = NULL;
            break;
        }
    }

    if (i == MAX_SERIAL_PORTS)
        return -ENODEV;

    if (HAL_UART_DeInit(serialPort->uartHandle) != HAL_OK)
        return -EFAULT;

    return 0;
}

ssize_t serial_write(serial_port_st* serialPort, const uint8_t* txBuffer, size_t bufferLen)
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

#ifdef ENABLE_LOG
    printf("[%lu] %s", xTaskGetTickCount(), txBuffer);
#endif

#ifdef THREAD_SAFE
    xSemaphoreGive(serialPort->txMutex);
#endif
    return 0;
}

ssize_t serial_read(serial_port_st* serialPort, uint8_t* rxBuffer, size_t bufferLen)
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


int serial_sync_readline(serial_port_st* serialPort, uint8_t* rxBuffer, size_t bufferSize)
{
    int ret;
    uint8_t ch;
    size_t bytesRead;

    if (!serialPort || !rxBuffer)
        return -EINVAL;

    if (!serialPort->initialized)
        return -EPERM;

    bytesRead = 0;
    while ((bytesRead < bufferSize))
    {
        ret = serial_read(serialPort, &ch, 1);

        //If read returns a negative value some error occurred, in this case exit the functions
        if (ret < 0)
            return ret;

        //If nothing was read just keep waiting
        if (ret == 0)
        {
            vTaskDelay(5/portTICK_PERIOD_MS);
            continue;
        }
        rxBuffer[bytesRead++] = ch;

        //If the received character matches the set terminator, exit and return the number of bytes that was read
        if (ch == serialPort->terminator[0])
            return (int)bytesRead;
    }

    return 0;
}

int serial_readline(serial_port_st* serialPort, uint8_t* rxBuffer, size_t bufferSize, duration_t maxWait)
{
    int ret;
    uint8_t ch;
    int8_t termPosition;
    size_t bytesRead;
    systick_t endTime;

    if (!serialPort || !rxBuffer)
        return -EINVAL;

    if (!serialPort->initialized)
        return -EPERM;

    endTime = GET_CURRENT_TIME();
    if (maxWait == MAX_DELAY)
        endTime = maxWait;
    else endTime = GET_CURRENT_TIME() + maxWait;

    termPosition = -1;
    bytesRead = 0;
    while ((bytesRead < bufferSize) && (GET_CURRENT_TIME() < endTime))
    {
        ret = serial_read(serialPort, &ch, 1);

        //If read returns a negative value some error occurred, in this case exit the functions
        if (ret < 0)
            return ret;

        //If nothing was read just keep waiting
        if (ret == 0)
        {
            THREAD_SLEEP_FOR(SYSTEM_TICK_FROM_MS(5));
            continue;
        }

        if (termPosition < 0)
        {
            if (ch == serialPort->terminator[0])
                termPosition = 1;
        }
        else
        {
            if (ch == serialPort->terminator[termPosition])
                termPosition++;
            else return -EINVAL;
        }

        if (termPosition == serialPort->terminatorSize)
        {
            rxBuffer[bytesRead] = '\0';
#ifdef ENABLE_LOG
            printf("[%lu] %s\n", xTaskGetTickCount(), (char*)rxBuffer);
#endif
            return (int) bytesRead;
        }

        if (termPosition < 0)
            rxBuffer[bytesRead++] = ch;
    }

    if (GET_CURRENT_TIME() >= endTime)
        return -ETIMEDOUT;

    return 0;
}

int serial_set_terminator(serial_port_st* serialPort, const char* newTerminator, uint8_t terminatorSize)
{
    if (!serialPort || (terminatorSize > MAX_TERMINATOR_LEN))
        return -EINVAL;

    if (!serialPort->initialized)
        return -EPERM;

    memcpy(serialPort->terminator, newTerminator, terminatorSize);
    serialPort->terminatorSize = terminatorSize;

    return 0;
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

/***********************************************************************************************************************
 * Private Functions
 ***********************************************************************************************************************/
static int getPortIndex(UART_HandleTypeDef* uartHandle)
{
    uint8_t i;

    for (i = 0; i < MAX_SERIAL_PORTS; ++i)
    {
        if (registeredPorts[i]->uartHandle->Instance == uartHandle->Instance)
            return i;
    }

    return -1;
}