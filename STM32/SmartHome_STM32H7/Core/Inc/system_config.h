/**
 * @file system_config.h
 * @brief Hang so cau hinh he thong SmartHome (STM32H7).
 */
#ifndef SYSTEM_CONFIG_H
#define SYSTEM_CONFIG_H

/* Chu ky */
#define SENSOR_PERIOD_MS          1000U
#define RFID_PERIOD_MS            250U

/* UART */
#define UART_LINK_TX_BUF_LEN      192U
#define UART_LINK_RX_BUF_LEN      256U

/* Khoa servo (SG90) */
#define SERVO_ANGLE_LOCK_ENGAGED  0U
#define SERVO_ANGLE_LOCK_RELEASED 90U
#define LOCK_AUTO_RELOCK_MS       5000U

/* Cua so (28BYJ-48 open-loop, homing mu khi boot) */
#define STEPPER_WINDOW_TRAVEL     4096U   /* 1 vong = dong hoan toan */
#define STEPPER_WINDOW_OPEN_STEPS 2048U   /* ~180 do mo cua so demo */

/* Nguong moi truong */
#define TEMP_VENT_OPEN_X10        300     /* 30.0 C */
#define TEMP_VENT_CLOSE_X10       280     /* 28.0 C hysteresis */
#define HUMI_VENT_OPEN            80U
#define HUMI_VENT_CLOSE           78U
#define DHT11_FAULT_THRESHOLD     10U

/* Den hanh lang (Relay 1) + den phong (Relay 2) */
#define LIGHT_HOLD_MS             3000U   /* Hall: PIR -> giu 3 giay */
#define ROOM_LIGHT_MANUAL_OVERRIDE_MS  (10U * 60U * 1000U)

/* Vỗ tay (Home Mode) */
#define CLAP_REFRACTORY_MS        200U
#define CLAP_WINDOW_MS            1500U

/* An ninh */
#define SECURITY_EXIT_DELAY_MS    20000U
#define SUSPICIOUS_ENTRY_DELAY_MS 15000U
#define AUTH_FAIL_LIMIT           3U
#define AUTH_LOCKOUT_BASE_MS      30000U
#define AUTH_LOCKOUT_MAX_MS       120000U
#define RISK_ALARM_THRESHOLD      3U
#define RISK_EVENT_COOLDOWN_MS    5000U

/* Buzzer bao dong */
#define ALARM_BEEP_ON_MS          500U
#define ALARM_BEEP_OFF_MS         500U
#define HC595_ALARM_BLINK_MS      500U

/* Anh sang 4 muc (raw ADC: thap = sang) */
#define LIGHT_LEVEL_BRIGHT_MAX    16383U
#define LIGHT_LEVEL_NORMAL_MAX    32767U
#define LIGHT_LEVEL_DIM_MAX       49151U
#define LIGHT_LEVEL_HYST          1500U

/* PIN mac dinh */
#define AUTH_DEFAULT_PIN          "1234"
#define AUTH_MAX_UIDS             4U
#define AUTH_PIN_LEN              4U

/* Flash luu cau hinh (sector 7 bank1, gan cuoi) */
#define AUTH_FLASH_ADDR           0x080FE000UL

#endif /* SYSTEM_CONFIG_H */
