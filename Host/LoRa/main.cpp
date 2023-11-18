#include "serial/inc/serial.h"
#include "rn2483/inc/rn2483.h"
#include "mac/server/inc/cloramac.h"
#include "mac/client/inc/mac_client.h"

static serial_port_st loraPort;
static rn2483_st loraRadio;

int main()
{
    //CLoRaMac loraMac;
    //loraMac.join();

    mac_init();

    while (1);

    return 0;
}
