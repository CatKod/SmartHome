/**
  ******************************************************************************
  * @file    light_sensor.c
  * @brief   Driver cam bien anh sang qua ADC1 kenh 16 (PA0).
  ******************************************************************************
  */

#include "light_sensor.h"
#include "system_config.h"
#include "system_types.h"
#include "adc.h"

#define LIGHT_ADC_TIMEOUT_MS   10U

static uint8_t light_status = 0;   /* Trang thai gan nhat (giu khi doc loi) */
static uint8_t light_level = LIGHT_LEVEL_DARK;

void LightSensor_Init(void)
{
  /* Hieu chuan offset + linearity: bat buoc tren STM32H7 sau moi lan cap dien.
     Chay khi ADC dang tat (truoc moi HAL_ADC_Start) nen an toan du goi
     nhieu lan tu cac driver dung chung ADC1. */
  if (HAL_ADCEx_Calibration_Start(&hadc1, ADC_CALIB_OFFSET_LINEARITY,
                                  ADC_SINGLE_ENDED) != HAL_OK)
  {
    Error_Handler();
  }
}

HAL_StatusTypeDef LightSensor_ReadRaw(uint16_t *raw)
{
  ADC_ChannelConfTypeDef sConfig = {0};
  HAL_StatusTypeDef status;

  /* ADC1 dung chung voi cam bien mua (kenh 17) nen phai chon lai kenh 16
     truoc moi lan doc. Sampling time 64.5 chu ky (CubeMX de 1.5 chu ky,
     qua ngan cho cau phan ap LDR tro khang cao). */
  sConfig.Channel                = ADC_CHANNEL_16;
  sConfig.Rank                   = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime           = ADC_SAMPLETIME_64CYCLES_5;
  sConfig.SingleDiff             = ADC_SINGLE_ENDED;
  sConfig.OffsetNumber           = ADC_OFFSET_NONE;
  sConfig.Offset                 = 0;
  sConfig.OffsetSignedSaturation = DISABLE;
  status = HAL_ADC_ConfigChannel(&hadc1, &sConfig);
  if (status != HAL_OK)
  {
    return status;
  }

  status = HAL_ADC_Start(&hadc1);
  if (status != HAL_OK)
  {
    return status;
  }

  status = HAL_ADC_PollForConversion(&hadc1, LIGHT_ADC_TIMEOUT_MS);
  if (status == HAL_OK)
  {
    *raw = (uint16_t)HAL_ADC_GetValue(&hadc1);
  }

  HAL_ADC_Stop(&hadc1);
  return status;
}

uint8_t LightSensor_GetStatus(void)
{
  uint16_t raw;

  if (LightSensor_ReadRaw(&raw) != HAL_OK)
  {
    return light_status;   /* Doc loi: giu trang thai cu */
  }

  /* Module LDR pho bien: cang sang dien ap AO cang thap.
     Hysteresis: chi doi trang thai khi vuot han nguong +/- khoang tre. */
  if (light_status == 0U)
  {
    if (raw < (LIGHT_SENSOR_THRESHOLD - LIGHT_SENSOR_HYSTERESIS))
    {
      light_status = 1U;
    }
  }
  else
  {
    if (raw > (LIGHT_SENSOR_THRESHOLD + LIGHT_SENSOR_HYSTERESIS))
    {
      light_status = 0U;
    }
  }

  return light_status;
}

uint8_t LightSensor_GetPercent(uint16_t *raw_out)
{
  uint16_t raw;

  if (LightSensor_ReadRaw(&raw) != HAL_OK)
  {
    if (raw_out != NULL)
    {
      *raw_out = 0U;
    }
    return 0U;
  }

  if (raw_out != NULL)
  {
    *raw_out = raw;
  }

  /* Raw thap = sang hon => phan tram dao nguoc */
  uint32_t pct = 100U - ((uint32_t)raw * 100U / 65535U);
  if (pct > 100U)
  {
    pct = 100U;
  }
  return (uint8_t)pct;
}

uint8_t LightSensor_GetLevel(void)
{
  uint16_t raw;

  if (LightSensor_ReadRaw(&raw) != HAL_OK)
  {
    return light_level;
  }

  uint8_t target = light_level;

  switch (light_level)
  {
  case LIGHT_LEVEL_BRIGHT:
    if (raw > (LIGHT_LEVEL_BRIGHT_MAX + LIGHT_LEVEL_HYST))
    {
      target = LIGHT_LEVEL_NORMAL;
    }
    break;
  case LIGHT_LEVEL_NORMAL:
    if (raw <= (LIGHT_LEVEL_BRIGHT_MAX - LIGHT_LEVEL_HYST))
    {
      target = LIGHT_LEVEL_BRIGHT;
    }
    else if (raw > (LIGHT_LEVEL_NORMAL_MAX + LIGHT_LEVEL_HYST))
    {
      target = LIGHT_LEVEL_DIM;
    }
    break;
  case LIGHT_LEVEL_DIM:
    if (raw <= (LIGHT_LEVEL_NORMAL_MAX - LIGHT_LEVEL_HYST))
    {
      target = LIGHT_LEVEL_NORMAL;
    }
    else if (raw > (LIGHT_LEVEL_DIM_MAX + LIGHT_LEVEL_HYST))
    {
      target = LIGHT_LEVEL_DARK;
    }
    break;
  default:
    if (raw <= (LIGHT_LEVEL_DIM_MAX - LIGHT_LEVEL_HYST))
    {
      target = LIGHT_LEVEL_DIM;
    }
    break;
  }

  light_level = target;
  light_status = (target >= LIGHT_LEVEL_NORMAL) ? 1U : 0U;
  return light_level;
}
