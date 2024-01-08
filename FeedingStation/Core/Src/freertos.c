/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <sys/errno.h>
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"
#include "../../User/LoRa/oswrapper/inc/oswrapper.h"
#include "../../User/LoRa/mac/client/inc/mac_client.h"
#include "usbd_cdc.h"
#include "../../User/LoRa/network/client/inc/network_client.h"
#include "../../User/LoRa/app/common/inc/app_types.h"
#include "../../User/yrm100x/inc/yrm100x_types.h"
#include "../../User/yrm100x/inc/yrm100x.h"
#include "usart.h"
#include "../../User/RFID/inc/rfid.h"
#include "../../User/FSM/inc/fsm.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
extern USBD_HandleTypeDef hUsbDeviceFS;
/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 128 * 8,
  .priority = 28,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);

extern void MX_USB_DEVICE_Init(void);
void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
    /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
    /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
    /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
    /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
    /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
    /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* init code for USB_DEVICE */
    MX_USB_DEVICE_Init();
  /* USER CODE BEGIN StartDefaultTask */
    HAL_GPIO_WritePin(REG_3V3_EN_GPIO_Port, REG_3V3_EN_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(REG_5V_EN_GPIO_Port, REG_5V_EN_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(RFID_EN_GPIO_Port, RFID_EN_Pin, GPIO_PIN_SET);

    THREAD_SLEEP_FOR(SYSTEM_TICK_FROM_MS(1000));
    printf("App Started\n");
    THREAD_SLEEP_FOR(SYSTEM_TICK_FROM_MS(1000));


    fsm_init();
    fsm_run();

  /* USER CODE END StartDefaultTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
int _write(int file, char *ptr, int len)
{
    USBD_CDC_SetTxBuffer(&hUsbDeviceFS, (uint8_t *) ptr, len);
    USBD_CDC_TransmitPacket(&hUsbDeviceFS);
    return len;
}
/* USER CODE END Application */

