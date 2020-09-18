/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
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
#include "stm32f3xx_hal.h"

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
#define LED_BLUE_Pin GPIO_PIN_0
#define LED_BLUE_GPIO_Port GPIOC
#define LED_GREEN_Pin GPIO_PIN_1
#define LED_GREEN_GPIO_Port GPIOC
#define GPIO_TEST_1_Pin GPIO_PIN_2
#define GPIO_TEST_1_GPIO_Port GPIOC
#define GPIO_TEST_2_Pin GPIO_PIN_3
#define GPIO_TEST_2_GPIO_Port GPIOC
#define ADC1_mA_A_Pin GPIO_PIN_0
#define ADC1_mA_A_GPIO_Port GPIOA
#define ADC1_mA_B_Pin GPIO_PIN_1
#define ADC1_mA_B_GPIO_Port GPIOA
#define ADC1_mA_C_Pin GPIO_PIN_2
#define ADC1_mA_C_GPIO_Port GPIOA
#define ADC2_mV_A_Pin GPIO_PIN_4
#define ADC2_mV_A_GPIO_Port GPIOA
#define ADC2_mV_B_Pin GPIO_PIN_5
#define ADC2_mV_B_GPIO_Port GPIOA
#define ADC2_mV_C_Pin GPIO_PIN_6
#define ADC2_mV_C_GPIO_Port GPIOA
#define MOS_EN_A_Pin GPIO_PIN_7
#define MOS_EN_A_GPIO_Port GPIOA
#define MOS_EN_B_Pin GPIO_PIN_4
#define MOS_EN_B_GPIO_Port GPIOC
#define MOS_EN_C_Pin GPIO_PIN_5
#define MOS_EN_C_GPIO_Port GPIOC
#define ADC3_HALL_C_Pin GPIO_PIN_0
#define ADC3_HALL_C_GPIO_Port GPIOB
#define ADC3_HALL_A_Pin GPIO_PIN_1
#define ADC3_HALL_A_GPIO_Port GPIOB
#define ADC4_mV_BUS_Pin GPIO_PIN_12
#define ADC4_mV_BUS_GPIO_Port GPIOB
#define ADC3_HALL_B_Pin GPIO_PIN_13
#define ADC3_HALL_B_GPIO_Port GPIOB
#define ADC4_TEMP_Pin GPIO_PIN_14
#define ADC4_TEMP_GPIO_Port GPIOB
#define SPI1_CS_3_Pin GPIO_PIN_11
#define SPI1_CS_3_GPIO_Port GPIOC
#define SPI1_CS_2_Pin GPIO_PIN_12
#define SPI1_CS_2_GPIO_Port GPIOC
#define SPI1_CS_1_Pin GPIO_PIN_2
#define SPI1_CS_1_GPIO_Port GPIOD
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
