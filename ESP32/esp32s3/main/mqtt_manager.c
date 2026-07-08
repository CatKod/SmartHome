/**
 * @file mqtt_manager.c
 * @brief Ket noi broker MQTT (mqtt.toolhub.app) va publish JSON cam bien.
 *
 * esp-mqtt tu dong reconnect khi mat ket noi nen module nay chi can
 * theo doi trang thai qua event de biet luc nao duoc phep publish.
 */
#include <stdio.h>

#include "esp_log.h"
#include "mqtt_client.h"

#include "app_config.h"
#include "mqtt_manager.h"

static const char *TAG = "MQTT_MANAGER";

static esp_mqtt_client_handle_t s_client    = NULL;
static volatile bool            s_connected = false;

/**
 * @brief Xu ly cac su kien tu esp-mqtt (connected/disconnected/error...).
 */
static void mqtt_event_handler(void *arg, esp_event_base_t base,
                               int32_t event_id, void *event_data)
{
    switch ((esp_mqtt_event_id_t)event_id) {
    case MQTT_EVENT_CONNECTED:
        s_connected = true;
        ESP_LOGI(TAG, "Da ket noi broker %s", APP_MQTT_BROKER_URI);
        break;

    case MQTT_EVENT_DISCONNECTED:
        s_connected = false;
        ESP_LOGW(TAG, "Mat ket noi broker, esp-mqtt se tu reconnect");
        break;

    case MQTT_EVENT_ERROR: {
        esp_mqtt_event_handle_t event = event_data;
        if (event->error_handle->error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT) {
            ESP_LOGE(TAG, "Loi transport, errno=%d",
                     event->error_handle->esp_transport_sock_errno);
        }
        break;
    }

    default:
        break;
    }
}

void mqtt_manager_init(void)
{
    const esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri       = APP_MQTT_BROKER_URI,
        .credentials.username     = APP_MQTT_USERNAME,
        .credentials.authentication.password = APP_MQTT_PASSWORD,
    };

    s_client = esp_mqtt_client_init(&mqtt_cfg);
    ESP_ERROR_CHECK(esp_mqtt_client_register_event(
        s_client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL));
    ESP_ERROR_CHECK(esp_mqtt_client_start(s_client));

    ESP_LOGI(TAG, "Dang ket noi broker %s ...", APP_MQTT_BROKER_URI);
}

bool mqtt_manager_is_connected(void)
{
    return s_connected;
}

bool mqtt_manager_publish_sensor(const sensor_data_t *data)
{
    if (!s_connected) {
        ESP_LOGW(TAG, "Chua ket noi broker, bo qua ban tin");
        return false;
    }

    char payload[128];
    int len = snprintf(payload, sizeof(payload),
                       "{\"temperature\": %.1f, \"humidity\": %d, "
                       "\"light_status\": %d, \"motion_status\": %d}",
                       data->temperature, data->humidity,
                       data->light_status, data->motion_status);
    if (len <= 0 || len >= (int)sizeof(payload)) {
        ESP_LOGE(TAG, "Loi dong goi JSON");
        return false;
    }

    int msg_id = esp_mqtt_client_publish(s_client, APP_MQTT_TOPIC_SENSOR,
                                         payload, len,
                                         1 /* QoS 1 */, 0 /* khong retain */);
    if (msg_id < 0) {
        ESP_LOGE(TAG, "Publish that bai");
        return false;
    }

    ESP_LOGI(TAG, "Publish [%s]: %s", APP_MQTT_TOPIC_SENSOR, payload);
    return true;
}
