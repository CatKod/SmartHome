/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    gpio.c
  * @brief   This file provides code for the configuration
  *          of all used GPIO pins.
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

/* Includes ------------------------------------------------------------------*/
#include "gpio.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/*----------------------------------------------------------------------------*/
/* Configure GPIO                                                             */
/*----------------------------------------------------------------------------*/
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/** Configure pins as
        * Analog
        * Input
        * Output
        * EVENT_OUT
        * EXTI
*/
void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOE, ULN2003_28BYJ_GPIO_Output_3_Pin|ULN2003_28BYJ_GPIO_Output_4_Pin|RELAY_1_Pin|RELAY_2_Pin
                          |SN74HC595N_DS_Pin|SN74HC595N_ST_CP_Pin|SN74HC595N_SH_CP_Pin|ULN2003_28BYJ_GPIO_Output_1_Pin
                          |ULN2003_28BYJ_GPIO_Output_2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(RFID_GPIO_Output_CS_GPIO_Port, RFID_GPIO_Output_CS_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, Buzzer_Pin|DHT11_GPIO_Output_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, Keypad_Row_1_Pin|Keypad_Row_2_Pin|Keypad_Row_3_Pin|Keypad_Row_4_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : ULN2003_28BYJ_GPIO_Output_3_Pin ULN2003_28BYJ_GPIO_Output_4_Pin RELAY_1_Pin RELAY_2_Pin
                           ULN2003_28BYJ_GPIO_Output_1_Pin ULN2003_28BYJ_GPIO_Output_2_Pin */
  GPIO_InitStruct.Pin = ULN2003_28BYJ_GPIO_Output_3_Pin|ULN2003_28BYJ_GPIO_Output_4_Pin|RELAY_1_Pin|RELAY_2_Pin
                          |ULN2003_28BYJ_GPIO_Output_1_Pin|ULN2003_28BYJ_GPIO_Output_2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pins : PIR_SR505_GPIO_EXTI0_Pin SOUND_SENSOR_GPIO_EXTI3_Pin RFID_GPIO_Output_RST_Pin THERMISTOR_GPIO_EXTI5_Pin */
  GPIO_InitStruct.Pin = PIR_SR505_GPIO_EXTI0_Pin|SOUND_SENSOR_GPIO_EXTI3_Pin|RFID_GPIO_Output_RST_Pin|THERMISTOR_GPIO_EXTI5_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : RFID_GPIO_Output_CS_Pin */
  GPIO_InitStruct.Pin = RFID_GPIO_Output_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(RFID_GPIO_Output_CS_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : Buzzer_Pin DHT11_GPIO_Output_Pin */
  GPIO_InitStruct.Pin = Buzzer_Pin|DHT11_GPIO_Output_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : DHT11_GPIO_Input_Pin */
  GPIO_InitStruct.Pin = DHT11_GPIO_Input_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(DHT11_GPIO_Input_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : SN74HC595N_DS_Pin SN74HC595N_ST_CP_Pin SN74HC595N_SH_CP_Pin */
  GPIO_InitStruct.Pin = SN74HC595N_DS_Pin|SN74HC595N_ST_CP_Pin|SN74HC595N_SH_CP_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pins : Keypad_Row_1_Pin Keypad_Row_2_Pin Keypad_Row_3_Pin Keypad_Row_4_Pin */
  GPIO_InitStruct.Pin = Keypad_Row_1_Pin|Keypad_Row_2_Pin|Keypad_Row_3_Pin|Keypad_Row_4_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pins : Keypad_Col_1_Pin Keypad_Col_2_Pin Keypad_Col_3_Pin Keypad_Col_4_Pin */
  GPIO_InitStruct.Pin = Keypad_Col_1_Pin|Keypad_Col_2_Pin|Keypad_Col_3_Pin|Keypad_Col_4_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI0_IRQn);

  HAL_NVIC_SetPriority(EXTI3_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI3_IRQn);

  HAL_NVIC_SetPriority(EXTI4_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI4_IRQn);

  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

}

/* USER CODE BEGIN 2 */

/* USER CODE END 2 */
