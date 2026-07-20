/**
 * @file auth_storage.c
 */
#include "auth_storage.h"
#include "system_config.h"
#include "main.h"
#include <string.h>

#define AUTH_MAGIC 0x534D4854UL /* 'SMHT' */
#define AUTH_VERSION 1U

typedef struct __attribute__((packed))
{
  uint32_t magic;
  uint32_t version;
  char     pin[5];
  uint8_t  uid_count;
  uint8_t  uids[AUTH_MAX_UIDS][AUTH_UID_LEN];
  uint8_t  fail_streak;
  uint8_t  reserved[2];
  uint32_t crc32;
} AuthFlash_t;

static AuthFlash_t s_cfg;
static uint32_t s_lockout_until = 0U;
static uint8_t s_lockout_level = 0U;

static uint32_t Auth_Crc32(const uint8_t *data, size_t len) /* Tính CRC32 để kiểm tra dữ liệu Flash có bị lỗi hay không */
{
  uint32_t crc = 0xFFFFFFFFUL;
  for (size_t i = 0; i < len; i++)
  {
    crc ^= data[i];
    for (uint8_t b = 0; b < 8U; b++)
    {
      if (crc & 1U)
      {
        crc = (crc >> 1) ^ 0xEDB88320UL;
      }
      else
      {
        crc >>= 1;
      }
    }
  }
  return ~crc;
}

static void Auth_Defaults(void)  /* Tạo cấu hình xác thực mặc định với PIN 1234 và chưa có UID */
{
  memset(&s_cfg, 0, sizeof(s_cfg));
  s_cfg.magic = AUTH_MAGIC;
  s_cfg.version = AUTH_VERSION;
  strncpy(s_cfg.pin, AUTH_DEFAULT_PIN, sizeof(s_cfg.pin) - 1U);
  s_cfg.uid_count = 0U;
}

static bool Auth_Validate(const AuthFlash_t *cfg)  /* Kiểm tra dữ liệu đọc từ Flash có hợp lệ hay không */
{
  if (cfg->magic != AUTH_MAGIC || cfg->version != AUTH_VERSION)
  {
    return false;
  }
  if (cfg->uid_count > AUTH_MAX_UIDS)
  {
    return false;
  }
  uint32_t crc = Auth_Crc32((const uint8_t *)cfg, sizeof(AuthFlash_t) - sizeof(uint32_t));
  return crc == cfg->crc32;
}

static void Auth_PrepareSave(AuthFlash_t *cfg) /* Cập nhật magic, version và CRC trước khi lưu Flash */
{
  cfg->magic = AUTH_MAGIC;
  cfg->version = AUTH_VERSION;
  cfg->crc32 = Auth_Crc32((const uint8_t *)cfg, sizeof(AuthFlash_t) - sizeof(uint32_t));
}

static bool Auth_FlashEraseSector(void) /* Xóa sector Flash chứa dữ liệu xác thực */
{
  FLASH_EraseInitTypeDef erase = {0};
  uint32_t sector_error = 0U;

  erase.TypeErase = FLASH_TYPEERASE_SECTORS;
  erase.Banks = FLASH_BANK_1;
  erase.Sector = FLASH_SECTOR_7;
  erase.NbSectors = 1U;
  erase.VoltageRange = FLASH_VOLTAGE_RANGE_3;

  if (HAL_FLASH_Unlock() != HAL_OK)
  {
    return false;
  }
  __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_ALL_ERRORS_BANK1);
  bool ok = (HAL_FLASHEx_Erase(&erase, &sector_error) == HAL_OK);
  HAL_FLASH_Lock();
  return ok;
}

static bool Auth_FlashProgram(const AuthFlash_t *cfg)  /* Ghi cấu hình PIN và UID xuống Flash */
{
  if (!Auth_FlashEraseSector())
  {
    return false;
  }
  if (HAL_FLASH_Unlock() != HAL_OK)
  {
    return false;
  }

  bool ok = true;
  const uint32_t *src = (const uint32_t *)cfg;
  uint32_t addr = AUTH_FLASH_ADDR;
  size_t words = (sizeof(AuthFlash_t) + 31U) / 32U;

  for (size_t i = 0; i < words; i++)
  {
    uint32_t chunk[8] = {0};
    memcpy(chunk, &src[i * 8], (i + 1U == words) ? (sizeof(AuthFlash_t) - i * 32U) : 32U);
    if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_FLASHWORD, addr, (uint32_t)chunk) != HAL_OK)
    {
      ok = false;
      break;
    }
    addr += 32U;
  }

  HAL_FLASH_Lock();
  return ok;
}

