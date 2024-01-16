#ifndef CONTROLBOX_STATION_TYPES_H
#define CONTROLBOX_STATION_TYPES_H


/***********************************************************************************************************************
 * Typedefs
 ******************************************************************************************************************++**/
typedef struct
{
    uint32_t phyAddr;
    uint8_t netAddr;
    float currBattery;
    float currTemperature;
}station_data_st;

#endif
