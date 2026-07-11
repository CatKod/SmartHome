/**
  ******************************************************************************
  * @file    pir_sensor.h
  * @brief   Driver cam bien chuyen dong PIR SR505.
  *
  * Phan cung (theo CubeMX):
  *   - PC0 (PIR_SR505_GPIO_EXTI0): ngo ra digital cua SR505,
  *     EXTI canh len, IRQ EXTI0 da bat trong gpio.c.
  *
 * Nguyen ly: ngat EXTI chot lai thoi diem phat hien chuyen dong.
 * PIR_GetMotion() tra ve 1 neu chan dang o muc HIGH hoac vua co canh len
 * trong khoang PIR_MOTION_HOLD_MS gan day (de khong bo lo xung ngan giua
 * hai lan doc dinh ky 1 giay).
 *
 * Luu y: HAL_GPIO_EXTI_Callback dung chung duoc dinh nghia trong gpio.c;
 * no goi PIR_EXTI_Callback() khi co canh len tren PC0.
 ******************************************************************************
 */

#ifndef __PIR_SENSOR_H
#define __PIR_SENSOR_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

/* Thoi gian giu trang thai "co chuyen dong" sau canh len cuoi cung (ms).
 * SR505 tu giu ngo ra HIGH ~8 s, gia tri nay chi de chong bo lo xung ngan. */
#define PIR_MOTION_HOLD_MS   3000U

/**
 * @brief Khoi tao driver (xoa trang thai chot). EXTI da duoc cau hinh
 *        san trong MX_GPIO_Init().
 */
void PIR_Init(void);

/**
 * @brief  Trang thai chuyen dong.
 * @retval 1 = co chuyen dong (pin HIGH hoac co canh len trong
 *         PIR_MOTION_HOLD_MS gan day), 0 = khong.
 */
uint8_t PIR_GetMotion(void);

/**
 * @brief Goi tu HAL_GPIO_EXTI_Callback (gpio.c) khi co canh len tren PC0.
 */
void PIR_EXTI_Callback(void);

#ifdef __cplusplus
}
#endif

#endif /* __PIR_SENSOR_H */
