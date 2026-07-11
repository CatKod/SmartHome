/**
  ******************************************************************************
  * @file    rfid_rc522.h
  * @brief   Driver dau doc RFID MFRC522 (13.56 MHz, the MIFARE) qua SPI1.
  *
  * Phan cung (theo CubeMX):
  *   - PA5/PA6/PA7 = SPI1 SCK/MISO/MOSI (~5.7 MHz, mode 0).
  *   - PA4 (RFID_GPIO_Output_CS) : chip select, active LOW.
  *   - PC4 (RFID_GPIO_Output_RST): reset, active LOW
  *     (gpio.c de mac dinh LOW => module dang bi giu reset,
  *      RC522_Init se keo len HIGH).
  *
  * Pham vi hien tai: phat hien the va doc UID 4 byte (du cho ung dung
  * quet the mo cua). Chua ho tro doc/ghi block va the UID 7/10 byte.
  ******************************************************************************
  */

#ifndef __RFID_RC522_H
#define __RFID_RC522_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

typedef enum
{
  RC522_OK = 0,       /* Thanh cong                       */
  RC522_ERR_NO_CARD,  /* Khong co the trong vung doc      */
  RC522_ERR_COMM,     /* Loi giao tiep / the tra loi sai  */
} RC522_Status_t;

/**
 * @brief Nha reset, cau hinh timer/dieu che va bat anten.
 *        Goi sau MX_SPI1_Init().
 */
void RC522_Init(void);

/**
 * @brief  Doc thanh ghi Version (chan doan ket noi SPI).
 * @retval 0x91/0x92 = MFRC522 that; 0x00/0xFF = loi day noi.
 */
uint8_t RC522_GetVersion(void);

/**
 * @brief  Phat hien the va doc UID (REQA + anticollision cap 1).
 * @param  uid  Bo dem >= 4 byte nhan UID.
 * @retval RC522_OK neu doc duoc UID; RC522_ERR_NO_CARD neu khong co the.
 */
RC522_Status_t RC522_ReadCardUID(uint8_t *uid);

/**
 * @brief Yeu cau the dang chon chuyen sang trang thai HALT
 *        (tranh doc lap lai lien tuc cung mot the).
 */
void RC522_Halt(void);

#ifdef __cplusplus
}
#endif

#endif /* __RFID_RC522_H */
