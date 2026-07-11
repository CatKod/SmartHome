/**
  ******************************************************************************
  * @file    pir_sensor.c
  * @brief   Driver PIR SR505 tren PC0 (EXTI0, canh len).
  ******************************************************************************
  */

#include "pir_sensor.h"

static volatile uint32_t pir_last_edge_tick = 0;
static volatile uint8_t  pir_edge_seen      = 0;

void PIR_Init(void)
{
  pir_edge_seen      = 0;
  pir_last_edge_tick = 0;
}

uint8_t PIR_GetMotion(void)
{
  /* Pin dang HIGH: SR505 dang trong thoi gian giu cua chinh no */
  if (HAL_GPIO_ReadPin(PIR_SR505_GPIO_EXTI0_GPIO_Port,
                       PIR_SR505_GPIO_EXTI0_Pin) == GPIO_PIN_SET)
  {
    return 1U;
  }

  /* Pin da xuong LOW nhung vua co canh len gan day: van bao co chuyen dong
     de xung ngan giua hai lan doc khong bi bo lo. */
  if (pir_edge_seen != 0U)
  {
    if ((HAL_GetTick() - pir_last_edge_tick) < PIR_MOTION_HOLD_MS)
    {
      return 1U;
    }
    pir_edge_seen = 0;
  }

  return 0U;
}

void PIR_EXTI_Callback(void)
{
  pir_last_edge_tick = HAL_GetTick();
  pir_edge_seen      = 1;
}
