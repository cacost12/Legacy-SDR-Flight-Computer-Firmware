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
#include "stm32h7xx_hal.h"

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
#define AUX2_CONT_Pin GPIO_PIN_6
#define AUX2_CONT_GPIO_Port GPIOE
#define AUX2_Pin GPIO_PIN_13
#define AUX2_GPIO_Port GPIOC
#define AUX1_Pin GPIO_PIN_14
#define AUX1_GPIO_Port GPIOC
#define SWITCH_Pin GPIO_PIN_15
#define SWITCH_GPIO_Port GPIOC
#define AUX1_CONT_Pin GPIO_PIN_0
#define AUX1_CONT_GPIO_Port GPIOC
#define MAIN_Pin GPIO_PIN_1
#define MAIN_GPIO_Port GPIOC
#define DROGUE_Pin GPIO_PIN_2
#define DROGUE_GPIO_Port GPIOC
#define DROGUE_CONT_Pin GPIO_PIN_3
#define DROGUE_CONT_GPIO_Port GPIOC
#define MAIN_CONT_Pin GPIO_PIN_0
#define MAIN_CONT_GPIO_Port GPIOA
#define STATUS_R_Pin GPIO_PIN_9
#define STATUS_R_GPIO_Port GPIOE
#define STATUS_B_Pin GPIO_PIN_11
#define STATUS_B_GPIO_Port GPIOE
#define STATUS_G_Pin GPIO_PIN_13
#define STATUS_G_GPIO_Port GPIOE
#define IMU_SCL_Pin GPIO_PIN_10
#define IMU_SCL_GPIO_Port GPIOB
#define IMU_SDA_Pin GPIO_PIN_11
#define IMU_SDA_GPIO_Port GPIOB
#define FLASH_SS_Pin GPIO_PIN_12
#define FLASH_SS_GPIO_Port GPIOB
#define USB_DETECT_Pin GPIO_PIN_10
#define USB_DETECT_GPIO_Port GPIOD
#define FLASH_WP_Pin GPIO_PIN_12
#define FLASH_WP_GPIO_Port GPIOD
#define FLASH_HOLD_Pin GPIO_PIN_13
#define FLASH_HOLD_GPIO_Port GPIOD
#define RX_USB_Pin GPIO_PIN_6
#define RX_USB_GPIO_Port GPIOC
#define TX_USB_Pin GPIO_PIN_7
#define TX_USB_GPIO_Port GPIOC
#define USB_SUSPEND_Pin GPIO_PIN_8
#define USB_SUSPEND_GPIO_Port GPIOA
#define USB_RST_Pin GPIO_PIN_12
#define USB_RST_GPIO_Port GPIOA
#define BP_SCL_Pin GPIO_PIN_6
#define BP_SCL_GPIO_Port GPIOB
#define BP_SDA_Pin GPIO_PIN_7
#define BP_SDA_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
