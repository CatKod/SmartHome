/**
  ******************************************************************************
  * @file    keypad.c
  * @brief   Driver ban phim ma tran 4x4 tren PD0..PD7 (xem keypad.h).
  ******************************************************************************
  */

#include "keypad.h"

typedef struct
{
  GPIO_TypeDef *port;
  uint16_t      pin;
} KeypadPin_t;

static const KeypadPin_t rows[4] =
{
  { Keypad_Row_1_GPIO_Port, Keypad_Row_1_Pin },
  { Keypad_Row_2_GPIO_Port, Keypad_Row_2_Pin },
  { Keypad_Row_3_GPIO_Port, Keypad_Row_3_Pin },
  { Keypad_Row_4_GPIO_Port, Keypad_Row_4_Pin },
};

static const KeypadPin_t cols[4] =
{
  { Keypad_Col_1_GPIO_Port, Keypad_Col_1_Pin },
  { Keypad_Col_2_GPIO_Port, Keypad_Col_2_Pin },
  { Keypad_Col_3_GPIO_Port, Keypad_Col_3_Pin },
  { Keypad_Col_4_GPIO_Port, Keypad_Col_4_Pin },
};

static const char key_map[4][4] =
{
  { '1', '2', '3', 'A' },
  { '4', '5', '6', 'B' },
  { '7', '8', '9', 'C' },
  { '*', '0', '#', 'D' },
};

/* Cho tin hieu on dinh sau khi doi muc hang (~1 us o 550 MHz) */
static void Keypad_SettleDelay(void)
{
  for (volatile uint32_t i = 0; i < 200U; i++)
  {
    __NOP();
  }
}

void Keypad_Init(void)
{
  for (uint8_t r = 0; r < 4U; r++)
  {
    HAL_GPIO_WritePin(rows[r].port, rows[r].pin, GPIO_PIN_SET);
  }
}

char Keypad_Scan(void)
{
  char key = 0;

  for (uint8_t r = 0; r < 4U; r++)
  {
    /* Keo hang dang quet xuong LOW, cac hang khac HIGH */
    for (uint8_t i = 0; i < 4U; i++)
    {
      HAL_GPIO_WritePin(rows[i].port, rows[i].pin,
                        (i == r) ? GPIO_PIN_RESET : GPIO_PIN_SET);
    }
    Keypad_SettleDelay();

    for (uint8_t c = 0; c < 4U; c++)
    {
      if (HAL_GPIO_ReadPin(cols[c].port, cols[c].pin) == GPIO_PIN_RESET)
      {
        key = key_map[r][c];
        break;
      }
    }
    if (key != 0)
    {
      break;
    }
  }

  /* Tra cac hang ve HIGH (trang thai cho) */
  for (uint8_t r = 0; r < 4U; r++)
  {
    HAL_GPIO_WritePin(rows[r].port, rows[r].pin, GPIO_PIN_SET);
  }

  return key;
}

char Keypad_GetKey(void)
{
  static char     raw_prev     = 0;   /* Gia tri quet lan truoc            */
  static char     stable_key   = 0;   /* Phim da qua chong doi             */
  static uint32_t change_tick  = 0;   /* Thoi diem gia tri quet thay doi   */

  char raw = Keypad_Scan();

  if (raw != raw_prev)
  {
    raw_prev    = raw;
    change_tick = HAL_GetTick();
    return 0;
  }

  if ((HAL_GetTick() - change_tick) < KEYPAD_DEBOUNCE_MS)
  {
    return 0;
  }

  /* Gia tri da on dinh du lau: chi bao 1 lan khi chuyen tu nha -> nhan */
  if (raw != stable_key)
  {
    stable_key = raw;
    if (raw != 0)
    {
      return raw;
    }
  }

  return 0;
}
