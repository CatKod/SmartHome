/**
  ******************************************************************************
  * @file    dht11.c
  * @brief   Driver DHT11 bit-bang 1-wire (xem mo ta phan cung trong dht11.h).
  *
  * Giao thuc DHT11:
  *   1. Host keo DATA xuong LOW >= 18 ms, sau do nha bus.
  *   2. Cam bien tra loi: LOW 80 us + HIGH 80 us.
  *   3. 40 bit du lieu, moi bit: LOW 50 us + HIGH 26..28 us (bit 0)
  *      hoac HIGH ~70 us (bit 1).
  *   4. 5 byte: [humi_int][humi_dec][temp_int][temp_dec][checksum].
  *
  * Do xung bang DWT->CYCCNT (bo dem chu ky CPU) nen khong can timer rieng.
  * SysTick/UART IRQ van bat trong luc doc: thoi gian thuc thi ISR (<1 us
  * o 550 MHz) khong du lam sai nguong phan biet bit (nguong 45 us).
  ******************************************************************************
  */

#include "dht11.h"

/* Nguong phan biet bit: HIGH > 45 us => bit 1 (bit 0 ~28 us, bit 1 ~70 us) */
#define DHT11_BIT_THRESHOLD_US   45U

/* Timeout cho tung pha tin hieu (us), noi long hon spec de chiu sai so.
   RESP de 200 us (spec 20..40 us + LOW 80 us) vi mot so hang nhai
   phan hoi cham hon dang ke. */
#define DHT11_TIMEOUT_RESP_US    200U
#define DHT11_TIMEOUT_BIT_US     120U

static uint32_t cycles_per_us = 550U;   /* Cap nhat lai theo SystemCoreClock */

/* ---------------------------------------------------------------------------
 * DEBUG (tam thoi): kiem chung timing va trang thai bus khi chan doan loi.
 * Xem bang Live Expressions:
 *   - dht11_dbg_cycles_per_us : ky vong 550 (SYSCLK 550 MHz).
 *   - dht11_dbg_selftest_us   : DWT do lai HAL_Delay(50), ky vong
 *                               50000..51000. Lech xa => thang do sai.
 *   - dht11_dbg_idle_level    : muc bus ngay truoc tin hieu start.
 *                               Ky vong 1; neu 0 => bus bi ghim LOW
 *                               (sai chan, chap day, PB2 con noi vao bus).
 * ------------------------------------------------------------------------- */
volatile uint32_t dht11_dbg_cycles_per_us = 0;
volatile uint32_t dht11_dbg_selftest_us   = 0;
volatile uint8_t  dht11_dbg_idle_level    = 0xFF;

/* ---------------------------------------------------------------------------
 * Tien ich thoi gian dua tren DWT cycle counter
 * ------------------------------------------------------------------------- */

static void DWT_Enable(void)
{
  CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
  DWT->LAR = 0xC5ACCE55U;               /* Mo khoa DWT tren Cortex-M7 */
  DWT->CYCCNT = 0U;
  DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
}

static inline uint32_t DWT_GetCycles(void)
{
  return DWT->CYCCNT;
}

static inline void Delay_us(uint32_t us)
{
  uint32_t start = DWT_GetCycles();
  uint32_t ticks = us * cycles_per_us;
  while ((DWT_GetCycles() - start) < ticks) {}
}

/* ---------------------------------------------------------------------------
 * Truy cap bus DATA: mot chan duy nhat (DHT11_DATA_*, mac dinh PB1),
 * output open-drain + pull-up.
 * Ghi 0 = keo bus xuong, ghi 1 = nha bus (pull-up giu HIGH).
 * IDR van phan anh muc thuc te tren chan ke ca trong che do output
 * nen doc bus khong can chuyen mode.
 * ------------------------------------------------------------------------- */

static inline void DHT11_BusLow(void)
{
  HAL_GPIO_WritePin(DHT11_DATA_GPIO_Port, DHT11_DATA_Pin, GPIO_PIN_RESET);
}

static inline void DHT11_BusRelease(void)
{
  HAL_GPIO_WritePin(DHT11_DATA_GPIO_Port, DHT11_DATA_Pin, GPIO_PIN_SET);
}

static inline uint8_t DHT11_BusRead(void)
{
  return (HAL_GPIO_ReadPin(DHT11_DATA_GPIO_Port, DHT11_DATA_Pin) == GPIO_PIN_SET) ? 1U : 0U;
}

/**
 * @brief  Cho bus dat muc `level`, tra ve thoi gian da cho (us).
 * @retval Thoi gian cho [us], hoac 0xFFFFFFFF neu qua timeout.
 */
