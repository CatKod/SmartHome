/**
  ******************************************************************************
  * @file    dht11.h
  * @brief   Driver DHT11 (nhiet do / do am) bit-bang 1-wire.
  *
  * Phan cung (theo CubeMX):
  *   - PB2 (DHT11_GPIO_Output): chan keo DATA xuong (chuyen sang open-drain
  *     trong DHT11_Init de tranh xung dot bus).
  *   - PB1 (DHT11_GPIO_Input) : chan doc muc logic cua DATA.
  *   => Ca hai chan phai duoc noi chung vao chan DATA cua DHT11.
  *   Module DHT11 thuong co san dien tro keo len; neu dung cam bien tran
  *   can them pull-up ngoai 4.7k..10k len 3.3V.
  *
  * Luu y: DHT11_Read() la ham blocking (~25 ms) va chi duoc goi toi da
  * 1 lan / giay (gioi han cua cam bien).
  ******************************************************************************
  */

#ifndef __DHT11_H
#define __DHT11_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

typedef enum
{
  DHT11_OK = 0,          /* Doc thanh cong                       */
  DHT11_ERR_NO_RESPONSE, /* Cam bien khong phan hoi tin hieu start */
  DHT11_ERR_TIMEOUT,     /* Timeout khi doc chuoi 40 bit         */
  DHT11_ERR_CHECKSUM     /* Sai checksum                         */
} DHT11_Status_t;

/**
 * @brief Khoi tao driver: bat DWT cycle counter (delay/do xung us),
 *        chuyen PB2 sang open-drain va nha bus o muc cao.
 */
void DHT11_Init(void);

/**
 * @brief  Doc mot lan nhiet do / do am tu DHT11.
 * @param  temp_x10  Nhiet do x10 [0.1 do C] (vd 275 = 27.5 do C).
 * @param  humi      Do am tuong doi [%].
 * @retval DHT11_OK neu thanh cong, ma loi neu that bai
 *         (khi loi, gia tri dau ra khong duoc ghi).
 */
DHT11_Status_t DHT11_Read(int16_t *temp_x10, uint8_t *humi);

#ifdef __cplusplus
}
#endif

#endif /* __DHT11_H */
