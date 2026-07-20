/**
 * @file app_config.h
 * @brief Cau hinh ESP32-S3 SmartHome Gateway.
 */
#ifndef APP_CONFIG_H
#define APP_CONFIG_H

/* UART */
#define APP_UART_PORT_NUM       1
#define APP_UART_TX_PIN         17
#define APP_UART_RX_PIN         18
#define APP_UART_BAUD_RATE      115200
#define APP_UART_RX_BUF_SIZE    1024
#define APP_UART_TX_BUF_SIZE    512
#define APP_FRAME_MAX_LEN       160
#define APP_CMD_ACK_TIMEOUT_MS  500
#define APP_CMD_RETRY           1

/* Wi-Fi */
#define APP_WIFI_SSID           "Hoang Hiep-2.4G-ext"
#define APP_WIFI_PASSWORD       "07042005"
#define APP_WIFI_MAX_RETRY      -1

/* MQTT */
#define APP_MQTT_BROKER_URI     "mqtt://mqtt.toolhub.app:1883"
#define APP_MQTT_USERNAME       "vinhhk"
#define APP_MQTT_PASSWORD       "20235876"

#define APP_MQTT_TOPIC_STATUS       "smarthome/status"
#define APP_MQTT_TOPIC_STATE_ENV    "smarthome/state/env"
#define APP_MQTT_TOPIC_STATE_MODE   "smarthome/state/mode"
#define APP_MQTT_TOPIC_STATE_LOCK   "smarthome/state/lock"
#define APP_MQTT_TOPIC_STATE_WINDOW "smarthome/state/window"
#define APP_MQTT_TOPIC_STATE_HALL_LIGHT "smarthome/state/hall_light"
#define APP_MQTT_TOPIC_STATE_ROOM_LIGHT "smarthome/state/room_light"
#define APP_MQTT_TOPIC_STATE_LIGHT      "smarthome/state/room_light"  /* alias: den phong */
#define APP_MQTT_TOPIC_STATE_ALARM  "smarthome/state/alarm"
#define APP_MQTT_TOPIC_EVENT        "smarthome/event"
#define APP_MQTT_TOPIC_CMD          "smarthome/cmd"
#define APP_MQTT_TOPIC_CMD_RESULT   "smarthome/cmd/result"

#endif /* APP_CONFIG_H */
