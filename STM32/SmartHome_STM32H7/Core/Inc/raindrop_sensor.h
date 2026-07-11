/**
  ******************************************************************************
  * @file    raindrop_sensor.h
  * @brief   Driver cam bien mua (raindrop) qua ADC1.
  *
  * Phan cung (theo CubeMX):
  *   - PA1 = ADC1_INP17 (RAINDROP_SENSOR_ADC_IN17): ngo ra AO cua module.
  *
  * ADC1 16-bit dung chung voi cam bien anh sang (kenh 16), moi driver
  * tu cau hinh lai kenh cua minh truoc moi lan doc.
  * Module raindrop: tam kho -> dien ap AO cao (gan VCC), cang uot dien ap
  * cang thap => raw < nguong nghia la co mua.
  ******************************************************************************
  */

#ifndef __RAINDROP_SENSOR_H
#define __RAINDROP_SENSOR_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

/* Nguong phan biet mua/kho tren thang do 16-bit (0..65535).
 * raw < nguong => co mua. Hieu chinh lai theo thuc te lap dat. */
#define RAINDROP_THRESHOLD    40000U
/* Do tre (hysteresis) tranh trang thai nhay lien tuc quanh nguong */
#define RAINDROP_HYSTERESIS   2000U

/**
 * @brief Khoi tao driver. ADC1 da duoc hieu chuan trong LightSensor_Init();
 *        neu khong dung light_sensor, ham nay tu hieu chuan.
 */
void RaindropSensor_Init(void);

/**
 * @brief  Doc gia tri ADC tho cua cam bien mua.
 * @param  raw  Gia tri 0..65535 (cang uot cang thap).
 * @retval HAL_OK neu thanh cong.
 */
HAL_StatusTypeDef RaindropSensor_ReadRaw(uint16_t *raw);

/**
 * @brief  Trang thai mua 0/1 (co hysteresis).
 * @retval 1 = co mua, 0 = kho. Neu doc ADC loi, giu trang thai gan nhat.
 */
uint8_t RaindropSensor_GetStatus(void);

#ifdef __cplusplus
}
#endif

#endif /* __RAINDROP_SENSOR_H */
