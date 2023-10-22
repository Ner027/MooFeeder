#ifndef DD_SERIAL_H
#define DD_SERIAL_H

#include "serial_types.h"
#include <stddef.h>

int serial_open(char* port, int br, serial_port_st* serialPort);
int serial_close(serial_port_st* serialPort);
int serial_write(serial_port_st* serialPort, const uint8_t* txBuffer, size_t bufferSize);
int serial_read(serial_port_st* serialPort, uint8_t* rxBuffer, size_t bufferSize);
int serial_sync_readline(serial_port_st* serialPort, uint8_t* rxBuffer, size_t bufferSize);
int serial_flush(serial_port_st* serialPort);

#endif
