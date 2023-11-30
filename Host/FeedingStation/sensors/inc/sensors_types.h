#ifndef FEEDINGSTATION_SENSORS_TYPES_H
#define FEEDINGSTATION_SENSORS_TYPES_H
#include <stdint.h>

typedef struct
{
    uint16_t rawVbat;
    uint16_t rawTemp;
}sensor_data_st;

#endif