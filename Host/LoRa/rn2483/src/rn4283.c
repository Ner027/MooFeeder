#include <asm-generic/errno-base.h>
#include <stdlib.h>
#include <errno.h>
#include <stdbool.h>
#include <memory.h>
#include "../inc/rn2483.h"
#include "../inc/rn2483_commands.h"
#include "../../serial/inc/serial.h"
#include "../../strlib/inc/strlib.h"

#define MAX_MSG_SIZE 230
#define TX_AMBLES_LEN CMD_RADIO_TX_LEN + CMD_TERMINATOR_LEN

typedef enum
{
    FW_VER,
    RET_UKN,
    RET_NUM,
    RADIO_OK,
    INV_PARAM,
    RADIO_ERR,
    RADIO_BUSY,
    RADIO_TX_OK,
    RADIO_RX_DATA,
    INTERNAL_ERROR
}rn2483_ret_et;

typedef struct
{
    const uint8_t* cmd;
    size_t len;
    rn2483_ret_et retOk;
}rn2483_cmd_st;

static const uint8_t hexDecodeLut[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 0, 0, 0, 0, 0, 0, 10, 11, 12, 13, 14, 15};
static const uint8_t hexEncodeLut[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

static const rn2483_cmd_st initCommands[] =
{
    {CMD_SYS_RST,   CMD_SYS_RST_LEN,    FW_VER},
    {CMD_MAC_PAUSE, CMD_MAC_PAUSE_LEN,  RET_UKN},
    {CMD_SET_FREQ,  CMD_SET_FREQ_LEN,   RADIO_OK},
    {CMD_SET_MODE,  CMD_SET_MODE_LEN,   RADIO_OK},
    {CMD_SET_PWR,   CMD_SET_PWR_LEN,    RADIO_OK},
    {CMD_SET_SF,    CMD_SET_SF_LEN,     RADIO_OK},
    {CMD_SET_AFC,   CMD_SET_AFC_LEN,    RADIO_OK},
    {CMD_SET_BW,    CMD_SET_BW_LEN,     RADIO_OK},
    {CMD_SET_PR,    CMD_SET_PR_LEN,     RADIO_OK},
    {CMD_SET_CRC,   CMD_SET_CRC_LEN,    RADIO_OK},
    {CMD_SET_IQI,   CMD_SET_IQI_LEN,    RADIO_OK},
    {CMD_SET_CR,    CMD_SET_CR_LEN,     RADIO_OK},
    {CMD_SET_WDT,   CMD_SET_WDT_LEN,    RADIO_OK},
    {CMD_SET_SYNC,  CMD_SET_SYNC_LEN,   RADIO_OK},
    {CMD_SET_BWD,   CMD_SET_BWD_LEN,    RADIO_OK},
};

#define INIT_COMMANDS_LEN (sizeof(initCommands) / sizeof(rn2483_cmd_st))

static rn2483_ret_et parse_response(rn2483_st* dev, size_t msgLen);
static rn2483_ret_et execute_command(rn2483_st* dev, const uint8_t* pCmd, size_t cmdLen);
static ssize_t bytes_to_hex(const uint8_t* pSrc, uint8_t* pDest, size_t srcLen);
static ssize_t hex_to_bytes(const uint8_t* pSrc, uint8_t* pDest, size_t srcLen);

int rn2483_init(rn2483_st* dev, serial_port_st* serialPort)
{
    int ret;
    size_t i;
    rn2483_ret_et radioRet;
    rn2483_cmd_st* currentCmd;

    if (!dev || !serialPort)
        return -EINVAL;

    ret = serial_set_terminator(serialPort, CMD_TERMINATOR, CMD_TERMINATOR_LEN);
    if (ret < 0)
        return -EFAULT;

    dev->serialPort = serialPort;

    for (i = 0; i < INIT_COMMANDS_LEN; ++i)
    {
        currentCmd = (rn2483_cmd_st*) &initCommands[i];

        if (execute_command(dev, currentCmd->cmd, currentCmd->len) != currentCmd->retOk)
            return -EFAULT;
    }

    ret = serial_write(serialPort, CMD_GET_EUI, CMD_GET_EUI_LEN);
    if (ret < 0)
        return -EFAULT;

    ret = serial_sync_readline(serialPort, dev->serialBuffer, BUFFER_SIZE);
    if (ret < 0)
        return -EFAULT;

    return (hex_to_bytes(dev->serialBuffer, dev->phyAddr, ret) > 0) ? 0 : -EFAULT;
}
int rn2483_tx(rn2483_st* dev, const uint8_t* pSrc, uint8_t srcLen)
{
    ssize_t ret;
    rn2483_ret_et radioRet;

    if (!dev || !pSrc || (srcLen > MAX_MSG_SIZE))
        return -EINVAL;

    //Compose transmission buffer -> "radio tx <data>\r\n"
    memcpy(dev->serialBuffer, CMD_RADIO_TX, CMD_RADIO_TX_LEN);

    ret = bytes_to_hex(pSrc, &dev->serialBuffer[CMD_RADIO_TX_LEN], srcLen);
    if (ret < 0)
        return -EFAULT;

    memcpy(&dev->serialBuffer[ret + CMD_RADIO_TX_LEN], CMD_TERMINATOR, CMD_TERMINATOR_LEN);

    //Check for command accepted
    if (execute_command(dev, dev->serialBuffer, ret + TX_AMBLES_LEN) != RADIO_OK)
        return -EBADMSG;

    //Wait for transmission to complete
    ret = serial_sync_readline(dev->serialPort, dev->serialBuffer, BUFFER_SIZE);
    if (ret < 0)
        return -EFAULT;

    radioRet = parse_response(dev, ret);
    if (radioRet != RADIO_TX_OK)
        return -EBADMSG;

    return 0;
}

int rn2483_rx(rn2483_st* dev, uint8_t* pDst, uint8_t destLen)
{
    int ret;
    rn2483_ret_et radioRet;

    if (!dev || !pDst)
        return -EINVAL;

    //Start reception
    radioRet = execute_command(dev, CMD_RADIO_RX, CMD_RADIO_RX_LEN);
    if (radioRet != RADIO_OK)
        return -EBADMSG;

    //Wait for reception return
    ret = serial_sync_readline(dev->serialPort, dev->serialBuffer, BUFFER_SIZE);
    if (ret < 0)
        return -EFAULT;

    //If radio returns "radio_err" WDT was triggered
    radioRet = parse_response(dev, ret);
    if (radioRet == RADIO_ERR)
        return -ETIMEDOUT;

    //Check if valid data was received
    if (radioRet != RADIO_RX_DATA)
        return -EBADMSG;

    ret -= RET_RX_LEN;
    if (destLen < ret)
        return -ENOSPC;
    //Parse data from hex back to raw bytes
    return (int) hex_to_bytes(&dev->serialBuffer[RET_RX_LEN], pDst, ret);
}

static rn2483_ret_et parse_response(rn2483_st* dev, size_t msgLen)
{
    switch (msgLen)
    {
        case RET_OK_LEN:
            if (strncmp((char*) dev->serialBuffer, RET_OK, msgLen) == 0)
                return RADIO_OK;
            break;
        case RET_ERR_LEN:
            if (strncmp((char*) dev->serialBuffer, RET_ERR, msgLen) == 0)
                return RADIO_ERR;
            break;
        case RET_FW_LEN:
            if (strncmp((char*) dev->serialBuffer, RET_FW, msgLen) == 0)
                return FW_VER;
            break;
        case RET_BUSY_LEN:
            if (strncmp((char*) dev->serialBuffer, RET_BUSY, msgLen) == 0)
                return RADIO_BUSY;
            break;
        case RET_INV_LEN:
            if (strncmp((char*) dev->serialBuffer, RET_INV, msgLen) == 0)
                return INV_PARAM;
            break;
        case RET_TX_OK_LEN:
            if (strncmp((char*) dev->serialBuffer, RET_TX_OK, msgLen) == 0)
                return RADIO_TX_OK;
            break;
        default:
            if (msgLen <= RET_RX_LEN)
                break;

            if (strncmp((char*) dev->serialBuffer, RET_RX, RET_RX_LEN) == 0)
                return RADIO_RX_DATA;

            break;
    }

    return RET_UKN;
}

static bool isValidHexChar(char ch)
{
    return (((ch >= '0') && (ch <= '9')) || ((ch >= 'A') && (ch <= 'F')));
}

static ssize_t hex_to_bytes(const uint8_t* pSrc, uint8_t* pDest, size_t srcLen)
{
    size_t i;

    if (!pSrc || !pDest || (srcLen & 1))
        return -EINVAL;

    for (i = 0; i < srcLen; i += 2)
    {
        if (!isValidHexChar((char)pSrc[i]) || !isValidHexChar((char)pSrc[i + 1]))
            return -EBADMSG;

        *pDest++ = ((hexDecodeLut[pSrc[i] - '0'] << 4) | hexDecodeLut[pSrc[i + 1] - '0']);
    }

    return (ssize_t) (srcLen >> 1);
}

static rn2483_ret_et execute_command(rn2483_st* dev, const uint8_t* pCmd, size_t cmdLen)
{
    int ret;

    ret = serial_write(dev->serialPort, pCmd, cmdLen);
    if (ret < 0)
        return INTERNAL_ERROR;

    ret = serial_sync_readline(dev->serialPort, dev->serialBuffer, BUFFER_SIZE);
    if (ret < 0)
        return INTERNAL_ERROR;

    return parse_response(dev, ret);
}

static ssize_t bytes_to_hex(const uint8_t* pSrc, uint8_t* pDest, size_t srcLen)
{
    size_t i;

    if (!pSrc || !pDest)
        return -EINVAL;

    for (i = 0; i < srcLen; ++i)
    {
        *pDest++ = hexEncodeLut[(pSrc[i] >> 4)];
        *pDest++ = hexEncodeLut[pSrc[i] & 0x0F];
    }

    return (ssize_t) (srcLen << 1);
}