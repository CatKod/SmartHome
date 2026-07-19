/**
 * @file auth_storage.h
 * @brief Luu PIN va danh sach UID RFID trong Flash (EEPROM emulation).
 */
#ifndef AUTH_STORAGE_H
#define AUTH_STORAGE_H

#include <stdint.h>
#include <stdbool.h>

#define AUTH_UID_LEN 4U

void AuthStorage_Init(void);
bool AuthStorage_VerifyPin(const char *pin);
bool AuthStorage_SetPin(const char *old_pin, const char *new_pin);
bool AuthStorage_IsUidAuthorized(const uint8_t uid[AUTH_UID_LEN]);
bool AuthStorage_AddUid(const uint8_t uid[AUTH_UID_LEN]);
uint8_t AuthStorage_GetUidCount(void);
bool AuthStorage_IsLockedOut(uint32_t now_ms, uint32_t *remaining_ms);
void AuthStorage_RegisterFailure(uint32_t now_ms);
void AuthStorage_ClearFailures(void);

#endif /* AUTH_STORAGE_H */
