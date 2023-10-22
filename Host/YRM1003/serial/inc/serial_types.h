#ifndef DD_SERIAL_TYPES_H
#define DD_SERIAL_TYPES_H
#include <termios.h>

typedef struct SerialPort
{
    int fileDescriptor;
    struct termios defaultConfig;
}serial_port_st;

#endif
