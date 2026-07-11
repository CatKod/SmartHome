/**
  ******************************************************************************
  * @file    thermistor.c
  * @brief   Driver module thermistor tren PC5 (EXTI9_5, canh len).
  ******************************************************************************
  */

#include "thermistor.h"

static volatile uint32_t therm_last_edge_tick = 0;
static volatile uint8_t  therm_edge_seen      = 0;

void Thermistor_Init(void)
{
  therm_edge_seen      = 0;
  therm_last_edge_tick = 0;
}

uint8_t Thermistor_GetAlarm(void)
{
  /* Pin dang HIGH: nhiet do van dang vuot nguong */
  if (HAL_GPIO_ReadPin(THERMISTOR_GPIO_EXTI5_GPIO_Port,
                       THERMISTOR_GPIO_EXTI5_Pin) == GPIO_PIN_SET)
  {
    return 0U;
  }

  /* Vua co canh len gan day: van bao de khong bo lo xung ngan */
  if (therm_edge_seen != 0U)
  {
    if ((HAL_GetTick() - therm_last_edge_tick) < THERMISTOR_ALARM_HOLD_MS)
    {
      return 0U;
    }
    therm_edge_seen = 0;
  }

  return 1U;
}

void Thermistor_EXTI_Callback(void)
{
  therm_last_edge_tick = HAL_GetTick();
  therm_edge_seen      = 1;
}
