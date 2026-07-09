/**
  ******************************************************************************
  * @file    light_sensor.c
  * @brief   Driver cam bien anh sang qua ADC1 kenh 16 (PA0).
  ******************************************************************************
  */

#include "light_sensor.h"
#include "adc.h"

#define LIGHT_ADC_TIMEOUT_MS   10U

static uint8_t light_status = 0;   /* Trang thai gan nhat (giu khi doc loi) */

void LightSensor_Init(void)
{
  ADC_ChannelConfTypeDef sConfig = {0};

  /* Hieu chuan offset + linearity: bat buoc tren STM32H7 sau moi lan cap dien */
  if (HAL_ADCEx_Calibration_Start(&hadc1, ADC_CALIB_OFFSET_LINEARITY,
                                  ADC_SINGLE_ENDED) != HAL_OK)
  {
    Error_Handler();
  }

  /* Cau hinh lai kenh 16 voi sampling time dai hon (CubeMX de 1.5 chu ky,
     qua ngan cho cau phan ap LDR tro khang cao). */
  sConfig.Channel                = ADC_CHANNEL_16;
  sConfig.Rank                   = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime           = ADC_SAMPLETIME_64CYCLES_5;
  sConfig.SingleDiff             = ADC_SINGLE_ENDED;
  sConfig.OffsetNumber           = ADC_OFFSET_NONE;
  sConfig.Offset                 = 0;
  sConfig.OffsetSignedSaturation = DISABLE;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
}

HAL_StatusTypeDef LightSensor_ReadRaw(uint16_t *raw)
{
  HAL_StatusTypeDef status;

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
