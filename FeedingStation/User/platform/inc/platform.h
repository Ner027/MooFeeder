#ifndef UT_PLATFORM_H
#define UT_PLATFORM_H

#include <unistd.h>

#if defined(_WIN32) || defined(__WIN64) || defined(__unix__) || defined(__linux) || defined(__linux__)
#define HOST
#else
#define TARGET
#endif

#endif

#ifdef HOST
#define GPIO_SET_LEVEL(p, n, l) do{} while(0)
#define delay_ms(x) usleep(x * 1000)
#define GET_BYTE(x, n) ((x) >> (n << 3))
#define CONCAT_BYTES(l, m) (((m) << 8) | (l))
#elif defined(TARGET)
#define GPIO_SET_LEVEL(p, n, l)  HAL_GPIO_WritePin((p), (n), (l))
#define delay_ms(x) vTaskDelay((x) / portTICKPERIOD_MS)
#define GET_BYTE(x, n) ((x) >> (n << 3))
#define CONCAT_BYTES(l, m) (((m) << 8) | (l))
#endif

#define YRM_EN_PORT RFID_EN_GPIO_Port
#define YRM_EN_PIN RFID_EN_Pin

