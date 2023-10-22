#include "../inc/yrm100x_tu.h"

yrm100x_st dev;

int main()
{
    int ret;

    ret = yrm100x_init(&dev, SERIAL_PORT);
    MY_ASSERT(ret == 0);

    ret = yrm100x_get_region(&dev);
    MY_ASSERT((ret < REGION_KOREA) && (ret != 5) && (ret >= 0));

    return 0;
}
