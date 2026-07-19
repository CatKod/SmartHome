/**
  ******************************************************************************
  * @file    relay.c
  * @brief   Driver 2 kenh relay tren PE4 / PE5 (xem relay.h).
  ******************************************************************************
  */

#include "relay.h"

typedef struct
{
  GPIO_TypeDef *port;
  uint16_t      pin;
} RelayPin_t;

static const RelayPin_t relay_pins[RELAY_COUNT] =
{
  { RELAY_1_GPIO_Port, RELAY_1_Pin },
  { RELAY_2_GPIO_Port, RELAY_2_Pin },
};

#if RELAY_ACTIVE_HIGH
  #define RELAY_LEVEL_ON    GPIO_PIN_SET
  #define RELAY_LEVEL_OFF   GPIO_PIN_RESET
#else
  #define RELAY_LEVEL_ON    GPIO_PIN_RESET
  #define RELAY_LEVEL_OFF   GPIO_PIN_SET
#endif

void Relay_Init(void)
{
  for (uint32_t i = 0; i < (uint32_t)RELAY_COUNT; i++)
  {
    HAL_GPIO_WritePin(relay_pins[i].port, relay_pins[i].pin, RELAY_LEVEL_OFF);
  }
}

void Relay_On(Relay_t relay)
{
  if (relay < RELAY_COUNT)
  {
    HAL_GPIO_WritePin(relay_pins[relay].port, relay_pins[relay].pin, RELAY_LEVEL_ON);
  }
}

void Relay_Off(Relay_t relay)
{
  if (relay < RELAY_COUNT)
  {
    HAL_GPIO_WritePin(relay_pins[relay].port, relay_pins[relay].pin, RELAY_LEVEL_OFF);
  }
}

void Relay_Toggle(Relay_t relay)
{
  if (relay < RELAY_COUNT)
  {
    HAL_GPIO_TogglePin(relay_pins[relay].port, relay_pins[relay].pin);
  }
}

uint8_t Relay_GetState(Relay_t relay)
{
  if (relay >= RELAY_COUNT)
  {
    return 0;
  }
  return (HAL_GPIO_ReadPin(relay_pins[relay].port, relay_pins[relay].pin)
          == RELAY_LEVEL_ON) ? 1U : 0U;
}
