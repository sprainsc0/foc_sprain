/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
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
#include "stm32f4xx_hal.h"

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
#define Current1_Pin GPIO_PIN_0
#define Current1_GPIO_Port GPIOC
#define Current2_Pin GPIO_PIN_1
#define Current2_GPIO_Port GPIOC
#define Current3_Pin GPIO_PIN_2
#define Current3_GPIO_Port GPIOC
#define AN_IN_Pin GPIO_PIN_3
#define AN_IN_GPIO_Port GPIOC
#define Voltage1_Pin GPIO_PIN_0
#define Voltage1_GPIO_Port GPIOA
#define Voltage2_Pin GPIO_PIN_1
#define Voltage2_GPIO_Port GPIOA
#define Voltage3_Pin GPIO_PIN_2
#define Voltage3_GPIO_Port GPIOA
#define ADC_Temp_Pin GPIO_PIN_3
#define ADC_Temp_GPIO_Port GPIOA
#define SPI1_CS_Pin GPIO_PIN_4
#define SPI1_CS_GPIO_Port GPIOA
#define MOTOR_Temp_Pin GPIO_PIN_4
#define MOTOR_Temp_GPIO_Port GPIOC
#define ADC_IN_Pin GPIO_PIN_5
#define ADC_IN_GPIO_Port GPIOC
#define LED_GREEN_Pin GPIO_PIN_0
#define LED_GREEN_GPIO_Port GPIOB
#define LED_RED_Pin GPIO_PIN_1
#define LED_RED_GPIO_Port GPIOB
#define L1_Pin GPIO_PIN_13
#define L1_GPIO_Port GPIOB
#define L2_Pin GPIO_PIN_14
#define L2_GPIO_Port GPIOB
#define L3_Pin GPIO_PIN_15
#define L3_GPIO_Port GPIOB
#define SPI3_CS_Pin GPIO_PIN_9
#define SPI3_CS_GPIO_Port GPIOC
#define H1_Pin GPIO_PIN_8
#define H1_GPIO_Port GPIOA
#define H2_Pin GPIO_PIN_9
#define H2_GPIO_Port GPIOA
#define H3_Pin GPIO_PIN_10
#define H3_GPIO_Port GPIOA
#define LED_GREEND2_Pin GPIO_PIN_2
#define LED_GREEND2_GPIO_Port GPIOD
#define GATE_EN_Pin GPIO_PIN_5
#define GATE_EN_GPIO_Port GPIOB
#define Fault_Pin GPIO_PIN_7
#define Fault_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
