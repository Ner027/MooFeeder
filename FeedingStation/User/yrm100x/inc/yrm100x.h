#ifndef DD_YRM100X_H
#define DD_YRM100X_H

/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/
#include "yrm100x_types.h"

/***********************************************************************************************************************
 * YRM100x Commands
 **********************************************************************************************************************/
#define CMD_GET_INFO        0x03
#define CMD_INV_SINGLE      0x22
#define CMD_INV_MULTI       0x27
#define CMD_SET_SPARAMS     0x0C
#define CMD_SET_SMODE       0x12
#define CMD_READ_BANK       0x39
#define CMD_WRITE_BANK      0x49
#define CMD_LOCK_TAG        0x82
#define CMD_KILL_TAG        0x65
#define CMD_GET_QPARAMS     0x0D
#define CMD_SET_QPARAMS     0x0E
#define CMD_SET_REGION      0x07
#define CMD_GET_REGION      0x08
#define CMD_SET_CHANNEL     0xAB
#define CMD_GET_CHANNEL     0xAA
#define CMD_FRQ_HOPPING     0xAD
#define CMD_GET_POWER       0xB7
#define CMD_SET_POWER       0xB6
#define CMD_CONT_WAVE       0xB0
#define CMD_GET_MPARAMS     0xF1
#define CMD_SET_MPARAMS     0xF0
#define CMD_SCAN_JAMMER     0xF2
#define CMD_SCAN_CHANNEL    0xF3
#define CMD_CFG_GPIO        0x1A
#define CMD_GOTO_SLEEP      0x17
#define CMD_CFG_IDLE        0x1D

/***********************************************************************************************************************
 * YRM100x Error Codes
 **********************************************************************************************************************/
#define ERR_CMD_ERR         0x17
#define ERR_FHHS_FAIL       0x20
#define ERR_INV_FAIL        0x15
#define ERR_ACC_FAIL        0x16
#define ERR_READ_FAIL       0x09
#define ERR_READ_ERR        0xA0
#define ERR_WRITE_FAIL      0x10
#define ERR_WRITE_ERR       0xB0
#define ERR_LOCK_FAIL       0x13
#define ERR_LOCK_ERR        0xC0
#define ERR_KILL_FAIL       0x13
#define ERR_KILL_ERR        0xD0

/***********************************************************************************************************************
 * YRM100x Regions
 **********************************************************************************************************************/
#define REGION_CHINA900     0x01
#define REGION_USA          0x02
#define REGION_EUROPE       0x03
#define REGION_CHINA800     0x04
#define REGION_KOREA        0x06

/***********************************************************************************************************************
 * YRM100x Constants
 **********************************************************************************************************************/
#define TAG_EPC_LEN 12


/***********************************************************************************************************************
 * YRM100x APIs
 **********************************************************************************************************************/
 /// \brief Initializes a YRM100x device
 /// \param dev YRM100x device handler
 /// \param serialPort Serial port to use as communication medium with YRM100x
 /// \return 0 if initialized properly, -EINVAL if invalid arguments are passed, -EALREADY if already initialized,
 /// or -EIO if an error occured while trying to open the serial port
int yrm100x_init(yrm100x_st* dev, void* serialPort);

/// \brief Writes a command with or without arguments to an YRM100x device
/// \param dev YRM100x device handler
/// \param cmd Command to execute
/// \param args Command arguments
/// \param argc Length of the arguments
/// \return 0 if command written correctly, -EINVAL if invalid arguments are passed, -EPERM if YRM100x device is not
/// initialized, or -EIO if an error occurred while trying to use the serial port
int yrm100x_write_command(yrm100x_st* dev, uint8_t cmd, const uint8_t* args, uint16_t argc);

/// \brief Reads and parses a response from a YRM100x device
/// \param dev YRM100x device handler
/// \param retSt Structure where return codes are received
/// \param retBuffer Buffer where returned parameters are received
/// \param bufferLen retBuffer max length
/// \return Returns 0 if response was read and parsed correctly, -EINVAL if invalid arguments are passed, -EPERM if
/// YRM100x device is not initialized, -EBADMSG if received response is corrupted, -EFAULT if response was an error code
/// or returns -ENOBUFS if bufferLen is less then the received response payload size
int yrm100x_read_parse(yrm100x_st* dev, yrm100x_ret_st* retSt, uint8_t* retBuffer, uint8_t bufferLen);

/// \brief Get the currently set region on a YRM100x device
/// \param dev YRM100x device handler
/// \return Returns the number of the set region, or negative error codes which are shared with yrm100x_write_command
/// and yrm100x_read_parse
int yrm100x_get_region(yrm100x_st* dev);

///
/// \param dev
/// \param region
/// \return
int yrm100x_set_region(yrm100x_st* dev, uint8_t region);

#endif
