/**
  ******************************************************************************
  * @file    thermistor.h
  * @brief   Driver module thermistor (ngo ra digital qua comparator).
  *
  * Phan cung (theo CubeMX):
  *   - PC5 (THERMISTOR_GPIO_EXTI5): ngo ra DO cua module KY-028 (hoac
  *     tuong duong), EXTI canh len, IRQ EXTI9_5 da bat trong gpio.c.
  *
  * Module chi bao 0/1 khi nhiet do vuot nguong chinh bang bien tro
  * (khong doc duoc gia tri nhiet do). Dung lam canh bao qua nhiet.
  * Ngat EXTI chot su kien de khong bo lo xung ngan; ngoai ra
  * Thermistor_GetAlarm() con doc truc tiep muc chan de bao trang thai
  * dang qua nguong keo dai.
  *
  * Luu y: HAL_GPIO_EXTI_Callback dung chung dinh nghia trong gpio.c.
  ******************************************************************************
  */

#ifndef __THERMISTOR_H
#define __THERMISTOR_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

/* Thoi gian giu trang thai "qua nhiet" sau canh len cuoi cung (ms) */
#define THERMISTOR_ALARM_HOLD_MS   3000U

/**
 * @brief Khoi tao driver (xoa trang thai chot). EXTI da duoc cau hinh
 *        san trong MX_GPIO_Init().
 */
void Thermistor_Init(void);

/**
 * @brief  Trang thai canh bao qua nhiet.
 * @retval 1 = pin dang HIGH hoac co canh len trong
 *         THERMISTOR_ALARM_HOLD_MS gan day, 0 = binh thuong.
 */
uint8_t Thermistor_GetAlarm(void);

/**
 * @brief Goi tu HAL_GPIO_EXTI_Callback (gpio.c) khi co canh len tren PC5.
 */
void Thermistor_EXTI_Callback(void);

#ifdef __cplusplus
}
#endif

#endif /* __THERMISTOR_H */
