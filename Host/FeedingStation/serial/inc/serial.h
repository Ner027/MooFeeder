#ifndef DD_SERIAL_H
#define DD_SERIAL_H

/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/
#include "serial_types.h"
#include <stddef.h>


/***********************************************************************************************************************
 * Serial APIs
 **********************************************************************************************************************/
/// \brief Tries to open a serial port
/// \param port Serial port inode name, ie. /dev/tty...
/// \param br Baud rate
/// \param serialPort Serial port handler struct
/// \return Returns 0 if everything went out correctly, otherwise returns -EINVAL if invalid arguments were passed,
/// or errno in case of an internal error
int serial_open(char* port, int br, serial_port_st* serialPort);


/// \brief Closes a previously opened serial port, returns its configs to their original state
/// \param serialPort Serial port handler
/// \return Returns 0 if everything went out correctly, otherwise returns -EINVAL if invalid arguments were passed,
/// -EPERM if device was no initialized, or errno in case of an internal error
int serial_close(serial_port_st* serialPort);

/// \brief Writes to a serial port
/// \param serialPort Serial port handler
/// \param txBuffer Bytes that the serial port should transmit
/// \param bufferSize Length of the transmission buffer
/// \return Returns 0 if everything went out correctly, otherwise returns -EINVAL if invalid arguments were passed,
/// -EPERM if device was no initialized, or errno in case of an internal error
int serial_write(serial_port_st* serialPort, const uint8_t* txBuffer, size_t bufferSize);

/// \brief Reads from a serial port
/// \param serialPort Serial port handler
/// \param rxBuffer Buffer to write bytes read from the serial port
/// \param bufferSize Max amount of bytes to read
/// \return Returns the number of bytes read, otherwise returns -EINVAL if invalid arguments were passed,
/// -EPERM if device was no initialized, or errno in case of an internal error
int serial_read(serial_port_st* serialPort, uint8_t* rxBuffer, size_t bufferSize);

/// \brief Wait and read until the set terminator character is found, or the buffer size is exceeded
/// \param serialPort Serial port handler
/// \param rxBuffer Buffer to write bytes read from the serial port
/// \param bufferSize Max amount of bytes to read
/// \return Returns the number of bytes read, otherwise returns -EINVAL if invalid arguments were passed,
/// -EPERM if device was no initialized, or errno in case of an internal error
int serial_sync_readline(serial_port_st* serialPort, uint8_t* rxBuffer, size_t bufferSize);

/// \brief Sets the character used to determine if the end of a line was reached
/// \param serialPort Serial port handler
/// \param newTerminator Character to use as the terminator
/// \return Returns 0 if everything went out correctly, otherwise returns -EINVAL if invalid arguments were passed,
/// -EPERM if device was no initialized, or errno in case of an internal error
int serial_set_terminator(serial_port_st* serialPort, char newTerminator);

/// \brief Flushes a serial port
/// \param serialPort Serial port handler
/// \return Returns 0 if everything went out correctly, otherwise returns -EINVAL if invalid arguments were passed,
/// -EPERM if device was no initialized, or errno in case of an internal error
int serial_flush(serial_port_st* serialPort);

#endif
