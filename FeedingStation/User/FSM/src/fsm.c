#include <sys/errno.h>
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include "../inc/fsm.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "main.h"

#define ENABLE_FSM_LOGS
#define DUMMY_CONSUMPTION

#ifdef ENABLE_FSM_LOGS
#include "../../util/inc/util.h"
#include "../../RFID/inc/rfid.h"
#include "../../LoRa/app/common/inc/app_types.h"
#include "../../LoRa/network/client/inc/network_client.h"
#include "stm32l4xx_hal_gpio.h"

#define FSM_LOG(format, ...) print("FSM", format, ##__VA_ARGS__)
#else
#define FSM_LOG(format, ...) do{}while(0)
#endif

typedef enum
{
    ST_INIT,
    ST_EVAL,
    ST_REQUEST_BUDGET,
    ST_GET_BUDGET,
    ST_REPORT_CONSUMPTION,
    ST_FEED,
    ST_OPEN,
    ST_CLOSE,
    ST_IDLE,
    ST_FAULT
}fsm_state_et;

static fsm_state_et currentState, nextState;
static SemaphoreHandle_t keepRunning;
static uint64_t currentRfidTag;
static float currentAllowedConsumption;
static float consumedVolume;
static network_frame_st currentNetFrame;

static void state_init();
static void state_eval();
static void state_idle();
static void state_fault();
static void state_get_budget();
static void state_request_budget();
static void state_report_consumption();
static void state_feed();
static void state_open();
static void state_close();

int fsm_init()
{
    nextState = ST_INIT;
    keepRunning = xSemaphoreCreateBinary();
    if (!keepRunning)
    {
        FSM_LOG("Failed to allocate FreeRTOS Resources!\n");
        return -ENOMEM;
    }

    return 0;
}

void fsm_run()
{
    while (xSemaphoreTake(keepRunning, 0) == pdFALSE)
    {
        currentState = nextState;
        switch (currentState)
        {
            case ST_INIT:
                state_init();
                break;
            case ST_EVAL:
                state_eval();
                break;
            case ST_IDLE:
                state_idle();
                break;
            case ST_FAULT:
                state_fault();
                break;
            case ST_REQUEST_BUDGET:
                state_request_budget();
                break;
            case ST_GET_BUDGET:
                state_get_budget();
                break;
            case ST_FEED:
                state_feed();
                break;
            case ST_OPEN:
                state_open();
                break;
            case ST_CLOSE:
                state_close();
                break;
            case ST_REPORT_CONSUMPTION:
                state_report_consumption();
                break;
            default:
                break;
        }
    }
}

static void state_init()
{
    int ret;

#ifdef DUMMY_CONSUMPTION
    srand(xTaskGetTickCount());
#endif

    nextState = ST_EVAL;

    ret = rfid_init();
    if (ret < 0)
    {
        FSM_LOG("Failed to initialize RFID Module!\n");
        nextState = ST_FAULT;
    }

    ret = network_init();
    if (ret < 0)
    {
        FSM_LOG("Failed to initialize LoRa Network!\n");
        nextState = ST_FAULT;
    }
}

static void state_eval()
{
    int ret;

    ret = rfid_get_current_tag(&currentRfidTag);
    if (ret < 0)
        nextState = ST_IDLE;
    else nextState = ST_REQUEST_BUDGET;
}

static void state_request_budget()
{
    int ret;

    app_frame_st* pAppFrame = (app_frame_st*) currentNetFrame.payload;
    budget_request_st* pBudgetReq = (budget_request_st*) pAppFrame->payload;

    memcpy(pBudgetReq->rfidTag, &currentRfidTag, RFID_TAG_LEN);
    pAppFrame->control.frameType = BUDGET_REQUEST;

    currentNetFrame.control.payloadLen = sizeof(app_control_st) + sizeof(budget_request_st);

    FSM_LOG("Requesting budget for calf with tag: 0x%.2llx\n", currentRfidTag);

    ret = network_send(&currentNetFrame, MAX_DELAY);
    if (ret < 0)
    {
        FSM_LOG("Failed to send message to network!\n");
        nextState = ST_EVAL;
    }
    else
    {
        nextState = ST_GET_BUDGET;
    }
}