static bool Auth_Save(void) /* Chuẩn bị rồi lưu cấu hình hiện tại xuống Flash */
{
  AuthFlash_t temp = s_cfg;
  Auth_PrepareSave(&temp);
  if (!Auth_FlashProgram(&temp))
  {
    return false;
  }
  s_cfg = temp;
  return true;
}

void AuthStorage_Init(void)  /* Khởi tạo module xác thực bằng cách đọc dữ liệu từ Flash */
{
  const AuthFlash_t *flash_cfg = (const AuthFlash_t *)AUTH_FLASH_ADDR;
  if (Auth_Validate(flash_cfg))
  {
    memcpy(&s_cfg, flash_cfg, sizeof(s_cfg));
  }
  else
  {
    Auth_Defaults();
    (void)Auth_Save();
  }
}

bool AuthStorage_VerifyPin(const char *pin) /* Khởi tạo module xác thực bằng cách đọc dữ liệu từ Flash */
{
  if (pin == NULL)
  {
    return false;
  }
  return (strncmp(pin, s_cfg.pin, AUTH_PIN_LEN) == 0) && (strlen(pin) == AUTH_PIN_LEN);
}

bool AuthStorage_SetPin(const char *old_pin, const char *new_pin) /* Đổi PIN sau khi xác nhận đúng PIN cũ */
{
  if (!AuthStorage_VerifyPin(old_pin) || new_pin == NULL || strlen(new_pin) != AUTH_PIN_LEN)
  {
    return false;
  }
  strncpy(s_cfg.pin, new_pin, sizeof(s_cfg.pin) - 1U);
  s_cfg.pin[sizeof(s_cfg.pin) - 1U] = '\0';
  return Auth_Save();
}

bool AuthStorage_IsUidAuthorized(const uint8_t uid[AUTH_UID_LEN]) /* Kiểm tra UID RFID có nằm trong danh sách được cấp quyền hay không */
{
  if (uid == NULL)
  {
    return false;
  }
  for (uint8_t i = 0; i < s_cfg.uid_count; i++)
  {
    if (memcmp(uid, s_cfg.uids[i], AUTH_UID_LEN) == 0)
    {
      return true;
    }
  }
  return false;
}

bool AuthStorage_AddUid(const uint8_t uid[AUTH_UID_LEN])  /* Thêm một UID RFID mới vào danh sách và lưu xuống Flash */
{
  if (uid == NULL || s_cfg.uid_count >= AUTH_MAX_UIDS)
  {
    return false;
  }
  if (AuthStorage_IsUidAuthorized(uid))
  {
    return true;
  }
  memcpy(s_cfg.uids[s_cfg.uid_count], uid, AUTH_UID_LEN);
  s_cfg.uid_count++;
  return Auth_Save();
}

uint8_t AuthStorage_GetUidCount(void)  /* Lấy số lượng UID RFID hiện đang được lưu */
{
  return s_cfg.uid_count;
}

bool AuthStorage_IsLockedOut(uint32_t now_ms, uint32_t *remaining_ms) /* Kiểm tra hệ thống có đang bị khóa tạm thời do nhập sai nhiều lần không */
{
  if (now_ms >= s_lockout_until)
  {
    if (remaining_ms != NULL)
    {
      *remaining_ms = 0U;
    }
    return false;
  }
  if (remaining_ms != NULL)
  {
    *remaining_ms = s_lockout_until - now_ms;
  }
  return true;
}

void AuthStorage_RegisterFailure(uint32_t now_ms) /* Ghi nhận một lần xác thực thất bại và kích hoạt lockout nếu đủ số lần */
{
  s_cfg.fail_streak++;
  if (s_cfg.fail_streak >= AUTH_FAIL_LIMIT)
  {
    s_lockout_level++;
    uint32_t lock_ms = AUTH_LOCKOUT_BASE_MS << (s_lockout_level - 1U);
    if (lock_ms > AUTH_LOCKOUT_MAX_MS)
    {
      lock_ms = AUTH_LOCKOUT_MAX_MS;
    }
    s_lockout_until = now_ms + lock_ms;
    s_cfg.fail_streak = 0U;
    (void)Auth_Save();
  }
}

void AuthStorage_ClearFailures(void) /* Xóa số lần sai và trạng thái lockout khi xác thực thành công */
{
  s_cfg.fail_streak = 0U;
  s_lockout_until = 0U;
  s_lockout_level = 0U;
  (void)Auth_Save();
}
