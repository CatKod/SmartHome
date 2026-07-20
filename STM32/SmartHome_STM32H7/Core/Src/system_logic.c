/**
 * @file system_logic.c
 * @brief Logic trung tam SmartHome STM32H7.
 */
#include "system_logic.h"
#include "system_config.h"
#include "uart_protocol.h"
#include "uart_link.h"
#include "auth_storage.h"
#include "dht11.h"
#include "light_sensor.h"
#include "pir_sensor.h"
#include "sound_sensor.h"
#include "thermistor.h"
#include "raindrop_sensor.h"
#include "relay.h"
#include "buzzer.h"
#include "servo_sg90.h"
#include "stepper_28byj.h"
#include "sn74hc595.h"
#include "keypad.h"
#include "rfid_rc522.h"
#include <stdio.h>
#include <string.h>

SystemData_t g_system;

typedef enum
{
  KP_IDLE = 0,
  KP_DOOR_PIN,
  KP_SECURITY_PIN,
  KP_CHANGE_OLD,
  KP_CHANGE_NEW,
  KP_CHANGE_CONFIRM,
  KP_ENROLL_PIN
} KeypadFlow_t;

static KeypadFlow_t s_kp_flow = KP_IDLE;
static char s_pin_buf[6];
static uint8_t s_pin_len = 0U;
static char s_new_pin_buf[6];

static uint32_t s_last_sensor_tick = 0U;
static uint32_t s_last_rfid_tick = 0U;

static uint32_t s_clap_window_start = 0U;
static uint8_t s_clap_count = 0U;
static uint32_t s_last_clap_tick = 0U;

static uint32_t s_risk_pir_tick = 0U;
static uint32_t s_risk_sound_tick = 0U;

static uint32_t s_alarm_beep_tick = 0U;
static uint8_t s_alarm_beep_on = 0U;
static uint32_t s_hc595_blink_tick = 0U;
static uint8_t s_hc595_alarm_bit = 0U;

static uint8_t s_enroll_wait_rfid = 0U;
static char s_old_pin_buf[6];

static void Control_SetBuzzerOff(SystemData_t *sys);

static const char *ModeToStr(SystemMode_t mode)
{
  switch (mode)
  {
  case MODE_HOME: return "HOME";
  case MODE_SECURITY_EXIT_DELAY: return "EXIT_DELAY";
  case MODE_SECURITY: return "SECURITY";
  case MODE_SUSPICIOUS: return "SUSPICIOUS";
  case MODE_ALARM: return "ALARM";
  default: return "HOME";
  }
}

static const char *LockToStr(LockState_t lock)
{
  return (lock == LOCK_RELEASED) ? "RELEASED" : "ENGAGED";
}

static const char *WindowToStr(WindowState_t w)
{
  switch (w)
  {
  case WINDOW_OPEN: return "OPEN";
  case WINDOW_MOVING: return "MOVING";
  default: return "CLOSED";
  }
}

static const char *LightLevelToStr(uint8_t level)
{
  switch (level)
  {
  case LIGHT_LEVEL_BRIGHT: return "BRIGHT";
  case LIGHT_LEVEL_NORMAL: return "NORMAL";
  case LIGHT_LEVEL_DIM: return "DIM";
  default: return "DARK";
  }
}

static void System_SendEvt(const char *evt_body) /* gửi một sự kiện vừa xảy ra từ STM32H7 sang ESP32. */
{
  char payload[UART_FRAME_MAX];
  snprintf(payload, sizeof(payload), "EVT,%s", evt_body);
  (void)UartLink_SendPayload(payload);
}

static void System_SendAck(const char *cmd, const char *result, const char *reason)  /* báo cho ESP32 biết một lệnh đó ntn */
{
  char payload[UART_FRAME_MAX];
  if (reason != NULL && reason[0] != '\0')
  {
    snprintf(payload, sizeof(payload), "ACK,%s,%s,%s", cmd, result, reason);
  }
  else
  {
    snprintf(payload, sizeof(payload), "ACK,%s,%s", cmd, result);
  }
  (void)UartLink_SendPayload(payload);
}

