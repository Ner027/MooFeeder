/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/
#include <fcntl.h>
#include <termios.h>
#include <memory.h>
#include <unistd.h>
#include <stdint.h>
#include <errno.h>
#include "../inc/serial.h"

/***********************************************************************************************************************
 * Defines
 **********************************************************************************************************************/
#define DEFAULT_TERMINATOR '\n'

/***********************************************************************************************************************
 * Private Prototypes
 **********************************************************************************************************************/
int serial_read_internal(serial_port_st* serialPort, uint8_t* rxBuffer, size_t bufferSize);

/***********************************************************************************************************************
 * Public Functions
 **********************************************************************************************************************/
#ifdef __linux //Serial ports are handled differently in Linux and Windows
int serial_open(char* port, int br, serial_port_st* serialPort)
{
    int ret;
    struct termios ttyDevice;

    if (!serialPort || !port)
        return -EINVAL;

    if (serialPort->initialized)
        return -EALREADY;

    ret = open(port, O_RDWR);
    if (ret < 0)
        return -errno;

    serialPort->fileDescriptor = ret;

    //Get current port configurations
    ret = tcgetattr(serialPort->fileDescriptor, &ttyDevice);
    if (ret < 0)
        return -errno;

    //Save current port configuration before changing it
    memcpy(&serialPort->defaultConfig, &ttyDevice, sizeof(struct termios));

    ret = cfsetspeed(&ttyDevice, br);
    if (ret < 0)
        return -errno;

    //8 bits, no parity, 1 stop bit, no Hw flow control
    ttyDevice.c_cflag |= CS8;
    ttyDevice.c_cflag &= ~PARENB;
    ttyDevice.c_cflag &= ~CSTOPB;
    ttyDevice.c_cflag &= ~CRTSCTS;

    //Disable modem specific lines, canonical mode, echo and signals
    ttyDevice.c_cflag |= CREAD | CLOCAL;
    ttyDevice.c_lflag &= ~ICANON;
    ttyDevice.c_lflag &= ~ECHO;
    ttyDevice.c_lflag &= ~ISIG;

    //Disable Sw flow control and handling of special bytes
    ttyDevice.c_iflag &= ~(IXON | IXOFF | IXANY);
    ttyDevice.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL);

    //Disable replace \n with \r\n
    ttyDevice.c_oflag &= ~OPOST;
    ttyDevice.c_oflag &= ~ONLCR;

    //Blocking read, with a timeout of 1 second
    ttyDevice.c_cc[VMIN]  = 1;
    ttyDevice.c_cc[VTIME] = 10;

    //Try to set the new attributes
    ret = tcsetattr(serialPort->fileDescriptor, TCSANOW, &ttyDevice);
    if (ret < 0)
        return -errno;

    //If reached this far, device should be initialized correctly
    serialPort->initialized = 1;

    //Set the terminator to a default value
    serialPort->terminator = DEFAULT_TERMINATOR;

    return 0;
}

int serial_close(serial_port_st* serialPort)
{
    int ret;

    if (!serialPort)
        return -EINVAL;

    if (!serialPort->initialized)
        return -EPERM;

    //Try to revert the serial ports configurations to its original state
    ret = tcsetattr(serialPort->fileDescriptor, TCSANOW, &serialPort->defaultConfig);
    if (ret < 0)
        return -errno;


    //Finally try to close the file descriptor
    ret = close(serialPort->fileDescriptor);
    if (ret < 0)
        return -errno;

    //If reached this far, device should have been closed properly
    serialPort->initialized = 0;

    return 0;
}

int serial_write(serial_port_st* serialPort, const uint8_t* txBuffer, size_t bufferSize)
{
    size_t ret;

    if (!serialPort || !txBuffer)
        return -EINVAL;

    if (!serialPort->initialized)
        return -EPERM;

    ret = write(serialPort->fileDescriptor, txBuffer, bufferSize);
    //Write returns the number of bytes writen, if the number of bytes writen doesn't match the passed buffer length
    //return an error
    if (ret != bufferSize)
        return -EIO;

    return 0;
}

int serial_read(serial_port_st* serialPort, uint8_t* rxBuffer, size_t bufferSize)
{
    if (!serialPort || !rxBuffer)
        return -EINVAL;

    if (!serialPort->initialized)
        return -EPERM;

    return serial_read_internal(serialPort, rxBuffer, bufferSize);
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
            continue;

        rxBuffer[bytesRead++] = ch;

        //If the received character matches the set terminator, exit and return the number of bytes that was read
        if (ch == serialPort->terminator)
            return (int)bytesRead;
    }

    return 0;
}

int serial_set_terminator(serial_port_st* serialPort, char newTerminator)
{
    if (!serialPort)
        return -EINVAL;

    if (!serialPort->initialized)
        return -EPERM;

    serialPort->terminator = newTerminator;

    return 0;
}

int serial_flush(serial_port_st* serialPort)
{
    int ret;
    
    if (!serialPort)
        return -EINVAL;

    if (!serialPort->initialized)
        return -EPERM;

    ret = tcflush(serialPort->fileDescriptor, TCIOFLUSH);
    if (ret < 0)
        return -errno;
    
    return 0;
}

/***********************************************************************************************************************
 * Private Functions
 **********************************************************************************************************************/
int serial_read_internal(serial_port_st* serialPort, uint8_t* rxBuffer, size_t bufferSize)
{
    ssize_t ret;

    ret = read(serialPort->fileDescriptor, rxBuffer, bufferSize);
    if (ret < 0)
        return -errno;

    return (int)ret;
}
#endif