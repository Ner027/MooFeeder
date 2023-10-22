#include <stdint-gcc.h>
#include <stdio.h>
#include <asm-generic/errno.h>
#include "serial/inc/serial.h"
#include "yrm100x/inc/yrm100x.h"
#include "platform/inc/platform.h"

int main()
{
    yrm100x_st rfid;
    uint8_t args, i;
    yrm100x_ret_st yrmRet;
    yrm100x_inv_ut inv;
    uint8_t rxBuffer[32];
    int ret;

    yrm100x_init(&rfid, "/dev/ttyUSB0");

    args = 0;
    yrm100x_write_command(&rfid, CMD_GET_INFO, &args, 1);
    ret = yrm100x_read_parse(&rfid, &yrmRet, rxBuffer, 32);

    if (ret != 0)
        return 0;

    printf("%s\n", rxBuffer);

    ret = yrm100x_get_region(&rfid);

    printf("Region: %d\n", ret);

    ret = yrm100x_set_region(&rfid, REGION_USA);

    ret = yrm100x_get_region(&rfid);

    printf("Region: %d\n", ret);

    while (1)
    {
        delay_ms(300);
        yrm100x_write_command(&rfid, CMD_INV_SINGLE, NULL, 0);
        ret = yrm100x_read_parse(&rfid, &yrmRet, inv.raw, 17);

        if (ret < 0)
        {
            if ((ret == -EFAULT) && (yrmRet.errCode == ERR_INV_FAIL))
                printf("No tag read!\n");
            else printf("Exited with code: 0x%x and error code: 0x%x\n", ret, yrmRet.errCode);
        }
        else
        {
            for (i = 0; i < 12; ++i)
                printf("%x", inv.data.epc[i]);

            printf("\n");
        }
    }

    return 0;
}