static void System_SendData(void)   /* hàm gửi toàn bộ trạng thái hệ thống định kỳ */
{
  char payload[UART_FRAME_MAX];
  float temp = (float)g_system.temp_x10 / 10.0f;
  uint8_t light_pct = 100U - (uint8_t)((uint32_t)g_system.light_raw * 100U / 65535U);
  uint8_t hour = 14;   // Replace with your real RTC hour variable (0-23)
  uint8_t minute = 35; // Replace with your real RTC minute variable (0-59)

  snprintf(payload, sizeof(payload),
           "DATA,%.1f,%u,%u,%s,%u,%u,%u,%u,%s,%s,%s,%s,%s,%u,%u",
           temp,
           (unsigned)g_system.humi,
           (unsigned)light_pct,
           LightLevelToStr(g_system.light_level),
           (unsigned)g_system.motion,
           (unsigned)g_system.sound,
           (unsigned)g_system.heat_alarm,
           (unsigned)g_system.rain,
           LockToStr(g_system.lock),
           WindowToStr(g_system.window),
           ModeToStr(g_system.mode),
           g_system.hall_light_on ? "ON" : "OFF",
           g_system.room_light_on ? "ON" : "OFF",
           (unsigned)g_system.alarm_active,
           (unsigned)g_system.risk_score,
           (unsigned)hour,
           (unsigned)minute); 
  (void)UartLink_SendPayload(payload);
}

static void System_SetHallLight(SystemData_t *sys, uint8_t on, LightSource_t src)   /* hàm điều khiển đèn hành lang */
{
  if (on != 0U)
  {
    Relay_On(RELAY_1);
  }
  else
  {
    Relay_Off(RELAY_1);
  }
  sys->relay[RELAY_1] = on;
  if (sys->hall_light_on != on)
  {
    sys->hall_light_on = on;
    sys->hall_light_source = src;
    char body[64];
    snprintf(body, sizeof(body), "HALL_LIGHT,%s,SRC,%u", on ? "ON" : "OFF", (unsigned)src);
    System_SendEvt(body);
  }
  else
  {
    sys->hall_light_on = on;
    sys->hall_light_source = src;
  }
}

static void System_SetRoomLight(SystemData_t *sys, uint8_t on, LightSource_t src)
{
  if (sys->mode == MODE_ALARM && on != 0U)
  {
    return;
  }
  if (on != 0U)
  {
    Relay_On(RELAY_2);
  }
  else
  {
    Relay_Off(RELAY_2);
  }
  sys->relay[RELAY_2] = on;
  if (sys->room_light_on != on)
  {
    sys->room_light_on = on;
    sys->room_light_source = src;
    char body[64];
    snprintf(body, sizeof(body), "ROOM_LIGHT,%s,SRC,%u", on ? "ON" : "OFF", (unsigned)src);
    System_SendEvt(body);
  }
  else
  {
    sys->room_light_on = on;
    sys->room_light_source = src;
  }
}

static void System_ForceAllLightsOff(SystemData_t *sys, LightSource_t src) /* Hàm tắt toàn bộ đèn trong hệ thống */
{
  System_SetHallLight(sys, 0U, src);
  System_SetRoomLight(sys, 0U, src);
}

static void System_SetLock(SystemData_t *sys, LockState_t lock)  /* Hàm điều khiển trạng thái khóa cửa bằng servo */
{
  if (lock == LOCK_RELEASED)
  {
    Servo_SetAngle(SERVO_ANGLE_LOCK_RELEASED);
    sys->servo_angle = SERVO_ANGLE_LOCK_RELEASED;
    sys->lock = LOCK_RELEASED;
    sys->lock_release_deadline = HAL_GetTick() + LOCK_AUTO_RELOCK_MS;
  }
  else
  {
    Servo_SetAngle(SERVO_ANGLE_LOCK_ENGAGED);
    sys->servo_angle = SERVO_ANGLE_LOCK_ENGAGED;
    sys->lock = LOCK_ENGAGED;
    sys->lock_release_deadline = 0U;
  }
}

