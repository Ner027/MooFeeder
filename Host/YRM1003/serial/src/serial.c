#include <fcntl.h>
#include <termios.h>
#include <asm-generic/errno.h>
#include <memory.h>
#include <unistd.h>
#include <stdint.h>
#include "../inc/serial.h"

#define LINE_TERMINATOR 0x7E

#ifdef __linux
///
/// \param port Serial port inode name, ie. /dev/tty...
/// \param br Baud rate
/// \param serialPort Serial port handler struct
/// \return 0 if everything went out correctly, otherwise returns error codes < 0
int serial_open(char* port, int br, serial_port_st* serialPort)
{
    int ret;
    struct termios ttyDevice;

    if (!serialPort || !port)
        return -EINVAL;

    ret = open(port, O_RDWR);

    if (ret < 0)
        return ret;

    serialPort->fileDescriptor = ret;

    ret = tcgetattr(serialPort->fileDescriptor, &ttyDevice);

    if (ret < 0)
        return ret;

    //Save current port configuration before changing it
    memcpy(&serialPort->defaultConfig, &ttyDevice, sizeof(struct termios));

    ret = cfsetspeed(&ttyDevice, br);

    if (ret < 0)
        return ret;

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

    ret = tcsetattr(serialPort->fileDescriptor, TCSANOW, &ttyDevice);

    return ret;
}

/// \brief Closes a previously opened serial port, returns its configs to their original state
/// \param serialPort Serial port handler
/// \return 0 if everything went out correctly, otherwise returns error codes < 0
int serial_close(serial_port_st* serialPort)
{
    int ret;

    if (!serialPort)
        return -EINVAL;

    ret = tcsetattr(serialPort->fileDescriptor, TCSANOW, &serialPort->defaultConfig);

    if (ret < 0)
        return ret;

    ret = close(serialPort->fileDescriptor);

    return ret;
}

int serial_write(serial_port_st* serialPort, const uint8_t* txBuffer, size_t bufferSize)
{
    size_t ret;

    if (!serialPort || !txBuffer)
        return -EINVAL;

    ret = write(serialPort->fileDescriptor, txBuffer, bufferSize);

    if (ret != bufferSize)
        return -EIO;

    return 0;
}

int serial_read(serial_port_st* serialPort, uint8_t* rxBuffer, size_t bufferSize)
{
    ssize_t ret;

    if (!serialPort || !rxBuffer)
        return -EINVAL;

    ret = read(serialPort->fileDescriptor, rxBuffer, bufferSize);

    if (ret < 0)
        return -EFAULT;

    return (int)ret;
}

int serial_sync_readline(serial_port_st* serialPort, uint8_t* rxBuffer, size_t bufferSize)
{
    int ret;
    size_t bytesRead = 0;
    uint8_t ch;

    while ((bytesRead < bufferSize))
    {
        ret = serial_read(serialPort, &ch, 1);

        if (ret < 0)
            continue;

        rxBuffer[bytesRead++] = ch;

        if (ch == LINE_TERMINATOR)
            return (int)bytesRead;
    }

    return 0;
}

int serial_flush(serial_port_st* serialPort)
{
    if (!serialPort)
        return -EINVAL;

    tcflush(serialPort->fileDescriptor, TCIOFLUSH);

    return 0;
}

#endif