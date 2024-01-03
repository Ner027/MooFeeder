/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/
#include <errno.h>
#include <memory.h>
#include "../inc/yrm100x.h"
#include "../../Serial/inc/serial.h"
#include "../../platform/inc/platform.h"
#include "usart.h"

/***********************************************************************************************************************
 * Defines
 **********************************************************************************************************************/
#define HEADER 0xBB
#define END 0x7E

#define PREAMBLE_SIZE 5
#define POSTAMBLE_SIZE 2
#define CHECKSUM_OFFSET 1
#define AMBLES_SIZE (PREAMBLE_SIZE + POSTAMBLE_SIZE)

#define TYPE_CMD  0x00
#define TYPE_RESP 0x01
#define TYPE_NOTE 0x02
#define TYPE_ERR 0xFF

#define HEADER_POS  0
#define TYPE_POS    1
#define CMD_POS     2
#define PLM_POS     3
#define PLL_POS     4
#define PAY_POS     5
#define CHK_POS(x) ((x) - 2)

/***********************************************************************************************************************
 * Private Prototypes
 **********************************************************************************************************************/
 /// \brief Calculates the checksum for a given YRM100x payload
 /// \param buffer Buffer containing the payload to calculate the checksum
 /// \param bufferSize Size of parameter buffer
 /// \return Returns the calculated checksum
static uint8_t yrm100x_calculate_checksum(const uint8_t* buffer, uint16_t bufferSize);

/***********************************************************************************************************************
 * Public Functions
 **********************************************************************************************************************/
int yrm100x_init(yrm100x_st* dev, void* serialPort)
{
    int ret;
    char terminator = END;

    if (!dev)
        return -EINVAL;

    //Check if device is not already initialized
    if (dev->initialized)
        return -EALREADY;

#ifdef HOST
    ret = serial_open((char*) serialPort, B115200, &dev->serialPort);
#else
    ret = serial_open(&dev->serialPort, (UART_HandleTypeDef*)serialPort);
#endif
    if (ret < 0)
        return -EIO;

    ret = serial_set_terminator(&dev->serialPort, &terminator, 1);
    if (ret < 0)
        return -EIO;

    //Hardware reset
    GPIO_SET_LEVEL(YRM_EN_PORT, YRM_EN_PIN, 0);
    THREAD_SLEEP_FOR(SYSTEM_TICK_FROM_MS(1));
    GPIO_SET_LEVEL(YRM_EN_PORT, YRM_EN_PIN, 1);

    serial_flush(&dev->serialPort);

    dev->initialized = 1;

    yrm100x_set_region(dev, REGION_EUROPE);

    return 0;
}

int yrm100x_kill(yrm100x_st* dev)
{
    if (!dev)
        return -EINVAL;

    if (!dev->initialized)
        return -EPERM;

    return serial_close(&dev->serialPort);
}

int yrm100x_write_command(yrm100x_st* dev, uint8_t cmd, const uint8_t* args, uint16_t argc)
{
    uint8_t chkSum;

    if (!dev)
        return -EINVAL;

    if (!dev->initialized)
        return -EPERM;

    //Compose Preamble
    dev->rwBuffer[HEADER_POS] = HEADER;
    dev->rwBuffer[TYPE_POS] = TYPE_CMD;
    dev->rwBuffer[CMD_POS] = cmd;
    dev->rwBuffer[PLM_POS] = GET_BYTE(argc, 1);
    dev->rwBuffer[PLL_POS] = GET_BYTE(argc, 0);

    //Some commands don't have parameters, so no need to copy them
    if (args && argc)
        memcpy(&dev->rwBuffer[PAY_POS], args, argc);

    //Calculate checksum
    chkSum = yrm100x_calculate_checksum(&dev->rwBuffer[CHECKSUM_OFFSET], argc + PREAMBLE_SIZE - 1);

    //Compose Postamble
    dev->rwBuffer[PREAMBLE_SIZE + argc] = chkSum;
    dev->rwBuffer[PREAMBLE_SIZE + argc + 1] = END;

    return serial_write(&dev->serialPort, dev->rwBuffer, argc + AMBLES_SIZE);
}

int yrm100x_read_parse(yrm100x_st* dev, yrm100x_ret_st* retSt, uint8_t* retBuffer, uint8_t bufferLen)
{
    int ret;
    uint8_t* rwBuffer;
    uint8_t chkSum;

    if (!dev || !retSt)
        return -EINVAL;

    if (!dev->initialized)
        return -EPERM;

    rwBuffer = dev->rwBuffer;

    ret = serial_sync_readline(&dev->serialPort, rwBuffer, RW_BUFFER_SIZE);
    if (ret <= 0)
        return -ENODATA;

    //Check if read message is valid by confirming its header
    if ((rwBuffer[HEADER_POS] != HEADER))
        return -EBADMSG;

    //Get response parameters
    retSt->len = CONCAT_BYTES(rwBuffer[PLL_POS], rwBuffer[PLM_POS]);
    chkSum = yrm100x_calculate_checksum(&rwBuffer[TYPE_POS], retSt->len + PREAMBLE_SIZE - 1);

    //Check if calculated checksum matches received checksum
    if (chkSum != rwBuffer[CHK_POS(ret)])
        return -EBADMSG;

    retSt->type = rwBuffer[TYPE_POS];
    retSt->cmd  = rwBuffer[CMD_POS];

    //If the command returns an error, the first byte of the payload contains a more detailed error code
    if (retSt->cmd == TYPE_ERR)
    {
        retSt->errCode = rwBuffer[PAY_POS];
        return -EFAULT;
    }

    if (!retBuffer || !bufferLen)
        return 0;

    //Check if passed buffer is big enough to copy received parameters
    if (bufferLen < retSt->len)
        return -ENOBUFS;

    memcpy(retBuffer, &dev->rwBuffer[PAY_POS], retSt->len);

    return 0;
}

int yrm100x_get_region(yrm100x_st* dev)
{
    int ret;
    uint8_t rxBuffer;
    yrm100x_ret_st retCodes;

    ret = yrm100x_write_command(dev, CMD_GET_REGION, NULL, 0);

    if (ret < 0)
        return ret;

    ret = yrm100x_read_parse(dev, &retCodes, &rxBuffer, 1);

    if (ret < 0)
        return ret;

    return (int)rxBuffer;
}

int yrm100x_set_region(yrm100x_st* dev, uint8_t region)
{
    int ret;
    yrm100x_ret_st retCodes;

    if ((region > REGION_CHINA800) && (region != REGION_KOREA))
        return -EINVAL;

    ret = yrm100x_write_command(dev, CMD_SET_REGION, &region, 1);

    if (ret < 0)
        return ret;

    ret = yrm100x_read_parse(dev, &retCodes, NULL, 0);

    if (ret < 0)
        return ret;

    return 0;
}

static uint8_t yrm100x_calculate_checksum(const uint8_t* buffer, uint16_t bufferSize)
{
    uint8_t sum = 0;

    //Checksum is obtained by adding all bytes from the type argument till the end of the parameters
    while (bufferSize--)
        sum += *buffer++;

    return sum;
}