# UART Protocol — STM32H7 ↔ ESP32-S3

Khung thống nhất (2 chiều):

```text
<STX>TYPE,field1,field2,...*CS<ETX>
```

- `STX` = `0x02`, `ETX` = `0x03`
- `CS` = XOR tất cả byte của phần `TYPE,field1,...` (2 ký tự hex in hoa)
- Baud: **115200 8N1**
- STM32: USART3 (PB10 TX, PB11 RX)
- ESP32: UART1 (GPIO17 TX, GPIO18 RX)

## STM32 → ESP32

### DATA (định kỳ 1 giây)

```text
DATA,temp,humi,light_pct,light_state,motion,sound,heat,rain,lock,window,mode,hall_light,room_light,alarm,risk
```

| Field | Ví dụ | Mô tả |
|-------|-------|-------|
| temp | 28.5 | °C từ DHT11 |
| humi | 65 | % |
| light_pct | 45 | 0–100 (cao = sáng) |
| light_state | DARK/DIM/NORMAL/BRIGHT | 4 mức |
| motion | 0/1 | PIR |
| sound | 0/1 | Âm thanh |
| heat | 0/1 | Thermistor DO (overheat) |
| rain | 0/1 | Mưa |
| lock | ENGAGED/RELEASED | Khóa servo |
| window | CLOSED/OPEN/MOVING | Cửa sổ |
| mode | HOME/EXIT_DELAY/SECURITY/SUSPICIOUS/ALARM | |
| hall_light | ON/OFF | Relay 1 — đèn hành lang (PIR + ánh sáng) |
| room_light | ON/OFF | Relay 2 — đèn phòng (vỗ tay / MQTT) |
| alarm | 0/1 | |
| risk | 0–N | Risk score (phiên SUSPICIOUS) |

### EVT (sự kiện tức thời)

```text
EVT,ACCESS_GRANTED,RFID
EVT,ACCESS_DENIED,PIN,FAILED,2
EVT,WARNING,DEVICE_OVERHEAT
EVT,ALARM,ACTIVATED
...
```

### ACK (phản hồi lệnh)

```text
ACK,LIGHT_ON,OK
ACK,WINDOW_OPEN,REJECT,RAIN
```

## ESP32 → STM32

```text
CMD,LIGHT_ON
CMD,LIGHT_OFF
CMD,WINDOW_OPEN
CMD,WINDOW_CLOSE
CMD,SECURITY_ON
CMD,SECURITY_OFF
CMD,ALARM_OFF
```

ESP32 chờ ACK tối đa **500 ms**, retry **1 lần**. Nếu timeout → publish `smarthome/cmd/result` với `result=TIMEOUT`.

## MQTT Topics (ESP32)

| Topic | Retain | Nội dung |
|-------|--------|----------|
| `smarthome/status` | yes | `online` / `offline` (LWT) |
| `smarthome/state/env` | yes | JSON môi trường |
| `smarthome/state/mode` | yes | HOME, SECURITY, ... |
| `smarthome/state/lock` | yes | ENGAGED / RELEASED |
| `smarthome/state/window` | yes | CLOSED / OPEN / MOVING |
| `smarthome/state/hall_light` | yes | ON / OFF (hành lang) |
| `smarthome/state/room_light` | yes | ON / OFF (phòng) |
| `smarthome/state/alarm` | yes | ON / OFF |
| `smarthome/event` | no | Nhật ký sự kiện |
| `smarthome/cmd` | — | Lệnh từ dashboard → ESP32 |
| `smarthome/cmd/result` | no | Kết quả lệnh JSON |
