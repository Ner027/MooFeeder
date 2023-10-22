#include <stddef.h>
#include <asm-generic/errno.h>
#include <stdio.h>
#include "../inc/yrm100x_tu.h"
#include "../../platform/inc/platform.h"

#define READ_TRIES 5

yrm100x_st dev;

int main()
{
    int ret;
    uint8_t i, tryCount;
    yrm100x_ret_st yrmRet;
    yrm100x_inv_ut yrmInv;

    ret = yrm100x_init(&dev, SERIAL_PORT);
    MY_ASSERT(ret == 0);

    tryCount = READ_TRIES;

    while (tryCount--)
    {
        ret = yrm100x_write_command(&dev, CMD_INV_SINGLE, NULL, 0);
        MY_ASSERT(ret == 0);

        ret = yrm100x_read_parse(&dev, &yrmRet, yrmInv.raw, 17);

        if (ret < 0)
        {
            if ((ret == -EFAULT) && (yrmRet.errCode == ERR_INV_FAIL))
            {
                printf("No tag read!\n");
            }
            else
            {
                printf("Exited with code: 0x%x and error code: 0x%x\n", ret, yrmRet.errCode);
                return 1;
            }
        }
        else
        {
            printf("Tag EPC: ");

            for (i = 0; i < TAG_EPC_LEN; ++i)
                printf("%x", yrmInv.data.epc[i]);

            printf("\n");
        }
    }

    return 0;
}
