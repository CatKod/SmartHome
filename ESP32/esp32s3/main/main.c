/**
 * @file main.c
 * @brief ESP32-S3 SmartHome Gateway: UART <-> MQTT.
 */
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "app_config.h"
#include "system_state.h"
#include "uart_handler.h"
#include "wifi_manager.h"
#include "mqtt_manager.h"

static const char *TAG = "MAIN";
static system_state_t s_state;

static void parse_data_frame(const char *body)
{
    system_state_t st = s_state;
    char light_level[12] = "DARK";
    char lock[12] = "ENGAGED";
    char window[12] = "CLOSED";
    char mode[16] = "HOME";
    char hall_light_str[4] = "OFF";
    char room_light_str[4] = "OFF";

    int matched = sscanf(body,
        "%f,%d,%d,%11[^,],%d,%d,%d,%d,%11[^,],%11[^,],%15[^,],%3[^,],%3[^,],%d,%d",
        &st.temperature, &st.humidity, &st.light_pct,
        light_level, &st.motion, &st.sound, &st.heat_alarm, &st.rain,
        lock, window, mode, hall_light_str, room_light_str, &st.alarm, &st.risk_score);

    if (matched >= 15) {
        strncpy(st.light_level, light_level, sizeof(st.light_level) - 1);
        strncpy(st.lock, lock, sizeof(st.lock) - 1);
        strncpy(st.window, window, sizeof(st.window) - 1);
        strncpy(st.mode, mode, sizeof(st.mode) - 1);
        st.hall_light_on = (strcmp(hall_light_str, "ON") == 0) ? 1 : 0;
        st.room_light_on = (strcmp(room_light_str, "ON") == 0) ? 1 : 0;
        s_state = st;
        mqtt_manager_publish_state(&s_state);
    }
}

static void on_uart_frame(const char *type, const char *body)
{
    if (strcmp(type, "DATA") == 0) {
        parse_data_frame(body);
    } else if (strcmp(type, "EVT") == 0) {
        mqtt_manager_publish_event(body);
    }
}

static void on_mqtt_cmd(const char *cmd)
{
    ESP_LOGI(TAG, "Nhan lenh MQTT: %s", cmd);
    cmd_result_t result = {0};
    bool ok = uart_handler_send_cmd(cmd, &result);
    if (result.cmd[0] == '\0') {
        strncpy(result.cmd, cmd, sizeof(result.cmd) - 1);
        strncpy(result.result, ok ? "OK" : "FAILED", sizeof(result.result) - 1);
    }
    mqtt_manager_publish_cmd_result(&result);
}

void app_main(void)
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ESP_LOGI(TAG, "=== SmartHome Gateway ESP32-S3 ===");

    memset(&s_state, 0, sizeof(s_state));
    strncpy(s_state.mode, "HOME", sizeof(s_state.mode) - 1);
    strncpy(s_state.lock, "ENGAGED", sizeof(s_state.lock) - 1);
    strncpy(s_state.window, "CLOSED", sizeof(s_state.window) - 1);
    strncpy(s_state.light_level, "DARK", sizeof(s_state.light_level) - 1);

    wifi_manager_init();
    mqtt_manager_init(on_mqtt_cmd);
    uart_handler_init(on_uart_frame);

    ESP_LOGI(TAG, "Gateway san sang");
}