static void state_report_consumption()
{
    int ret;
    app_frame_st* pAppFrame;
    consumption_report_st* pReport;

    if (consumedVolume < 0.1f)
    {
        nextState = ST_IDLE;
        return;
    }

    pAppFrame = (app_frame_st*) currentNetFrame.payload;
    pAppFrame->control.frameType = CONSUMPTION_REPORT;
    pReport = (consumption_report_st*) pAppFrame->payload;

#ifdef DUMMY_CONSUMPTION
     consumedVolume = (float) (rand() % (uint32_t)currentAllowedConsumption);
#endif

    memcpy(&pReport->volumeConsumed, &consumedVolume, sizeof(uint32_t));
    memcpy(&pReport->rfidTag, &currentRfidTag, RFID_TAG_LEN);

    ret = network_send(&currentNetFrame, MAX_DELAY);
    if (ret < 0)
        printf("Failed to submit consumption report!\n");

    nextState = ST_EVAL;
}

static void state_get_budget()
{
    int ret;
    app_frame_st* pAppFrame;
    budget_response_st* pBudgetResp;
    float* pTemp;

    ret = network_receive(&currentNetFrame, MAX_DELAY);
    if (ret < 0)
    {
        FSM_LOG("Failed reading from network!\n");
        nextState = ST_EVAL;
    }

    pAppFrame = (app_frame_st*) currentNetFrame.payload;

    if (pAppFrame->control.frameType != BUDGET_RESPONSE)
    {
        FSM_LOG("Received unexpected message! Expecting a budget response, frame type was: %d\n", pAppFrame->control.frameType);
        nextState = ST_IDLE;
        return;
    }

    pBudgetResp = (budget_response_st*) pAppFrame->payload;
    uint64_t newTag;
    memcpy(&newTag, &currentRfidTag, RFID_TAG_LEN);
    if (newTag != currentRfidTag)
    {
        FSM_LOG("Received budget response message from other calf! Received Tag: 0x%llx | Expected Tag: 0x%llx\n", newTag, currentRfidTag);
        nextState = ST_IDLE;
        return;
    }

    //If performing a cast to a float straight from an integer, it will result in just a float representation of its integer value
    //What we want here is to interpret the binary value of that integer as a float, this can be achieved using this kind of casts
    pTemp = (float*) &pBudgetResp->allowedConsumption;
    currentAllowedConsumption = *pTemp;

    FSM_LOG("Received budget response to calf: 0x%llx with allowed consumption of: %f\n", currentRfidTag, currentAllowedConsumption);

    if (currentAllowedConsumption <= 0.01f)
    {
        FSM_LOG("Current calf exceeded consumption budget!\n");
        nextState = ST_IDLE;
    }
    else
    {
        nextState = ST_OPEN;
    }
}

static void state_feed()
{
    int ret;
    uint64_t newTag;

    ret = rfid_get_current_tag(&newTag);
    if (ret < 0)
    {
        if (ret != -ENODATA)
            FSM_LOG("Failed to get current RFID Tag value! Exited with error code: (%d)\n", ret);

        nextState = ST_CLOSE;
    }

    if (newTag != currentRfidTag)
    {
        nextState = ST_CLOSE;
        return;
    }

    consumedVolume += 0.1f;

    if (consumedVolume >= currentAllowedConsumption)
        nextState = ST_CLOSE;
    else nextState = ST_FEED;
}

static void state_open()
{
    HAL_GPIO_WritePin(VALVE_GPIO_Port, VALVE_Pin, GPIO_PIN_SET);
    consumedVolume = 0;
    nextState = ST_FEED;
}

static void state_close()
{
    HAL_GPIO_WritePin(VALVE_GPIO_Port, VALVE_Pin, GPIO_PIN_RESET);
    nextState = ST_REPORT_CONSUMPTION;
}

static void state_idle()
{
    vTaskDelay(500/portTICK_PERIOD_MS);
    nextState = ST_EVAL;
}

static void state_fault()
{
    while (1);
}