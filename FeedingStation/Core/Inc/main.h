/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define VALVE_Pin GPIO_PIN_0
#define VALVE_GPIO_Port GPIOA
#define METER_Pin GPIO_PIN_1
#define METER_GPIO_Port GPIOA
#define METER_EXTI_IRQn EXTI1_IRQn
#define RN2483_EN_Pin GPIO_PIN_4
#define RN2483_EN_GPIO_Port GPIOA
#define VBAT_Pin GPIO_PIN_0
#define VBAT_GPIO_Port GPIOB
#define RFID_EN_Pin GPIO_PIN_8
#define RFID_EN_GPIO_Port GPIOA
#define REG_5V_EN_Pin GPIO_PIN_3
#define REG_5V_EN_GPIO_Port GPIOH
#define REG_3V3_EN_Pin GPIO_PIN_9
#define REG_3V3_EN_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
