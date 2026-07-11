/**
  ******************************************************************************
  * @file    keypad.h
  * @brief   Driver ban phim ma tran 4x4.
  *
  * Phan cung (theo CubeMX):
  *   - PD0..PD3 (Keypad_Row_1..4): hang, output push-pull.
  *   - PD4..PD7 (Keypad_Col_1..4): cot, input pull-up.
  *
  * Nguyen ly quet: keo lan luot tung hang xuong LOW, hang con lai HIGH;
  * phim duoc nhan se keo cot tuong ung xuong LOW (cot co pull-up).
  *
  * So do phim mac dinh:
  *      1 2 3 A
  *      4 5 6 B
  *      7 8 9 C
  *      * 0 # D
  ******************************************************************************
  */

#ifndef __KEYPAD_H
#define __KEYPAD_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

/* Thoi gian chong doi phim (ms) */
#define KEYPAD_DEBOUNCE_MS   20U

/**
 * @brief Dua cac hang ve muc HIGH (trang thai cho quet).
 */
void Keypad_Init(void);

/**
 * @brief  Quet tuc thoi, khong chong doi.
 * @retval Ky tu phim dang nhan ('0'..'9','A'..'D','*','#'),
 *         hoac 0 neu khong phim nao duoc nhan.
 */
char Keypad_Scan(void);

/**
 * @brief  Lay phim moi duoc nhan (co chong doi, phat hien canh nhan).
 *         Goi lap lai trong vong lap chinh; moi lan nhan chi tra ve 1 lan.
 * @retval Ky tu phim vua nhan, hoac 0 neu khong co phim moi.
 */
char Keypad_GetKey(void);

#ifdef __cplusplus
}
#endif

#endif /* __KEYPAD_H */
