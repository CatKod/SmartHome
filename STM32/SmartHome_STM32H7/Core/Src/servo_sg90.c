/**
  ******************************************************************************
  * @file    servo_sg90.c
  * @brief   Driver servo SG90 bang PWM TIM2 CH1 (PA15), tick 1 us.
  ******************************************************************************
  */

#include "servo_sg90.h"
#include "tim.h"

static uint16_t Servo_AngleToPulse(uint8_t angle)
{
  if (angle > 180U)
  {
    angle = 180U;
  }
  return (uint16_t)(SERVO_PULSE_MIN_US
         + ((uint32_t)angle * (SERVO_PULSE_MAX_US - SERVO_PULSE_MIN_US)) / 180U);
}

void Servo_Init(uint8_t initial_angle)
{
  Servo_SetAngle(initial_angle);
  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
}

void Servo_SetAngle(uint8_t angle)
{
  Servo_SetPulse(Servo_AngleToPulse(angle));
}

void Servo_SetPulse(uint16_t pulse_us)
{
  if (pulse_us < SERVO_PULSE_MIN_US)
  {
    pulse_us = SERVO_PULSE_MIN_US;
  }
  else if (pulse_us > SERVO_PULSE_MAX_US)
  {
    pulse_us = SERVO_PULSE_MAX_US;
  }
  __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, pulse_us);
}

uint8_t Servo_GetAngle(void)
{
  uint32_t pulse = __HAL_TIM_GET_COMPARE(&htim2, TIM_CHANNEL_1);

  if (pulse <= SERVO_PULSE_MIN_US)
  {
    return 0U;
  }
  if (pulse >= SERVO_PULSE_MAX_US)
  {
    return 180U;
  }
  return (uint8_t)(((pulse - SERVO_PULSE_MIN_US) * 180U)
                   / (SERVO_PULSE_MAX_US - SERVO_PULSE_MIN_US));
}

void Servo_Stop(void)
{
  HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_1);
}