static void System_UpdateHC595(SystemData_t *sys)   /* Hàm cập nhật các LED trạng thái thông qua IC 74HC595 */
{
  uint8_t leds = 0U;
  leds |= (1U << 0); /* System online */
  if (sys->mode == MODE_HOME)
  {
    leds |= (1U << 1);
  }
  if (sys->mode == MODE_SECURITY || sys->mode == MODE_SUSPICIOUS)
  {
    leds |= (1U << 2);
  }
  if (sys->lock == LOCK_ENGAGED)
  {
    leds |= (1U << 3);
  }
  if (sys->motion != 0U)
  {
    leds |= (1U << 4);
  }
  if (sys->sound != 0U)
  {
    leds |= (1U << 5);
  }
  if (sys->rain != 0U)
  {
    leds |= (1U << 6);
  }
  if (sys->mode == MODE_ALARM)
  {
    uint32_t now = HAL_GetTick();
    if ((now - s_hc595_blink_tick) >= HC595_ALARM_BLINK_MS)
    {
      s_hc595_blink_tick = now;
      s_hc595_alarm_bit ^= 1U;
    }
    if (s_hc595_alarm_bit != 0U)
    {
      leds |= (1U << 7);
    }
  }
  if (sys->hc595 != leds)
  {
    HC595_Write(leds);
    sys->hc595 = leds;
  }
}

static void System_EnterHome(SystemData_t *sys)  /* Hàm đưa hệ thống về chế độ HOME */
{
  sys->mode = MODE_HOME;
  sys->alarm_active = 0U;
  sys->risk_score = 0U;
  sys->mode_deadline = 0U;
  Control_SetBuzzerOff(sys);
}

static void Control_SetBuzzerOff(SystemData_t *sys)   /* Hàm tắt còi buzzer và cập nhật trạng thái hệ thống */
{
  Buzzer_Off();
  sys->buzzer = 0U;
}

static void System_EnterAlarm(SystemData_t *sys)  /* Hàm đưa hệ thống vào chế độ báo động ALARM */
{
  sys->mode = MODE_ALARM;
  sys->alarm_active = 1U;
  System_SetLock(sys, LOCK_ENGAGED);
  System_ForceAllLightsOff(sys, LIGHT_SRC_ALARM);
  System_SendEvt("ALARM,ACTIVATED");
}

static void System_GrantAccess(SystemData_t *sys, const char *method)  /* Hàm xử lý khi người dùng xác thực thành công */
{
  strncpy(sys->last_access_method, method, sizeof(sys->last_access_method) - 1U);
  AuthStorage_ClearFailures();
  sys->auth_fail_count = 0U;

  if (sys->mode == MODE_ALARM || sys->mode == MODE_SECURITY || sys->mode == MODE_SUSPICIOUS)
  {
    System_EnterHome(sys);
    System_SetLock(sys, LOCK_RELEASED);
    char body[48];
    snprintf(body, sizeof(body), "ACCESS_GRANTED,%s,MODE,HOME", method);
    System_SendEvt(body);
    return;
  }

  System_SetLock(sys, LOCK_RELEASED);
  char body[48];
  snprintf(body, sizeof(body), "ACCESS_GRANTED,%s,LOCK,RELEASED", method);
  System_SendEvt(body);
}

static void System_DenyAccess(SystemData_t *sys, const char *method)  /* Hàm xử lý khi người dùng xác thực thất bại */
{
  uint32_t now = HAL_GetTick();
  uint32_t rem = 0U;
  if (AuthStorage_IsLockedOut(now, &rem))
  {
    System_SendEvt("ACCESS_DENIED,LOCKOUT");
    Buzzer_Beep(100U);
    return;
  }

  AuthStorage_RegisterFailure(now);
  sys->auth_fail_count++;
  char body[48];
  snprintf(body, sizeof(body), "ACCESS_DENIED,%s,FAILED,%u", method, (unsigned)sys->auth_fail_count);
  System_SendEvt(body);

  if (sys->auth_fail_count >= AUTH_FAIL_LIMIT)
  {
    System_SendEvt("WARNING,FAILED_ACCESS_LIMIT");
    if (sys->mode == MODE_SECURITY || sys->mode == MODE_SUSPICIOUS)
    {
      sys->risk_score++;
    }
  }
}

static void System_StartSecurityExit(SystemData_t *sys)  /* Hàm bắt đầu thời gian chờ trước khi kích hoạt chế độ SECURITY */
{
  sys->mode = MODE_SECURITY_EXIT_DELAY;
  sys->mode_deadline = HAL_GetTick() + SECURITY_EXIT_DELAY_MS;
  System_SendEvt("SECURITY,EXIT_DELAY");
}

static void System_ActivateSecurity(SystemData_t *sys) /* Hàm kích hoạt chế độ an ninh SECURITY */
{
  sys->mode = MODE_SECURITY;
  sys->risk_score = 0U;
  System_ForceAllLightsOff(sys, LIGHT_SRC_SECURITY);
  System_SetLock(sys, LOCK_ENGAGED);
  if (sys->window != WINDOW_CLOSED)
  {
    Stepper_Cancel();
    Stepper_Start(STEPPER_WINDOW_TRAVEL, STEPPER_CCW);
    sys->window = WINDOW_MOVING;
  }
  System_SendEvt("SECURITY,ACTIVE");
}

