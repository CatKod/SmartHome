/**
  ******************************************************************************
  * @file    buzzer.h
  * @brief   Driver buzzer (coi bao) loai active (co mach dao dong san).
  *
  * Phan cung (theo CubeMX):
  *   - PB0 (Buzzer): output push-pull, HIGH = keu.
  *
  * Neu dung buzzer passive (can cap xung), chuyen PB0 sang PWM (TIM3_CH3)
  * trong CubeMX va viet lai driver nay.
  ******************************************************************************
  */

#ifndef __BUZZER_H
#define __BUZZER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

/**
 * @brief Tat buzzer (trang thai ban dau).
 */
void Buzzer_Init(void);

/**
 * @brief Bat buzzer (keu lien tuc cho den khi goi Buzzer_Off).
 */
void Buzzer_On(void);

/**
 * @brief Tat buzzer.
 */
void Buzzer_Off(void);

/**
 * @brief Keu 1 tieng bip trong `duration_ms` mili-giay.
 *        Ham BLOCKING (dung HAL_Delay) - chi dung cho bao hieu ngan.
 */
void Buzzer_Beep(uint32_t duration_ms);

#ifdef __cplusplus
}
#endif

#endif /* __BUZZER_H */
