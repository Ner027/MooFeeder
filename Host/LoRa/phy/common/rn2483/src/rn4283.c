/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/
#include <asm-generic/errno-base.h>
#include <stdlib.h>
#include <errno.h>
#include <stdbool.h>
#include <memory.h>
#include "../inc/rn2483.h"
#include "../inc/rn2483_commands.h"
#include "../../serial/inc/serial.h"
#include "../../strlib/inc/strlib.h"

/***********************************************************************************************************************
 * Defines
 **********************************************************************************************************************/
#define MAX_MSG_SIZE 230
#define TX_AMBLES_LEN (CMD_RADIO_TX_LEN + CMD_TERMINATOR_LEN)

/***********************************************************************************************************************
 * Typedefs
 **********************************************************************************************************************/
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

/***********************************************************************************************************************
 * Private Variables
 **********************************************************************************************************************/
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

/***********************************************************************************************************************
 * Private Prototypes
 **********************************************************************************************************************/
static rn2483_ret_et parse_response(rn2483_st* dev, size_t msgLen);
static rn2483_ret_et execute_command(rn2483_st* dev, const uint8_t* pCmd, size_t cmdLen);
static bool isValidHexChar(char ch);
static ssize_t bytes_to_hex(const uint8_t* pSrc, uint8_t* pDest, size_t srcLen);
static ssize_t hex_to_bytes(const uint8_t* pSrc, uint8_t* pDest, size_t srcLen);

/***********************************************************************************************************************
 * Public Functions
 **********************************************************************************************************************/
int rn2483_init(rn2483_st* dev, serial_port_st* serialPort)
{
    int ret;
    size_t i;
    rn2483_cmd_st* currentCmd;

    if (!dev || !serialPort)
        return -EINVAL;

    //Change the serial port terminator to the one used by the RN2483 radio (\r\n)
    ret = serial_set_terminator(serialPort, CMD_TERMINATOR_STR, CMD_TERMINATOR_LEN);
    if (ret < 0)
        return -EFAULT;

    dev->serialPort = serialPort;

    //Execute all the command on the initialization list
    for (i = 0; i < INIT_COMMANDS_LEN; ++i)
    {
        currentCmd = (rn2483_cmd_st*) &initCommands[i];

        //If any of the commands fail to execute return an error
        if (execute_command(dev, currentCmd->cmd, currentCmd->len) != currentCmd->retOk)
            return -EFAULT;
    }

    //After initialization is finished, return the physical EUI from the RN2483 device
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

int rn2483_set_wdt(rn2483_st* dev, uint64_t timeout)
{
    if (!dev)
        return -EINVAL;

    sprintf((char*)dev->serialBuffer, "radio set wdt %ld\r\n", timeout);

    if (execute_command(dev, dev->serialBuffer, strlen((char*) dev->serialBuffer)) != RADIO_OK)
        return -EFAULT;

    return 0;
}

/***********************************************************************************************************************
 * Private Functions
 **********************************************************************************************************************/
/// \brief This function parses a text response from a RN2483 device to a more usable type
/// \note This functions doesn't perform parameter checking as it is supposed to be used internally only
/// \param dev Handler to the RN2483 device
/// \param msgLen Length of the message to parse
/// \return Returns a rn2483_ret_et depending on the message received
static rn2483_ret_et parse_response(rn2483_st* dev, size_t msgLen)
{
    //First check the length of the message, as it is faster than checking if it fully matches other strings first
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

/// \brief This function checks whether or not a character can be used in an hexadecimal number
/// \param ch Character to check
/// \return True if character is a valid hex char, otherwise returns false
static bool isValidHexChar(char ch)
{
    return (((ch >= '0') && (ch <= '9')) || ((ch >= 'A') && (ch <= 'F')));
}

/// \brief This functions executes a command on a RN2483 device, waiting for the response to said command
/// \note This functions doesn't perform parameter checking as it is supposed to be used internally only
/// \param dev Handler to the RN2483 device
/// \param pCmd Buffer containing the command to execute
/// \param cmdLen Length of the command to execute
/// \return INTERNAL_ERROR if a low-level functions fails to execute, otherwise returns the response to the command
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

/// \brief This function converts an hex string to a raw byte array
/// \param pSrc Hexadecimal string
/// \param pDest Buffer to write the translated bytes to
/// \param srcLen Length of the hexadecimal string
/// \return -EINVAL if invalid parameters are passed, -EBADMSG is the string contains an invalid hex character, returns
/// the length of the retrieved raw message
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

/// \brief This functions converts a byte array to an hexadecimal string
/// \param pSrc Buffer containing the bytes that will be translated to hex
/// \param pDest Buffer where the hexadecimal string will be written to
/// \param srcLen Number of bytes to convert from
/// \return -EINVAL if invalid parameters are passed, otherwise returns the size of the resulting hex string
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