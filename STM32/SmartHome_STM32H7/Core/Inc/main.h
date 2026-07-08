/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
#define ULN2003_28BYJ_GPIO_Output_3_Pin GPIO_PIN_2
#define ULN2003_28BYJ_GPIO_Output_3_GPIO_Port GPIOE
#define ULN2003_28BYJ_GPIO_Output_4_Pin GPIO_PIN_3
#define ULN2003_28BYJ_GPIO_Output_4_GPIO_Port GPIOE
#define RELAY_1_Pin GPIO_PIN_4
#define RELAY_1_GPIO_Port GPIOE
#define RELAY_2_Pin GPIO_PIN_5
#define RELAY_2_GPIO_Port GPIOE
#define PIR_SR505_GPIO_EXTI0_Pin GPIO_PIN_0
#define PIR_SR505_GPIO_EXTI0_GPIO_Port GPIOC
#define PIR_SR505_GPIO_EXTI0_EXTI_IRQn EXTI0_IRQn
#define SOUND_SENSOR_GPIO_EXTI3_Pin GPIO_PIN_3
#define SOUND_SENSOR_GPIO_EXTI3_GPIO_Port GPIOC
#define SOUND_SENSOR_GPIO_EXTI3_EXTI_IRQn EXTI3_IRQn
#define LIGHT_SENSOR_ADC_IN16_Pin GPIO_PIN_0
#define LIGHT_SENSOR_ADC_IN16_GPIO_Port GPIOA
#define RAINDROP_SENSOR_ADC_IN17_Pin GPIO_PIN_1
#define RAINDROP_SENSOR_ADC_IN17_GPIO_Port GPIOA
#define RFID_GPIO_Output_CS_Pin GPIO_PIN_4
#define RFID_GPIO_Output_CS_GPIO_Port GPIOA
#define RFID_SPI1_SCK_Pin GPIO_PIN_5
#define RFID_SPI1_SCK_GPIO_Port GPIOA
#define RFID_SPI1_MISO_Pin GPIO_PIN_6
#define RFID_SPI1_MISO_GPIO_Port GPIOA
#define RFID_SPI1_MOSI_Pin GPIO_PIN_7
#define RFID_SPI1_MOSI_GPIO_Port GPIOA
#define RFID_GPIO_Output_RST_Pin GPIO_PIN_4
#define RFID_GPIO_Output_RST_GPIO_Port GPIOC
#define RFID_GPIO_Output_RST_EXTI_IRQn EXTI4_IRQn
#define THERMISTOR_GPIO_EXTI5_Pin GPIO_PIN_5
#define THERMISTOR_GPIO_EXTI5_GPIO_Port GPIOC
#define THERMISTOR_GPIO_EXTI5_EXTI_IRQn EXTI9_5_IRQn
#define Buzzer_Pin GPIO_PIN_0
#define Buzzer_GPIO_Port GPIOB
#define DHT11_GPIO_Input_Pin GPIO_PIN_1
#define DHT11_GPIO_Input_GPIO_Port GPIOB
#define DHT11_GPIO_Output_Pin GPIO_PIN_2
#define DHT11_GPIO_Output_GPIO_Port GPIOB
#define SN74HC595N_DS_Pin GPIO_PIN_7
#define SN74HC595N_DS_GPIO_Port GPIOE
#define SN74HC595N_ST_CP_Pin GPIO_PIN_8
#define SN74HC595N_ST_CP_GPIO_Port GPIOE
#define SN74HC595N_SH_CP_Pin GPIO_PIN_9
#define SN74HC595N_SH_CP_GPIO_Port GPIOE
#define ESP32_USART3_TX_Pin GPIO_PIN_10
#define ESP32_USART3_TX_GPIO_Port GPIOB
#define ESP32_USART3_RX_Pin GPIO_PIN_11
#define ESP32_USART3_RX_GPIO_Port GPIOB
#define Servo_SG90_TIM2_CH1_Pin GPIO_PIN_15
#define Servo_SG90_TIM2_CH1_GPIO_Port GPIOA
#define STM32F4_UART4_TX_Pin GPIO_PIN_10
#define STM32F4_UART4_TX_GPIO_Port GPIOC
#define STM32F4_UART4_RX_Pin GPIO_PIN_11
#define STM32F4_UART4_RX_GPIO_Port GPIOC
#define Keypad_Row_1_Pin GPIO_PIN_0
#define Keypad_Row_1_GPIO_Port GPIOD
#define Keypad_Row_2_Pin GPIO_PIN_1
#define Keypad_Row_2_GPIO_Port GPIOD
#define Keypad_Row_3_Pin GPIO_PIN_2
#define Keypad_Row_3_GPIO_Port GPIOD
#define Keypad_Row_4_Pin GPIO_PIN_3
#define Keypad_Row_4_GPIO_Port GPIOD
#define Keypad_Col_1_Pin GPIO_PIN_4
#define Keypad_Col_1_GPIO_Port GPIOD
#define Keypad_Col_2_Pin GPIO_PIN_5
#define Keypad_Col_2_GPIO_Port GPIOD
#define Keypad_Col_3_Pin GPIO_PIN_6
#define Keypad_Col_3_GPIO_Port GPIOD
#define Keypad_Col_4_Pin GPIO_PIN_7
#define Keypad_Col_4_GPIO_Port GPIOD
#define ULN2003_28BYJ_GPIO_Output_1_Pin GPIO_PIN_0
#define ULN2003_28BYJ_GPIO_Output_1_GPIO_Port GPIOE
#define ULN2003_28BYJ_GPIO_Output_2_Pin GPIO_PIN_1
#define ULN2003_28BYJ_GPIO_Output_2_GPIO_Port GPIOE

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
