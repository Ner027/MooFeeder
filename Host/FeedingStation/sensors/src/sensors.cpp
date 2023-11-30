#include "../inc/sensors.h"
#include "../inc/sensors_types.h"
#include "../../oswrapper/inc/oswrapper.h"

#ifdef ENABLE_SENSORS_LOG
#include "../../util/inc/util.h"
#define SENS_LOG(format, ...) print("SENSORS", format, ##__VA_ARGS__)
#else
#define SENS_LOG(format, ...) do{}while(0)
#endif

#define MAX_SENSOR_MESSAGES 4
#define SAMPLING_PERIOD_MS 5000

QueueHandle_st qSensorData;
static sensor_data_st currentSample = {0};

#define ENABLE_SENSORS_LOG

#ifdef HOST
void t_sensors()
#elif defined(TARGET)
void t_sensors(void* args)
#endif
{
    sensor_data_st dummySample;
    int ret = QueueInit(&qSensorData, MAX_SENSOR_MESSAGES, sizeof(sensor_data_st));
    if (!ret)
    {
        SENS_LOG("Failed to create sensor data queue!\n");
        return;
    }

    while (1)
    {
        //TODO: Add sample sensors
        currentSample.rawTemp += 2;
        currentSample.rawVbat += 4;

        ret = QueueFreeSpace(&qSensorData);
        if (ret == 0)
            QueuePop(&qSensorData, &dummySample, SYSTEM_TICK_FROM_MS(0));

        ret = QueuePush(&qSensorData, &currentSample, SYSTEM_TICK_FROM_MS(0));
        if (ret < 0)
            SENS_LOG("Failed to insert sample!\n");

        THREAD_SLEEP_FOR(SYSTEM_TICK_FROM_MS(SAMPLING_PERIOD_MS));
    }

    SENS_LOG("Sensors Task Killed!\n");
}