static WindowState_t s_window_target = WINDOW_CLOSED;

static void System_WindowOnMoveDone(SystemData_t *sys) /* Hàm cập nhật trạng thái cửa sổ sau khi động cơ bước chạy xong */
{
  if (s_window_target == WINDOW_OPEN)
  {
    sys->window = WINDOW_OPEN;
    sys->stepper_pos = (int32_t)STEPPER_WINDOW_OPEN_STEPS;
  }
  else
  {
    sys->window = WINDOW_CLOSED;
    sys->stepper_pos = 0;
  }
}

static void System_WindowSetTarget(SystemData_t *sys, WindowState_t target) /* Hàm đặt mục tiêu mở hoặc đóng cửa sổ */
{
  s_window_target = target;
  if (target == WINDOW_OPEN && sys->window != WINDOW_OPEN)
  {
    Stepper_Cancel();
    Stepper_Start(STEPPER_WINDOW_OPEN_STEPS, STEPPER_CW);
    sys->window = WINDOW_MOVING;
  }
  else if (target == WINDOW_CLOSED && sys->window != WINDOW_CLOSED)
  {
    Stepper_Cancel();
    Stepper_Start(STEPPER_WINDOW_TRAVEL, STEPPER_CCW);
    sys->window = WINDOW_MOVING;
  }
}

static void System_WindowArbitrate(SystemData_t *sys) /* Hàm phân xử và quyết định trạng thái đóng/mở cửa sổ */
{
  if (Stepper_IsBusy())
  {
    sys->window = WINDOW_MOVING;
    return;
  }

  if (sys->window == WINDOW_MOVING)
  {
    System_WindowOnMoveDone(sys);
  }

  if (sys->mode == MODE_ALARM || sys->mode == MODE_SECURITY || sys->mode == MODE_SUSPICIOUS)
  {
    if (sys->window != WINDOW_CLOSED)
    {
      System_WindowSetTarget(sys, WINDOW_CLOSED);
    }
    return;
  }

  if (sys->rain != 0U)
  {
    if (sys->window == WINDOW_OPEN || sys->window == WINDOW_MOVING)
    {
      System_WindowSetTarget(sys, WINDOW_CLOSED);
      System_SendEvt("WINDOW,CLOSED,RAIN");
    }
    return;
  }

  if (sys->mode != MODE_HOME)
  {
    return;
  }

  uint8_t vent_need = 0U;
  if (sys->temp_x10 > TEMP_VENT_OPEN_X10 || sys->humi > HUMI_VENT_OPEN)
  {
    vent_need = 1U;
  }
  if (sys->temp_x10 < TEMP_VENT_CLOSE_X10 && sys->humi < HUMI_VENT_CLOSE)
  {
    vent_need = 0U;
  }
  if (sys->heat_alarm != 0U)
  {
    vent_need = 1U;
  }

  if (vent_need != 0U && sys->window == WINDOW_CLOSED)
  {
    System_WindowSetTarget(sys, WINDOW_OPEN);
    System_SendEvt(sys->heat_alarm ? "WINDOW,OPEN,OVERHEAT" : "WINDOW,OPEN,HIGH_TEMP");
  }
}

static void System_ProcessClap(SystemData_t *sys) /* Hàm xử lý số lần vỗ tay để điều khiển đèn phòng */
{
  if (sys->mode != MODE_HOME)
  {
    return;
  }

  uint32_t events = SoundSensor_GetEventCount();
  uint32_t now = HAL_GetTick();
  for (uint32_t i = 0; i < events; i++)
  {
    if (s_clap_count == 0U)
    {
      s_clap_window_start = now;
    }
    if ((now - s_last_clap_tick) < CLAP_REFRACTORY_MS && s_last_clap_tick != 0U)
    {
      continue;
    }
    s_last_clap_tick = now;
    s_clap_count++;
  }

  if (s_clap_count > 0U && (now - s_clap_window_start) > CLAP_WINDOW_MS)
  {
    if (s_clap_count == 2U)
    {
      System_SetRoomLight(sys, 0U, LIGHT_SRC_DOUBLE_CLAP);
      sys->room_light_manual_until = now + ROOM_LIGHT_MANUAL_OVERRIDE_MS;
    }
    else if (s_clap_count == 3U)
    {
      System_SetRoomLight(sys, 1U, LIGHT_SRC_TRIPLE_CLAP);
      sys->room_light_manual_until = now + ROOM_LIGHT_MANUAL_OVERRIDE_MS;
    }
    s_clap_count = 0U;
    s_clap_window_start = 0U;
  }
}

