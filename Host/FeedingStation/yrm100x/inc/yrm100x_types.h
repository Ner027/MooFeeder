#ifndef DD_YRM100X_TYPES_H
#define DD_YRM100X_TYPES_H

/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/
#include <stdint.h>
#include "../../serial/inc/serial_types.h"

/***********************************************************************************************************************
 * Defines
 **********************************************************************************************************************/
#define RW_BUFFER_SIZE 32
#define EPC_SIZE 12

/***********************************************************************************************************************
 * Structs
 **********************************************************************************************************************/
typedef struct
{
    uint8_t initialized;
    uint8_t rwBuffer[RW_BUFFER_SIZE];
    serial_port_st serialPort;
}yrm100x_st;

typedef struct
{
    uint8_t type;
    uint8_t cmd;
    uint8_t errCode;
    uint16_t len;
}yrm100x_ret_st;

typedef struct __attribute__((packed))
{
    uint8_t rssi;
    uint8_t pcMsb;
    uint8_t pcLsb;
    uint8_t epc[EPC_SIZE];
    uint8_t crcMsb;
    uint8_t crcLsb;
}yrm100x_inv_st;

/***********************************************************************************************************************
 * Unions
 **********************************************************************************************************************/
typedef union
{
    uint8_t raw[sizeof(yrm100x_inv_st)];
    yrm100x_inv_st data;
}yrm100x_inv_ut;
#endif