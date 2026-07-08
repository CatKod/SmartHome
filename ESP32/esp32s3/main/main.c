/**
 * @file main.c
 * @brief ESP32-S3 Gateway - Giai doan 1.
 *
 * Luong du lieu:
 *   STM32H7 --UART--> [uart_handler] --queue--> [mqtt_publisher_task]
 *                                                    |
 *                                              [mqtt_manager] --> broker
 *
 * Cac module:
 *   - wifi_manager : ket noi Wi-Fi station, tu reconnect.
 *   - uart_handler : nhan frame <STX>...<ETX>, parse, day vao queue.
 *   - mqtt_manager : ket noi broker, publish JSON.
 */
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "app_config.h"
#include "sensor_types.h"
#include "uart_handler.h"
#include "wifi_manager.h"
#include "mqtt_manager.h"

static const char *TAG = "MAIN";

/* Queue trung gian giua uart_handler va task publish */
#define SENSOR_QUEUE_LEN 8
static QueueHandle_t s_sensor_queue = NULL;

/**
 * @brief Task cho du lieu cam bien tu queue va publish len MQTT.
 */
static void mqtt_publisher_task(void *arg)
{
    sensor_data_t data;

    for (;;) {
        if (xQueueReceive(s_sensor_queue, &data, portMAX_DELAY) == pdTRUE) {
            mqtt_manager_publish_sensor(&data);
        }
    }
}

void app_main(void)
{
    /* NVS can cho Wi-Fi luu calibration data */
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ESP_LOGI(TAG, "=== SmartHome Gateway ESP32-S3 - Giai doan 1 ===");

    /* 1. Ket noi Wi-Fi (block den khi co IP lan dau) */
    wifi_manager_init();

    /* 2. Khoi dong MQTT client */
    mqtt_manager_init();

    /* 3. Tao queue + khoi dong UART nhan du lieu tu STM32H7 */
    s_sensor_queue = xQueueCreate(SENSOR_QUEUE_LEN, sizeof(sensor_data_t));
    configASSERT(s_sensor_queue != NULL);
    uart_handler_init(s_sensor_queue);

    /* 4. Task publish du lieu */
    xTaskCreate(mqtt_publisher_task, "mqtt_publisher", 4096, NULL, 5, NULL);

    ESP_LOGI(TAG, "Khoi dong hoan tat, dang cho du lieu tu STM32H7...");
}