static void System_ProcessAutoHallLight(SystemData_t *sys)  /* Hàm tự động điều khiển đèn hành lang dựa trên PIR và mức ánh sáng */
{
  uint32_t now = HAL_GetTick();

  if (sys->mode == MODE_ALARM || sys->mode == MODE_SECURITY ||
      sys->mode == MODE_SUSPICIOUS || sys->mode == MODE_SECURITY_EXIT_DELAY)
  {
    System_SetHallLight(sys, 0U, LIGHT_SRC_SECURITY);
    return;
  }

  if (sys->mode != MODE_HOME)
  {
    return;
  }

  uint8_t dark_enough = (sys->light_level == LIGHT_LEVEL_DARK || sys->light_level == LIGHT_LEVEL_DIM) ? 1U : 0U; /* ktra ánh sáng đủ tối ko */

  if (sys->motion != 0U && dark_enough != 0U)  /* Nếu vừa có chuyển động vừa đủ tối thì bật đèn.*/
  {
    sys->hall_light_motion_deadline = now + LIGHT_HOLD_MS;
    if (sys->hall_light_on == 0U)
    {
      System_SetHallLight(sys, 1U, LIGHT_SRC_AUTO_PIR);
      System_SendEvt("HALL_LIGHT,ON,MOTION_DARK");
    }
  }
  else if (sys->hall_light_on != 0U && sys->hall_light_source == LIGHT_SRC_AUTO_PIR)
  {
    if (sys->motion == 0U && now > sys->hall_light_motion_deadline)
    {
      System_SetHallLight(sys, 0U, LIGHT_SRC_AUTO_PIR);
      System_SendEvt("HALL_LIGHT,OFF,NO_MOTION");
    }
  }
}

static uint8_t s_heat_warn_sent = 0U;

static void System_ProcessThermistor(SystemData_t *sys) /* Hàm xử lý cảnh báo quá nhiệt từ cảm biến thermistor */
{
  if (sys->heat_alarm != 0U)
  {
    if (sys->mode != MODE_ALARM)
    {
      Buzzer_On();
      sys->buzzer = 1U;
    }
    if (s_heat_warn_sent == 0U)
    {
      System_SendEvt("WARNING,DEVICE_OVERHEAT");
      s_heat_warn_sent = 1U;
    }
  }
  else
  {
    s_heat_warn_sent = 0U;
    if (sys->mode != MODE_ALARM && sys->buzzer != 0U)
    {
      Control_SetBuzzerOff(sys);
    }
  }
}

static void System_ProcessAlarmBuzzer(SystemData_t *sys)  /* Hàm điều khiển buzzer kêu ngắt quãng trong chế độ ALARM */
{
  if (sys->mode != MODE_ALARM)
  {
    return;
  }
  uint32_t now = HAL_GetTick();
  uint32_t interval = s_alarm_beep_on ? ALARM_BEEP_ON_MS : ALARM_BEEP_OFF_MS;
  if ((now - s_alarm_beep_tick) >= interval)
  {
    s_alarm_beep_tick = now;
    s_alarm_beep_on ^= 1U;
    if (s_alarm_beep_on != 0U)
    {
      Buzzer_On();
    }
    else
    {
      Buzzer_Off();
    }
    sys->buzzer = s_alarm_beep_on;
  }
}

