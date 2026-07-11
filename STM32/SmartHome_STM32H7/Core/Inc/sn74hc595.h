/**
  ******************************************************************************
  * @file    sn74hc595.h
  * @brief   Driver IC dich 8-bit SN74HC595N (bit-bang 3 day).
  *
  * Phan cung (theo CubeMX):
  *   - PE7 (SN74HC595N_DS)   : DS / SER    - du lieu noi tiep.
  *   - PE9 (SN74HC595N_SH_CP): SH_CP / SRCLK - clock dich bit.
  *   - PE8 (SN74HC595N_ST_CP): ST_CP / RCLK  - latch ra ngo ra.
  *
  * Ghi MSB truoc: bit 7 cua byte ung voi Q7 cua IC.
  * Ho tro noi chuoi (daisy-chain) nhieu IC qua HC595_WriteMulti:
  * byte dau tien trong mang se nam o IC XA nhat trong chuoi.
  ******************************************************************************
  */

#ifndef __SN74HC595_H
#define __SN74HC595_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

/**
 * @brief Dua cac chan dieu khien ve muc thap va xoa ngo ra ve 0x00.
 */
void HC595_Init(void);

/**
 * @brief  Ghi 1 byte ra IC (Q7..Q0), MSB truoc, latch ngay.
 */
void HC595_Write(uint8_t data);

/**
 * @brief  Ghi `len` byte cho chuoi IC noi tiep nhau, latch 1 lan cuoi.
 * @param  data  data[0] den IC xa nhat, data[len-1] den IC gan MCU nhat.
 */
void HC595_WriteMulti(const uint8_t *data, uint16_t len);

#ifdef __cplusplus
}
#endif

#endif /* __SN74HC595_H */
