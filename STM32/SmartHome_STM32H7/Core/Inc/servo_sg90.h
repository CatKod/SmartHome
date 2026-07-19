/**
  ******************************************************************************
  * @file    servo_sg90.h
  * @brief   Driver servo SG90 bang PWM.
  *
  * Phan cung (theo CubeMX):
  *   - PA15 = TIM2_CH1 (Servo_SG90_TIM2_CH1): chan tin hieu servo.
  *
  * TIM2 (cau hinh trong tim.c): prescaler 274, period 19999.
  * Clock timer APB1 = 275 MHz => tick = 1 us, chu ky = 20 ms (50 Hz).
  * => Gia tri CCR chinh la do rong xung tinh bang micro-giay.
  *
  * SG90: xung 500..2500 us ung voi goc 0..180 do.
  ******************************************************************************
  */

#ifndef __SERVO_SG90_H
#define __SERVO_SG90_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

/* Do rong xung (us) ung voi goc 0 va 180 do. Neu servo cua ban khong quay
 * du 180 do, hieu chinh lai 2 gia tri nay (nhieu con SG90 la 1000..2000). */
#define SERVO_PULSE_MIN_US   500U
#define SERVO_PULSE_MAX_US   2500U

/**
 * @brief Bat PWM TIM2 CH1 va dua servo ve goc `initial_angle`.
 *        Goi sau MX_TIM2_Init().
 */
void Servo_Init(uint8_t initial_angle);

/**
 * @brief  Quay servo den goc mong muon.
 * @param  angle  Goc 0..180 do (gia tri lon hon bi kep ve 180).
 */
void Servo_SetAngle(uint8_t angle);

/**
 * @brief  Dat truc tiep do rong xung (us), kep trong
 *         [SERVO_PULSE_MIN_US, SERVO_PULSE_MAX_US].
 */
void Servo_SetPulse(uint16_t pulse_us);

/**
 * @brief  Goc hien tai (suy tu CCR), don vi do.
 */
uint8_t Servo_GetAngle(void);

/**
 * @brief Tat xung PWM (servo tha long, khong giu vi tri).
 */
void Servo_Stop(void);

#ifdef __cplusplus
}
#endif

#endif /* __SERVO_SG90_H */
