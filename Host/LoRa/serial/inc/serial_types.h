#ifndef DD_SERIAL_TYPES_H
#define DD_SERIAL_TYPES_H

/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/
#ifdef __linux
#include <termios.h>
#include <stdint.h>
#endif

#define MAX_TERMINATOR_LEN 4

/***********************************************************************************************************************
 * Structs
 **********************************************************************************************************************/
#ifdef __linux
typedef struct
{
    int fileDescriptor;
    uint8_t initialized;
    uint8_t terminatorSize;
    char terminator[MAX_TERMINATOR_LEN];
    struct termios defaultConfig;
}serial_port_st;
#endif

#endif
