#include <stdio.h>
#include "wifi_scan.h"

int main()
{
    struct bss_info scannedNetworks[16];
    struct wifi_scan* interface = wifi_scan_init("wlan0");
    int status = wifi_scan_all(interface, scannedNetworks, 16);

    for (int i = 0; i < status; ++i)
    {
        struct bss_info* networkInfo = &scannedNetworks[i];

        printf("SSID: %s\n", networkInfo->ssid);
    }

    return 0;
}
