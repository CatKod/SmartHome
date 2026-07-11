/**
  ******************************************************************************
  * @file    sound_sensor.c
  * @brief   Driver cam bien am thanh tren PC3 (EXTI3, canh len).
  ******************************************************************************
  */

#include "sound_sensor.h"

static volatile uint32_t sound_last_edge_tick = 0;
static volatile uint8_t  sound_edge_seen      = 0;
static volatile uint32_t sound_event_count    = 0;

void SoundSensor_Init(void)
{
  sound_edge_seen      = 0;
  sound_last_edge_tick = 0;
  sound_event_count    = 0;
}

uint8_t SoundSensor_GetDetected(void)
{
  if (sound_edge_seen != 0U)
  {
    if ((HAL_GetTick() - sound_last_edge_tick) < SOUND_EVENT_HOLD_MS)
    {
      return 1U;
    }
    sound_edge_seen = 0;
  }
  return 0U;
}

uint32_t SoundSensor_GetEventCount(void)
{
  uint32_t count = sound_event_count;
  sound_event_count = 0;
  return count;
}

void SoundSensor_EXTI_Callback(void)
{
  sound_last_edge_tick = HAL_GetTick();
  sound_edge_seen      = 1;
  sound_event_count++;
}