static uint32_t DHT11_WaitLevel(uint8_t level, uint32_t timeout_us)
{
  uint32_t start = DWT_GetCycles();
  uint32_t timeout_cycles = timeout_us * cycles_per_us;

  while (DHT11_BusRead() != level)
  {
    uint32_t elapsed = DWT_GetCycles() - start;
    if (elapsed > timeout_cycles)
    {
      return 0xFFFFFFFFU;
    }
  }
  return (DWT_GetCycles() - start) / cycles_per_us;
}

/* ---------------------------------------------------------------------------
 * API
 * ------------------------------------------------------------------------- */

void DHT11_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  cycles_per_us = SystemCoreClock / 1000000U;
  DWT_Enable();

  /* DEBUG: kiem chung thang do us cua DWT bang SysTick lam chuan.
     HAL_Delay(50) keo dai 50..51 ms => selftest ky vong 50000..51000 us. */
  dht11_dbg_cycles_per_us = cycles_per_us;
  uint32_t t0 = DWT_GetCycles();
  HAL_Delay(50);
  dht11_dbg_selftest_us = (DWT_GetCycles() - t0) / cycles_per_us;

  /* Chan DATA: chuyen sang output open-drain + pull-up de vua keo bus
     xuong duoc (start signal) vua doc duoc phan hoi cua cam bien qua IDR
     ma khong phai doi mode giua chung.
     Pull-up noi bo chi ho tro giu HIGH; module DHT11 thuong da co
     pull-up rieng. */
  GPIO_InitStruct.Pin   = DHT11_DATA_Pin;
  GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull  = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(DHT11_DATA_GPIO_Port, &GPIO_InitStruct);

  DHT11_BusRelease();
}

DHT11_Status_t DHT11_Read(int16_t *temp_x10, uint8_t *humi)
{
  uint8_t data[5] = {0};

  /* DEBUG: muc bus luc ranh, phai la 1 (pull-up giu HIGH).
     Neu la 0 => bus bi ghim LOW: sai chan / chap day / PB2 con noi bus. */
  dht11_dbg_idle_level = DHT11_BusRead();

  /* --- Tin hieu start: keo LOW >= 18 ms roi nha bus --- */
  DHT11_BusLow();
  HAL_Delay(20);
  DHT11_BusRelease();
  Delay_us(30);                          /* Host nha bus 20..40 us */

  /* --- Phan hoi cua cam bien: LOW 80 us + HIGH 80 us --- */
  if (DHT11_WaitLevel(0U, DHT11_TIMEOUT_RESP_US) == 0xFFFFFFFFU)
  {
    return DHT11_ERR_NO_RESPONSE;
  }
  if (DHT11_WaitLevel(1U, DHT11_TIMEOUT_BIT_US) == 0xFFFFFFFFU)
  {
    return DHT11_ERR_NO_RESPONSE;
  }
  if (DHT11_WaitLevel(0U, DHT11_TIMEOUT_BIT_US) == 0xFFFFFFFFU)
  {
    return DHT11_ERR_NO_RESPONSE;
  }

  /* --- 40 bit du lieu --- */
  for (uint8_t i = 0; i < 40U; i++)
  {
    /* Pha LOW 50 us truoc moi bit */
    if (DHT11_WaitLevel(1U, DHT11_TIMEOUT_BIT_US) == 0xFFFFFFFFU)
    {
      return DHT11_ERR_TIMEOUT;
    }
    /* Do do rong xung HIGH de phan biet bit 0/1 */
    uint32_t high_us = DHT11_WaitLevel(0U, DHT11_TIMEOUT_BIT_US);
    if (high_us == 0xFFFFFFFFU)
    {
      return DHT11_ERR_TIMEOUT;
    }

    data[i / 8U] <<= 1;
    if (high_us > DHT11_BIT_THRESHOLD_US)
    {
      data[i / 8U] |= 1U;
    }
  }

  /* --- Checksum --- */
  if ((uint8_t)(data[0] + data[1] + data[2] + data[3]) != data[4])
  {
    return DHT11_ERR_CHECKSUM;
  }

  /* data[3] bit7 = dau am (mot so ban DHT11/DHT12), 4 bit thap = phan thap phan */
  int16_t t = (int16_t)data[2] * 10 + (int16_t)(data[3] & 0x0FU);
  if ((data[3] & 0x80U) != 0U)
  {
    t = -t;
  }

  *temp_x10 = t;
  *humi     = data[0];
  return DHT11_OK;
}
