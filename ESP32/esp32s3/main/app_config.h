/**
 * @file app_config.h
 * @brief Cau hinh chung cho ESP32-S3 Gateway (Giai doan 1).
 *
 * Tap trung toan bo thong so o day de de thay doi khi doi phan cung
 * hoac doi moi truong mang.
 */
#ifndef APP_CONFIG_H
#define APP_CONFIG_H

/* ==================== UART (noi voi STM32H7) ==================== */
/* STM32H7 USART3: PB10 = TX -> noi vao UART_RX_PIN cua ESP32
 *                 PB11 = RX -> noi vao UART_TX_PIN cua ESP32
 * Nho noi chung GND giua 2 board!
 */
#define APP_UART_PORT_NUM       1           /* UART1 cua ESP32-S3            */
#define APP_UART_TX_PIN         17          /* ESP32 TX  -> STM32 PB11 (RX)  */
#define APP_UART_RX_PIN         18          /* ESP32 RX  <- STM32 PB10 (TX)  */
#define APP_UART_BAUD_RATE      115200
#define APP_UART_RX_BUF_SIZE    1024        /* Buffer driver UART            */

/* Khung truyen: <STX>Temp,Humi,Light,Motion<ETX> */
#define APP_FRAME_STX           0x02
#define APP_FRAME_ETX           0x03
#define APP_FRAME_MAX_LEN       64          /* Do dai toi da phan payload    */

/* ==================== Wi-Fi ==================== */
/* TODO: Dien thong tin Wi-Fi cua ban vao day */
#define APP_WIFI_SSID           "I2_5G"
#define APP_WIFI_PASSWORD       "abcd1232"
#define APP_WIFI_MAX_RETRY      -1          /* -1 = retry vo han             */

/* ==================== MQTT ==================== */
/* Cac lua chon URI:
 *   mqtt://mqtt.toolhub.app:1883    (TCP thuong)
 *   mqtts://mqtt.toolhub.app:8883   (TLS - can them chung chi CA)
 *   ws://mqtt.toolhub.app:8083      (WebSocket)
 *   wss://mqtt.toolhub.app:8084     (WebSocket Secure)
 * Giai doan 1 dung TCP thuong cho don gian.
 */
#define APP_MQTT_BROKER_URI     "mqtt://mqtt.toolhub.app:1883"
#define APP_MQTT_USERNAME       "vinhhk"
#define APP_MQTT_PASSWORD       "20235876"
#define APP_MQTT_TOPIC_SENSOR   "smarthome/sensor_data"

#endif /* APP_CONFIG_H */
