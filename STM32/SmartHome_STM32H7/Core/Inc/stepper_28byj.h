/**
  ******************************************************************************
  * @file    stepper_28byj.h
  * @brief   Driver dong co buoc 28BYJ-48 qua module ULN2003.
  *
  * Phan cung (theo CubeMX):
  *   - PE0 (ULN2003_28BYJ_GPIO_Output_1): IN1
  *   - PE1 (ULN2003_28BYJ_GPIO_Output_2): IN2
  *   - PE2 (ULN2003_28BYJ_GPIO_Output_3): IN3
  *   - PE3 (ULN2003_28BYJ_GPIO_Output_4): IN4
  *
  * Dieu khien half-step (8 nhip): 28BYJ-48 giam toc 1/64
  * => 4096 half-step cho 1 vong truc ra.
  *
  * Luu y: cac ham Rotate* la BLOCKING (dung HAL_Delay giua cac buoc,
  * toi thieu 2 ms/buoc). Quay 1 vong mat ~8 giay.
  ******************************************************************************
  */

#ifndef __STEPPER_28BYJ_H
#define __STEPPER_28BYJ_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

/* So half-step cho 1 vong truc ra cua 28BYJ-48 (64 buoc x giam toc 1/64) */
#define STEPPER_STEPS_PER_REV   4096U

/* Thoi gian giua 2 half-step (ms). 2 ms ~ toc do toi da an toan. */
#define STEPPER_STEP_DELAY_MS   2U

typedef enum
{
  STEPPER_CW  = 0,   /* Thuan chieu kim dong ho  */
  STEPPER_CCW = 1    /* Nguoc chieu kim dong ho  */
} StepperDir_t;

/**
 * @brief Tat het cuon day (dong co tha long, khong nong).
 */
void Stepper_Init(void);

/**
 * @brief  Quay `steps` half-step theo huong `dir`. Ham BLOCKING.
 */
void Stepper_RotateSteps(uint32_t steps, StepperDir_t dir);

/**
 * @brief  Quay mot goc (do) theo huong `dir`. Ham BLOCKING.
 *         Vi du 90 do = 1024 half-step ~ 2 giay.
 */
void Stepper_RotateAngle(uint32_t angle_deg, StepperDir_t dir);

/**
 * @brief Ngat dien tat ca cuon day. Goi sau khi quay xong de dong co
 *        khong bi nong (28BYJ-48 khong can giu moment thi nen tha).
 */
void Stepper_Release(void);

#ifdef __cplusplus
}
#endif

#endif /* __STEPPER_28BYJ_H */
