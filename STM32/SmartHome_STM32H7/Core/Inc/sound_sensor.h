/**
  ******************************************************************************
  * @file    sound_sensor.h
  * @brief   Driver cam bien am thanh (module micro + comparator, ngo ra digital).
  *
  * Phan cung (theo CubeMX):
  *   - PC3 (SOUND_SENSOR_GPIO_EXTI3): ngo ra DO cua module,
  *     EXTI canh len, IRQ EXTI3 da bat trong gpio.c.
  *
  * Nguyen ly: module chi phat xung ngan (vai ms) khi am thanh vuot nguong
  * (chinh bang bien tro tren module), nen bat buoc dung ngat EXTI de chot
  * su kien; SoundSensor_GetDetected() tra ve 1 neu co xung trong khoang
  * SOUND_EVENT_HOLD_MS gan day.
  *
  * Luu y: HAL_GPIO_EXTI_Callback dung chung dinh nghia trong gpio.c.
  ******************************************************************************
  */

#ifndef __SOUND_SENSOR_H
#define __SOUND_SENSOR_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

/* Thoi gian giu trang thai "co am thanh" sau xung cuoi cung (ms),
 * de vong doc dinh ky 1 giay khong bo lo su kien. */
#define SOUND_EVENT_HOLD_MS   2000U

/**
 * @brief Khoi tao driver (xoa trang thai chot). EXTI da duoc cau hinh
 *        san trong MX_GPIO_Init().
 */
void SoundSensor_Init(void);

/**
 * @brief  Trang thai phat hien am thanh.
 * @retval 1 = co am thanh trong SOUND_EVENT_HOLD_MS gan day, 0 = khong.
 */
uint8_t SoundSensor_GetDetected(void);

/**
 * @brief  So xung am thanh dem duoc tu lan goi truoc (dung de do "do on").
 *         Doc xong tu dong xoa ve 0.
 */
uint32_t SoundSensor_GetEventCount(void);

/**
 * @brief Goi tu HAL_GPIO_EXTI_Callback (gpio.c) khi co canh len tren PC3.
 */
void SoundSensor_EXTI_Callback(void);

#ifdef __cplusplus
}
#endif

#endif /* __SOUND_SENSOR_H */
