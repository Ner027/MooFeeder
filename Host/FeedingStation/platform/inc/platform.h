#ifndef FEEDINGSTATION_PLATFORM_H
#define FEEDINGSTATION_PLATFORM_H

#include "../../common.h"
#include "../../util/inc/util.h"

#define YRM_EN_PORT 0
#define YRM_EN_PIN 0

#ifdef HOST
#include <stdio.h>
#include <unistd.h>
#define GPIO_SET_LEVEL(p, n, l) printf("[GPIO] Pin: %d from Port: %d Set to: %d\n", (p), (n), (l))
#define delay_ms(x) usleep((x) * 1000)
#define GET_BYTE(x, n) ((x) >> (n << 3))
#define CONCAT_BYTES(l, m) (((m) << 8) | (l))

#define YRM_SERIAL_PORT "/dev/ttyUSB0"

#elif defined(TARGET)
#define GPIO_SET_LEVEL(p, n, l)  HAL_GPIO_WritePin((p), (n), (l))
#define delay_ms(x) vTaskDelay((x) / portTICKPERIOD_MS)
#define LOG_PORT
#endif

#ifdef __cplusplus
extern "C"{
#endif
int platform_init();
#ifdef __cplusplus
}
#endif
#endif