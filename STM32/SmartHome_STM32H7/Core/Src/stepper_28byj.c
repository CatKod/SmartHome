/**
  ******************************************************************************
  * @file    stepper_28byj.c
  * @brief   Driver 28BYJ-48 + ULN2003, half-step tren PE0..PE3.
  ******************************************************************************
  */

#include "stepper_28byj.h"

/* Bang half-step 8 nhip, bit0..bit3 = IN1..IN4 */
static const uint8_t half_step_table[8] =
{
  0x01,  /* IN1             */
  0x03,  /* IN1 + IN2       */
  0x02,  /* IN2             */
  0x06,  /* IN2 + IN3       */
  0x04,  /* IN3             */
  0x0C,  /* IN3 + IN4       */
  0x08,  /* IN4             */
  0x09   /* IN4 + IN1       */
};

static uint8_t step_index = 0;   /* Vi tri hien tai trong bang half-step */

static uint32_t s_steps_remaining = 0U;
static StepperDir_t s_run_dir = STEPPER_CW;
static uint32_t s_last_step_tick = 0U;
static uint8_t s_running = 0U;

static void Stepper_WritePhase(uint8_t phase)
{
  HAL_GPIO_WritePin(ULN2003_28BYJ_GPIO_Output_1_GPIO_Port, ULN2003_28BYJ_GPIO_Output_1_Pin,
                    (phase & 0x01U) ? GPIO_PIN_SET : GPIO_PIN_RESET);
  HAL_GPIO_WritePin(ULN2003_28BYJ_GPIO_Output_2_GPIO_Port, ULN2003_28BYJ_GPIO_Output_2_Pin,
                    (phase & 0x02U) ? GPIO_PIN_SET : GPIO_PIN_RESET);
  HAL_GPIO_WritePin(ULN2003_28BYJ_GPIO_Output_3_GPIO_Port, ULN2003_28BYJ_GPIO_Output_3_Pin,
                    (phase & 0x04U) ? GPIO_PIN_SET : GPIO_PIN_RESET);
  HAL_GPIO_WritePin(ULN2003_28BYJ_GPIO_Output_4_GPIO_Port, ULN2003_28BYJ_GPIO_Output_4_Pin,
                    (phase & 0x08U) ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

void Stepper_Init(void)
{
  step_index = 0;
  Stepper_Release();
}

void Stepper_RotateSteps(uint32_t steps, StepperDir_t dir)
{
  for (uint32_t i = 0; i < steps; i++)
  {
    if (dir == STEPPER_CW)
    {
      step_index = (uint8_t)((step_index + 1U) & 0x07U);
    }
    else
    {
      step_index = (uint8_t)((step_index + 7U) & 0x07U);
    }

    Stepper_WritePhase(half_step_table[step_index]);
    HAL_Delay(STEPPER_STEP_DELAY_MS);
  }
}

void Stepper_RotateAngle(uint32_t angle_deg, StepperDir_t dir)
{
  Stepper_RotateSteps((angle_deg * STEPPER_STEPS_PER_REV) / 360U, dir);
}

void Stepper_Release(void)
{
  Stepper_WritePhase(0x00U);
}

void Stepper_Start(uint32_t steps, StepperDir_t dir)
{
  if (steps == 0U)
  {
    return;
  }
  s_steps_remaining = steps;
  s_run_dir = dir;
  s_running = 1U;
  s_last_step_tick = HAL_GetTick();
}

void Stepper_Cancel(void)
{
  s_running = 0U;
  s_steps_remaining = 0U;
  Stepper_Release();
}

uint8_t Stepper_IsBusy(void)
{
  return (s_running != 0U && s_steps_remaining > 0U) ? 1U : 0U;
}

void Stepper_Tick(void)
{
  if (!Stepper_IsBusy())
  {
    if (s_running != 0U)
    {
      s_running = 0U;
      Stepper_Release();
    }
    return;
  }

  uint32_t now = HAL_GetTick();
  if ((now - s_last_step_tick) < STEPPER_STEP_DELAY_MS)
  {
    return;
  }
  s_last_step_tick = now;

  if (s_run_dir == STEPPER_CW)
  {
    step_index = (uint8_t)((step_index + 1U) & 0x07U);
  }
  else
  {
    step_index = (uint8_t)((step_index + 7U) & 0x07U);
  }
  Stepper_WritePhase(half_step_table[step_index]);
  s_steps_remaining--;

  if (s_steps_remaining == 0U)
  {
    s_running = 0U;
    Stepper_Release();
  }
}
