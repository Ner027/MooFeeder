#include "../inc/rfid.h"
#include "../../yrm100x/inc/yrm100x.h"
#include "../../oswrapper/inc/oswrapper.h"

yrm100x_st devRfid;
#define RFID_SAMPLE_PERIOD_MS 200

#define ENABLE_RFID_LOGS
#include "../../util/inc/util.h"
#include "../../platform/inc/platform.h"

#ifdef ENABLE_RFID_LOGS
#define RFID_LOG(format, ...) print("RFID", format, ##__VA_ARGS__)
#else
#define RFID_LOG(format, ...) do{}while(0)
#endif


#ifdef HOST
void t_scan_rfid()
#elif defined(TARGET)
void t_scan_rfid(void* args)
#endif
{
    int ret;
    yrm100x_ret_st rfidRc;
    yrm100x_inv_ut rfidInv;

    yrm100x_init(&devRfid, (void *) YRM_SERIAL_PORT);
    if (ret < 0)
    {
        RFID_LOG("Failed to initialize RFID Reader!\n");
        return;
    }

    RFID_LOG("RFID Reader initialized with success!\n");


    while (1)
    {
        ret = yrm100x_write_command(&devRfid, CMD_INV_SINGLE, null, 0);
        if (ret < 0)
        {
            RFID_LOG("Failed write to RFID device\n");
            continue;
        }

        ret = yrm100x_read_parse(&devRfid, &rfidRc, rfidInv.raw, sizeof(yrm100x_inv_st));
        if (ret < 0)
        {
            if ((ret == -EFAULT) && (rfidRc.errCode == ERR_INV_FAIL))
            {
                RFID_LOG("No tag read!\n");
            }
            else
            {
                RFID_LOG("Error while trying to perform an RFID read! Exited with code: %d\n", ret);
            }
        }
        else
        {
            RFID_LOG()
        }
        THREAD_SLEEP_FOR(SYSTEM_TICK_FROM_MS(RFID_SAMPLE_PERIOD_MS));
    }
}