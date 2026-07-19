/**
  ******************************************************************************
  * @file    buzzer.c
  * @brief   Driver buzzer active tren PB0 (xem buzzer.h).
  ******************************************************************************
  */

#include "buzzer.h"

void Buzzer_Init(void)
{
  Buzzer_Off();
}

void Buzzer_On(void)
{
  HAL_GPIO_WritePin(Buzzer_GPIO_Port, Buzzer_Pin, GPIO_PIN_SET);
}

void Buzzer_Off(void)
{
  HAL_GPIO_WritePin(Buzzer_GPIO_Port, Buzzer_Pin, GPIO_PIN_RESET);
}

void Buzzer_Beep(uint32_t duration_ms)
{
  Buzzer_On();
  HAL_Delay(duration_ms);
  Buzzer_Off();
}