static void System_ProcessRisk(SystemData_t *sys)  /* Hàm tính điểm nguy cơ và kích hoạt báo động khi phát hiện xâm nhập */
{ 
  uint32_t now = HAL_GetTick();

  if (sys->mode == MODE_SECURITY && sys->motion != 0U)
  {
    sys->mode = MODE_SUSPICIOUS;
    sys->risk_score = 0U;
    sys->mode_deadline = now + SUSPICIOUS_ENTRY_DELAY_MS;  /*cho người dùng 15 giây để xác thực*/
    System_SendEvt("WARNING,MOTION_SECURITY_WAIT_AUTH");
    return;
  }

  if (sys->mode != MODE_SUSPICIOUS)
  {
    return;
  }

  if (sys->motion != 0U && (now - s_risk_pir_tick) > RISK_EVENT_COOLDOWN_MS)
  {
    s_risk_pir_tick = now;
    sys->risk_score++;
  }
  if (sys->sound != 0U && (now - s_risk_sound_tick) > RISK_EVENT_COOLDOWN_MS)  /*chuyển động thì tăng 1 điểm, chỉ tăng lại sau thời gian cooldown */
  {
    s_risk_sound_tick = now;     
    sys->risk_score++;
  }
  if (sys->mode_deadline != 0U && now >= sys->mode_deadline)   /*Có âm thanh thì tăng 1 điểm*/
  {
    sys->risk_score += 2U;   /*Hệ thống cộng thêm 2 điểm vì người trong nhà không nhập đúng PIN hoặc quét thẻ đúng trong thời gian chờ.*/
    sys->mode_deadline = 0U;
  }

  if (sys->risk_score >= RISK_ALARM_THRESHOLD)
  {
    System_EnterAlarm(sys);
  }
}

static void System_ProcessModes(SystemData_t *sys)  /* Hàm xử lý chuyển chế độ và tự động khóa lại cửa */
{
  uint32_t now = HAL_GetTick();

  if (sys->mode == MODE_SECURITY_EXIT_DELAY && sys->mode_deadline != 0U && now >= sys->mode_deadline) /* Hết thời gian chờ thì kích hoạt SECURITY */
  {
    System_ActivateSecurity(sys);
  }

  if (sys->lock == LOCK_RELEASED && sys->lock_release_deadline != 0U && now >= sys->lock_release_deadline) /* Hết 5 giây mở khóa thì tự khóa cửa lại */
  {
    System_SetLock(sys, LOCK_ENGAGED);
    System_SendEvt("LOCK,ENGAGED");
  }
}

static void System_ResetKeypadFlow(void) /* Hàm đưa luồng nhập keypad về trạng thái ban đầu */
{
  s_kp_flow = KP_IDLE;
  s_pin_len = 0U;
  memset(s_pin_buf, 0, sizeof(s_pin_buf));
}

static void System_SubmitPin(SystemData_t *sys) /* Hàm xử lý mã PIN sau khi người dùng nhập đủ và nhấn xác nhận */
{
  s_pin_buf[s_pin_len] = '\0';

  switch (s_kp_flow)
  {
  case KP_DOOR_PIN:
    if (AuthStorage_VerifyPin(s_pin_buf))
    {
      System_GrantAccess(sys, "PIN");
    }
    else
    {
      System_DenyAccess(sys, "PIN");
    }
    System_ResetKeypadFlow();
    break;

  case KP_SECURITY_PIN:
    if (AuthStorage_VerifyPin(s_pin_buf))
    {
      System_StartSecurityExit(sys);
    }
    else
    {
      System_DenyAccess(sys, "PIN");
    }
    System_ResetKeypadFlow();
    break;

  case KP_CHANGE_OLD:
    if (AuthStorage_VerifyPin(s_pin_buf))
    {
      strncpy(s_old_pin_buf, s_pin_buf, sizeof(s_old_pin_buf) - 1U);
      s_kp_flow = KP_CHANGE_NEW;
      s_pin_len = 0U;
    }
    else
    {
      System_DenyAccess(sys, "PIN");
      System_ResetKeypadFlow();
    }
    break;

  case KP_CHANGE_NEW:
    strncpy(s_new_pin_buf, s_pin_buf, sizeof(s_new_pin_buf) - 1U);
    s_kp_flow = KP_CHANGE_CONFIRM;
    s_pin_len = 0U;
    break;

  case KP_CHANGE_CONFIRM:
    if (strncmp(s_pin_buf, s_new_pin_buf, AUTH_PIN_LEN) == 0)
    {
      if (AuthStorage_SetPin(s_old_pin_buf, s_pin_buf))
      {
        System_SendEvt("PIN,CHANGED");
      }
    }
    System_ResetKeypadFlow();
    break;

  case KP_ENROLL_PIN:
    if (AuthStorage_VerifyPin(s_pin_buf))
    {
      s_enroll_wait_rfid = 1U;
      System_SendEvt("RFID,ENROLL_WAIT");
    }
    else
    {
      System_DenyAccess(sys, "PIN");
    }
    System_ResetKeypadFlow();
    break;

  default:
    System_ResetKeypadFlow();
    break;
  }
}

