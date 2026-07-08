/**
 * @file wifi_manager.h
 * @brief Quan ly ket noi Wi-Fi Station: ket noi, tu dong reconnect.
 */
#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <stdbool.h>

/**
 * @brief Khoi tao Wi-Fi station va bat dau ket noi.
 *        Ham nay CHAN (block) cho den khi lay duoc IP lan dau,
 *        de dam bao MQTT khoi dong sau khi da co mang.
 */
void wifi_manager_init(void);

/** @brief Wi-Fi hien tai da co IP hay chua. */
bool wifi_manager_is_connected(void);

#endif /* WIFI_MANAGER_H */
