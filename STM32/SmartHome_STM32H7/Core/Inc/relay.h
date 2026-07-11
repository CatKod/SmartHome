/**
  ******************************************************************************
  * @file    relay.h
  * @brief   Driver 2 kenh relay.
  *
  * Phan cung (theo CubeMX):
  *   - PE4 (RELAY_1): dieu khien relay 1, output push-pull.
  *   - PE5 (RELAY_2): dieu khien relay 2, output push-pull.
  *
  * Mac dinh coi module relay kich muc HIGH (IN = HIGH => relay dong).
  * Neu dung module kich muc LOW (pho bien voi module co opto),
  * doi RELAY_ACTIVE_HIGH thanh 0.
  ******************************************************************************
  */

#ifndef __RELAY_H
#define __RELAY_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

/* 1 = relay dong khi chan IN o muc HIGH; 0 = kich muc LOW */
#define RELAY_ACTIVE_HIGH   1

typedef enum
{
  RELAY_1 = 0,
  RELAY_2,
  RELAY_COUNT
} Relay_t;

/**
 * @brief Dua ca 2 relay ve trang thai NGAT (an toan sau khi cap dien).
 */
void Relay_Init(void);

/**
 * @brief Dong (bat) relay.
 */
void Relay_On(Relay_t relay);

/**
 * @brief Ngat (tat) relay.
 */
void Relay_Off(Relay_t relay);

/**
 * @brief Dao trang thai relay.
 */
void Relay_Toggle(Relay_t relay);

/**
 * @brief  Trang thai hien tai cua relay.
 * @retval 1 = dang dong, 0 = dang ngat.
 */
uint8_t Relay_GetState(Relay_t relay);

#ifdef __cplusplus
}
#endif

#endif /* __RELAY_H */