static void System_HandleKey(SystemData_t *sys, char key)  /* Hàm xử lý chức năng tương ứng với từng phím trên keypad */
{
  if (key == '*')
  {
    System_ResetKeypadFlow();
    return;
  }

  if (key == 'A' && s_kp_flow == KP_IDLE)
  {
    s_kp_flow = KP_SECURITY_PIN;
    s_pin_len = 0U;
    return;
  }
  if (key == 'D' && s_kp_flow == KP_IDLE)
  {
    s_kp_flow = KP_CHANGE_OLD;
    s_pin_len = 0U;
    return;
  }
  if (key == 'C' && s_kp_flow == KP_IDLE)
  {
    s_kp_flow = KP_ENROLL_PIN;
    s_pin_len = 0U;
    return;
  }

  if (key >= '0' && key <= '9')
  {
    if (s_kp_flow == KP_IDLE)
    {
      s_kp_flow = KP_DOOR_PIN;
      s_pin_len = 0U;
    }
    if (s_pin_len < AUTH_PIN_LEN)
    {
      s_pin_buf[s_pin_len++] = key;
    }
    return;
  }

  if (key == '#')
  {
    if (s_pin_len == AUTH_PIN_LEN)
    {
      System_SubmitPin(sys);
    }
  }
}

static void System_SensorRead(SystemData_t *sys)  /* Hàm đọc và cập nhật dữ liệu từ toàn bộ cảm biến */
{
  int16_t temp_x10;
  uint8_t humi;
  DHT11_Status_t st = DHT11_Read(&temp_x10, &humi);
  if (st == DHT11_OK)
  {
    sys->temp_x10 = temp_x10;
    sys->humi = humi;
    sys->dht11_fail_count = 0U;
  }
  else
  {
    sys->dht11_fail_count++;
    if (sys->dht11_fail_count >= DHT11_FAULT_THRESHOLD)
    {
      System_SendEvt("SENSOR_FAULT,DHT11");
      sys->dht11_fail_count = 0U;
    }
  }

  sys->light_level = LightSensor_GetLevel();
  (void)LightSensor_GetPercent(&sys->light_raw);
  sys->motion = PIR_GetMotion();
  sys->sound = SoundSensor_GetDetected();
  sys->heat_alarm = Thermistor_GetAlarm();
  sys->rain = RaindropSensor_GetStatus();
}

static void System_RFIDPoll(SystemData_t *sys)  /* Hàm quét và xử lý thẻ RFID */
{ 
  uint8_t uid[4];
  if (RC522_ReadCardUID(uid) != RC522_OK)
  {
    return;
  }

  memcpy(sys->rfid_uid, uid, sizeof(uid));  /* Lưu UID vừa đọc vào trạng thái hệ thống */
  sys->rfid_valid = 1U;
  sys->rfid_tick = HAL_GetTick();
  RC522_Halt();

  if (s_enroll_wait_rfid != 0U)  /* Nếu đang ở chế độ thêm thẻ thì lưu UID mới vào Flash */
  {
    if (AuthStorage_AddUid(uid))
    {
      System_SendEvt("RFID,ENROLLED");
    }
    s_enroll_wait_rfid = 0U;
    return;
  }

  if (AuthStorage_IsUidAuthorized(uid))  /* Thẻ hợp lệ thì cho phép truy cập, không hợp lệ thì từ chối */
  {
    System_GrantAccess(sys, "RFID");
  }
  else
  {
    System_DenyAccess(sys, "RFID");
  }
}

