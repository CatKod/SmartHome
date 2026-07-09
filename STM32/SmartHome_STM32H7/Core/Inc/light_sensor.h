/**
  ******************************************************************************
  * @file    light_sensor.h
  * @brief   Driver cam bien anh sang (LDR/quang tro) qua ADC1.
  *
  * Phan cung (theo CubeMX):
  *   - PA0 = ADC1_INP16 (LIGHT_SENSOR_ADC_IN16): ngo ra analog cua module.
  *
  * ADC1: 16-bit, single conversion, software trigger (cau hinh trong adc.c).
  * Driver tu cau hinh lai sampling time dai hon (64.5 chu ky) vi nguon
  * tro khang cao (cau phan ap LDR) can thoi gian nap dien dung.
  ******************************************************************************
  */

#ifndef __LIGHT_SENSOR_H
#define __LIGHT_SENSOR_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

/* Nguong phan biet sang/toi tren thang do 16-bit (0..65535).
 * Voi module LDR thong dung (AO noi cau phan ap): cang SANG dien ap cang
 * THAP => raw < nguong nghia la co anh sang. Neu module cua ban nguoc lai,
 * doi lai phep so sanh trong LightSensor_GetStatus(). */
#define LIGHT_SENSOR_THRESHOLD    32768U
/* Do tre (hysteresis) tranh trang thai nhay lien tuc quanh nguong */
#define LIGHT_SENSOR_HYSTERESIS   2000U

/**
 * @brief Hieu chuan ADC1 (bat buoc tren H7 de so lieu chinh xac).
 *        Goi 1 lan sau MX_ADC1_Init().
 */
void LightSensor_Init(void);

/**
 * @brief  Doc gia tri ADC tho cua cam bien anh sang.
 * @param  raw  Gia tri 0..65535.
 * @retval HAL_OK neu thanh cong.
 */
HAL_StatusTypeDef LightSensor_ReadRaw(uint16_t *raw);

/**
 * @brief  Trang thai anh sang 0/1 (co hysteresis).
 * @retval 1 = sang, 0 = toi. Neu doc ADC loi, giu trang thai gan nhat.
 */
uint8_t LightSensor_GetStatus(void);

#ifdef __cplusplus
}
#endif

#endif /* __LIGHT_SENSOR_H */
