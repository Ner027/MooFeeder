#include "../inc/platform.h"
#include "../../yrm100x/inc/yrm100x.h"
#ifdef TARGET
#include "../../serial/inc/serial.h"
#endif

#ifdef TARGET
static serial_port_st serialPortLog;
#endif

#define ENABLE_BSP_LOGS
#ifdef ENABLE_BSP_LOGS
#define BSP_LOG(format, ...) print("BSP", format, ##__VA_ARGS__)
#else
#define BSP_LOG(format, ...) do{}while(0)
#endif

int platform_init()
{
    int ret;

#ifdef TARGET
    //TODO: Init Log Serial Port
#endif


    return 0;
}

#ifdef TARGET
int _write(int file, char *ptr, int len)
{
    serial_write(&serialPortLog, (uint8_t*) ptr, len);
    return len;
}
#endif