void System_OnUartCmd(const char *cmd)  /* Hàm xử lý lệnh điều khiển nhận từ ESP32 qua UART */
{
  if (cmd == NULL)
  {
    return;
  }

  if (strcmp(cmd, "LIGHT_ON") == 0)
  {
    System_SetRoomLight(&g_system, 1U, LIGHT_SRC_MQTT);
    g_system.room_light_manual_until = HAL_GetTick() + ROOM_LIGHT_MANUAL_OVERRIDE_MS;
    System_SendAck(cmd, "OK", NULL);
    return;
  }
  if (strcmp(cmd, "LIGHT_OFF") == 0)
  {
    System_SetRoomLight(&g_system, 0U, LIGHT_SRC_MQTT);
    g_system.room_light_manual_until = HAL_GetTick() + ROOM_LIGHT_MANUAL_OVERRIDE_MS;
    System_SendAck(cmd, "OK", NULL);
    return;
  }
  if (strcmp(cmd, "WINDOW_OPEN") == 0)
  {
    if (g_system.rain != 0U)
    {
      System_SendAck(cmd, "REJECT", "RAIN");
      return;
    }
    if (g_system.mode == MODE_ALARM || g_system.mode == MODE_SECURITY || g_system.mode == MODE_SUSPICIOUS)
    {
      System_SendAck(cmd, "REJECT", "SECURITY");
      return;
    }
    System_WindowSetTarget(&g_system, WINDOW_OPEN);
    System_SendAck(cmd, "OK", NULL);
    return;
  }
  if (strcmp(cmd, "WINDOW_CLOSE") == 0)
  {
    System_WindowSetTarget(&g_system, WINDOW_CLOSED);
    System_SendAck(cmd, "OK", NULL);
    return;
  }
  if (strcmp(cmd, "SECURITY_ON") == 0)
  {
    System_ActivateSecurity(&g_system);
    System_SendAck(cmd, "OK", NULL);
    return;
  }
  if (strcmp(cmd, "SECURITY_OFF") == 0)
  {
    System_EnterHome(&g_system);
    System_SendAck(cmd, "OK", NULL);
    return;
  }
  if (strcmp(cmd, "ALARM_OFF") == 0)
  {
    System_EnterHome(&g_system);
    Control_SetBuzzerOff(&g_system);
    System_SendAck(cmd, "OK", NULL);
    System_SendEvt("ALARM,CLEARED,MQTT");
    return;
  }

  System_SendAck(cmd, "REJECT", "UNKNOWN");
}

void System_Init(void)  /* Hàm khởi tạo toàn bộ hệ thống nhà thông minh */
{
  memset(&g_system, 0, sizeof(g_system));
  g_system.mode = MODE_HOME;
  g_system.lock = LOCK_ENGAGED;
  g_system.window = WINDOW_CLOSED;

  DHT11_Init();
  LightSensor_Init();
  PIR_Init();
  SoundSensor_Init();
  Thermistor_Init();
  RaindropSensor_Init();
  Keypad_Init();
  RC522_Init();
  Relay_Init();
  Buzzer_Init();
  Stepper_Init();
  HC595_Init();
  Servo_Init(0);
  AuthStorage_Init();

  Relay_Off(RELAY_1);
  Relay_Off(RELAY_2);
  Buzzer_Off();
  Servo_SetAngle(SERVO_ANGLE_LOCK_ENGAGED);
  HC595_Write(0x00U);

  /* Homing mu: dong cua so ve vi tri 0 */
  Stepper_RotateSteps(STEPPER_WINDOW_TRAVEL, STEPPER_CCW);
  g_system.stepper_pos = 0;
  g_system.window = WINDOW_CLOSED;

  HAL_Delay(1000);

  UartLink_Init(System_OnUartCmd);
  UartLink_StartRx();

  System_SendData();
  System_SendEvt("SYSTEM,BOOT,HOME");
}

void System_Loop(void)  /* Hàm vòng lặp chính, liên tục xử lý toàn bộ hoạt động của hệ thống */
{
  char key = Keypad_GetKey();
  if (key != 0)
  {
    g_system.key = key;
    g_system.key_tick = HAL_GetTick();
    System_HandleKey(&g_system, key);
  }

  uint32_t now = HAL_GetTick();
  if ((now - s_last_rfid_tick) >= RFID_PERIOD_MS)
  {
    s_last_rfid_tick += RFID_PERIOD_MS;
    System_RFIDPoll(&g_system);
  }

  Stepper_Tick();
  System_WindowArbitrate(&g_system);
  System_ProcessModes(&g_system);
  System_ProcessRisk(&g_system);
  System_ProcessAlarmBuzzer(&g_system);
  System_ProcessThermistor(&g_system);

  if ((now - s_last_sensor_tick) >= SENSOR_PERIOD_MS)
  {
    s_last_sensor_tick += SENSOR_PERIOD_MS;
    System_SensorRead(&g_system);
    System_ProcessClap(&g_system);
    System_ProcessAutoHallLight(&g_system);
    System_UpdateHC595(&g_system);
    System_SendData();
  }

  UartLink_Poll();
}
