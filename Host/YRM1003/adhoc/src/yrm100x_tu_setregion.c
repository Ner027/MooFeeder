#include "../inc/yrm100x_tu.h"

yrm100x_st dev;

int main()
{
    int ret;

    ret = yrm100x_init(&dev, SERIAL_PORT);
    MY_ASSERT(ret == 0);

    ret = yrm100x_set_region(&dev, REGION_CHINA900);
    MY_ASSERT(ret == 0);
    ret = yrm100x_set_region(&dev, REGION_USA);
    MY_ASSERT(ret == 0);
    ret = yrm100x_set_region(&dev, REGION_EUROPE);
    MY_ASSERT(ret == 0);
    ret = yrm100x_set_region(&dev, REGION_CHINA800);
    MY_ASSERT(ret == 0);
    ret = yrm100x_set_region(&dev, REGION_KOREA);
    MY_ASSERT(ret == 0);

    return 0;
}