/**
 * @file system_types.h
 * @brief Kieu du lieu va trang thai he thong SmartHome.
 */
#ifndef SYSTEM_TYPES_H
#define SYSTEM_TYPES_H

#include <stdint.h>
#include "relay.h"
#include "stepper_28byj.h"

typedef enum
{
  MODE_HOME = 0,
  MODE_SECURITY_EXIT_DELAY,
  MODE_SECURITY,
  MODE_SUSPICIOUS,
  MODE_ALARM
} SystemMode_t;

typedef enum
{
  LOCK_ENGAGED = 0,
  LOCK_RELEASED
} LockState_t;

typedef enum
{
  WINDOW_CLOSED = 0,
  WINDOW_OPEN,
  WINDOW_MOVING
} WindowState_t;

typedef enum
{
  LIGHT_LEVEL_DARK = 0,
  LIGHT_LEVEL_DIM,
  LIGHT_LEVEL_NORMAL,
  LIGHT_LEVEL_BRIGHT
} LightLevel_t;

typedef enum
{
  LIGHT_SRC_NONE = 0,
  LIGHT_SRC_AUTO_PIR,
  LIGHT_SRC_DOUBLE_CLAP,
  LIGHT_SRC_TRIPLE_CLAP,
  LIGHT_SRC_MQTT,
  LIGHT_SRC_SECURITY,
  LIGHT_SRC_ALARM
} LightSource_t;

/**
 * @brief Toan bo trang thai he thong.
 */
typedef struct
{
  /* Cam bien moi truong */
  int16_t  temp_x10;
  uint8_t  humi;
  uint16_t light_raw;
  uint8_t  light_level;      /* LightLevel_t */
  uint8_t  motion;
  uint8_t  sound;
  uint8_t  heat_alarm;
  uint8_t  rain;

  /* Dau vao nguoi dung */
  uint8_t  rfid_uid[4];
  uint8_t  rfid_valid;
  uint32_t rfid_tick;
  char     key;
  uint32_t key_tick;

  /* Dieu khien */
  uint8_t  relay[RELAY_COUNT];
  uint8_t  buzzer;
  uint8_t  servo_angle;
  int32_t  stepper_pos;
  uint8_t  hc595;

  /* Logic he thong */
  SystemMode_t mode;
  LockState_t  lock;
  WindowState_t window;
  uint8_t      hall_light_on;    /* RELAY_1: den hanh lang (PIR + anh sang) */
  uint8_t      room_light_on;    /* RELAY_2: den phong (vo tay / MQTT) */
  LightSource_t hall_light_source;
  LightSource_t room_light_source;
  uint8_t      alarm_active;
  uint8_t      risk_score;

  uint32_t lock_release_deadline;
  uint32_t hall_light_motion_deadline;
  uint32_t room_light_manual_until;
  uint32_t mode_deadline;
  uint32_t auth_lockout_until;
  uint8_t  auth_fail_count;
  uint8_t  dht11_fail_count;

  char last_access_method[8];
} SystemData_t;

#endif /* SYSTEM_TYPES_H */
