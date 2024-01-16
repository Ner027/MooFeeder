#ifndef LORA_APP_TYPES_H
#define LORA_APP_TYPES_H

/***********************************************************************************************************************
 * Includes
 ******************************************************************************************************************++**/
#include <cstdint>
#include "oswrapper/oswrapper.h"

/***********************************************************************************************************************
 * Defines
 ******************************************************************************************************************++**/
#define RFID_TAG_LEN 8
#define APP_PAYLOAD_LEN 16
#define TEXT_PAYLOAD_LEN 32
#define APP_CTRL_LEN sizeof(app_control_st)

/***********************************************************************************************************************
 * Typedefs
 ******************************************************************************************************************++**/
typedef ENUM1B(AppFrameType)
{
    BUDGET_REQUEST      = 0x00,
    BUDGET_RESPONSE     = 0x01,
    CONSUMPTION_REPORT  = 0x02,
    SENSOR_DATA         = 0x03,
    TEXT_MESSAGE        = 0x04
}app_frame_type_et;

typedef struct
{
    app_frame_type_et frameType;
}app_control_st;

typedef struct
{
    app_control_st control;
    uint8_t payload[APP_PAYLOAD_LEN];
}app_frame_st;

typedef struct
{
    uint8_t rfidTag[RFID_TAG_LEN];
}budget_request_st;

typedef struct
{
    uint8_t rfidTag[RFID_TAG_LEN];
    uint32_t allowedConsumption;
}budget_response_st;

typedef struct
{
    uint8_t rfidTag[RFID_TAG_LEN];
    uint32_t volumeConsumed;
}consumption_report_st;

typedef struct
{
    uint16_t rawTemperature;
    uint16_t rawVbat;
}sensor_data_st;

typedef struct
{
    uint8_t messageSize;
    char textMessage[TEXT_PAYLOAD_LEN];
}text_msg_st;

const uint8_t appLenLut[] =
{
    sizeof(budget_request_st),
    sizeof(budget_response_st),
    sizeof(consumption_report_st),
    sizeof(sensor_data_st),
    sizeof(text_msg_st)
};

#endif