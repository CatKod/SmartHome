/**
 * @file mqtt_manager.h
 * @brief Quan ly ket noi MQTT va publish du lieu cam bien.
 */
#ifndef MQTT_MANAGER_H
#define MQTT_MANAGER_H

#include <stdbool.h>
#include "sensor_types.h"

/**
 * @brief Khoi tao va bat dau MQTT client (tu dong reconnect boi esp-mqtt).
 *        Goi sau khi Wi-Fi da ket noi.
 */
void mqtt_manager_init(void);

/** @brief MQTT client dang ket noi toi broker hay khong. */
bool mqtt_manager_is_connected(void);

/**
 * @brief Dong goi du lieu cam bien thanh JSON va publish (QoS 1)
 *        len topic APP_MQTT_TOPIC_SENSOR.
 *
 * JSON: {"temperature": 27.5, "humidity": 65, "light_status": 1, "motion_status": 0}
 *
 * @return true neu publish duoc dua vao hang doi gui thanh cong.
 */
bool mqtt_manager_publish_sensor(const sensor_data_t *data);

#endif /* MQTT_MANAGER_H */
