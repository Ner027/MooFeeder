#include "serial/inc/serial.h"
#include "rn2483/inc/rn2483.h"
#include "mac/server/inc/cloramac.h"
#include "mac/client/inc/mac_client.h"

static serial_port_st loraPort;
static rn2483_st loraRadio;

//#define SERVER
int main()
{
#ifdef SERVER
    CLoRaMac loraMac;
    loraMac.join();
#else
    mac_init();
#endif

    while (1);

    return 0;
}
