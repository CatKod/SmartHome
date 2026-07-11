/**
  ******************************************************************************
  * @file    raindrop_sensor.c
  * @brief   Driver cam bien mua qua ADC1 kenh 17 (PA1).
  ******************************************************************************
  */

#include "raindrop_sensor.h"
#include "adc.h"

#define RAINDROP_ADC_TIMEOUT_MS   10U

static uint8_t rain_status = 0;   /* Trang thai gan nhat (giu khi doc loi) */

void RaindropSensor_Init(void)
{
  /* Hieu chuan ADC1 (an toan goi lai neu light_sensor da hieu chuan,
     vi luc nay ADC van dang tat). */
  if (HAL_ADCEx_Calibration_Start(&hadc1, ADC_CALIB_OFFSET_LINEARITY,
                                  ADC_SINGLE_ENDED) != HAL_OK)
  {
    Error_Handler();
  }
}

HAL_StatusTypeDef RaindropSensor_ReadRaw(uint16_t *raw)
{
  ADC_ChannelConfTypeDef sConfig = {0};
  HAL_StatusTypeDef status;

  /* ADC1 dung chung voi cam bien anh sang (kenh 16) nen phai chon lai
     kenh 17 truoc moi lan doc. Sampling time dai vi nguon tro khang cao. */
  sConfig.Channel                = ADC_CHANNEL_17;
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

  status = HAL_ADC_PollForConversion(&hadc1, RAINDROP_ADC_TIMEOUT_MS);
  if (status == HAL_OK)
  {
    *raw = (uint16_t)HAL_ADC_GetValue(&hadc1);
  }

  HAL_ADC_Stop(&hadc1);
  return status;
}

uint8_t RaindropSensor_GetStatus(void)
{
  uint16_t raw;

  if (RaindropSensor_ReadRaw(&raw) != HAL_OK)
  {
    return rain_status;   /* Doc loi: giu trang thai cu */
  }

  /* Cang uot dien ap cang thap. Hysteresis chong nhay quanh nguong. */
  if (rain_status == 0U)
  {
    if (raw < (RAINDROP_THRESHOLD - RAINDROP_HYSTERESIS))
    {
      rain_status = 1U;
    }
  }
  else
  {
    if (raw > (RAINDROP_THRESHOLD + RAINDROP_HYSTERESIS))
    {
      rain_status = 0U;
    }
  }

  return rain_status;
}
