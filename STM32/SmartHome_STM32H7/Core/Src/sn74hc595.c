/**
  ******************************************************************************
  * @file    sn74hc595.c
  * @brief   Driver SN74HC595N bit-bang tren PE7/PE8/PE9.
  *
  * Khong can delay giua cac canh clock: thoi gian thuc thi
  * HAL_GPIO_WritePin (vai chuc ns o 275 MHz) da lon hon thoi gian
  * setup/hold toi thieu cua 74HC595 (~20 ns o 3.3V).
  ******************************************************************************
  */

#include "sn74hc595.h"

static inline void HC595_ClockPulse(void)
{
  HAL_GPIO_WritePin(SN74HC595N_SH_CP_GPIO_Port, SN74HC595N_SH_CP_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(SN74HC595N_SH_CP_GPIO_Port, SN74HC595N_SH_CP_Pin, GPIO_PIN_RESET);
}

static inline void HC595_Latch(void)
{
  HAL_GPIO_WritePin(SN74HC595N_ST_CP_GPIO_Port, SN74HC595N_ST_CP_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(SN74HC595N_ST_CP_GPIO_Port, SN74HC595N_ST_CP_Pin, GPIO_PIN_RESET);
}

static void HC595_ShiftByte(uint8_t data)
{
  for (uint8_t i = 0; i < 8U; i++)
  {
    HAL_GPIO_WritePin(SN74HC595N_DS_GPIO_Port, SN74HC595N_DS_Pin,
                      ((data & 0x80U) != 0U) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HC595_ClockPulse();
    data <<= 1;
  }
}

void HC595_Init(void)
{
  HAL_GPIO_WritePin(SN74HC595N_DS_GPIO_Port,    SN74HC595N_DS_Pin,    GPIO_PIN_RESET);
  HAL_GPIO_WritePin(SN74HC595N_SH_CP_GPIO_Port, SN74HC595N_SH_CP_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(SN74HC595N_ST_CP_GPIO_Port, SN74HC595N_ST_CP_Pin, GPIO_PIN_RESET);
  HC595_Write(0x00U);
}

void HC595_Write(uint8_t data)
{
  HC595_ShiftByte(data);
  HC595_Latch();
}

void HC595_WriteMulti(const uint8_t *data, uint16_t len)
{
  for (uint16_t i = 0; i < len; i++)
  {
    HC595_ShiftByte(data[i]);
  }
  HC595_Latch();
}
