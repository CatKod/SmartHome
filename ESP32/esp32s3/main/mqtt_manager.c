/**
 * @file mqtt_manager.c
 */
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "esp_log.h"
#include "mqtt_client.h"

#include "app_config.h"
#include "mqtt_manager.h"

static const char *TAG = "MQTT";
static esp_mqtt_client_handle_t s_client = NULL;
static volatile bool s_connected = false;
static mqtt_cmd_cb_t s_cmd_cb = NULL;

static void mqtt_publish_str(const char *topic, const char *payload, int retain)
{
    if (!s_connected || !s_client) return;
    esp_mqtt_client_publish(s_client, topic, payload, 0, 1, retain);
}

static void mqtt_event_handler(void *arg, esp_event_base_t base,
                               int32_t event_id, void *event_data)
{
    switch ((esp_mqtt_event_id_t)event_id) {
    case MQTT_EVENT_CONNECTED:
        s_connected = true;
        ESP_LOGI(TAG, "Ket noi broker thanh cong");
        mqtt_publish_str(APP_MQTT_TOPIC_STATUS, "online", 1);
        esp_mqtt_client_subscribe(s_client, APP_MQTT_TOPIC_CMD, 1);
        break;

    case MQTT_EVENT_DISCONNECTED:
        s_connected = false;
        ESP_LOGW(TAG, "Mat ket noi broker");
        break;

    case MQTT_EVENT_DATA: {
        esp_mqtt_event_handle_t event = event_data;
        if (event->topic_len == (int)strlen(APP_MQTT_TOPIC_CMD) &&
            strncmp(event->topic, APP_MQTT_TOPIC_CMD, event->topic_len) == 0 &&
            s_cmd_cb) {
            char cmd[48] = {0};
            size_t len = event->data_len < sizeof(cmd) - 1 ? (size_t)event->data_len : sizeof(cmd) - 1;
            memcpy(cmd, event->data, len);
            s_cmd_cb(cmd);
        }
        break;
    }

    default:
        break;
    }
}

void mqtt_manager_init(mqtt_cmd_cb_t cmd_cb)
{
    s_cmd_cb = cmd_cb;

    const esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = APP_MQTT_BROKER_URI,
        .credentials.username = APP_MQTT_USERNAME,
        .credentials.authentication.password = APP_MQTT_PASSWORD,
        .session.last_will.topic = APP_MQTT_TOPIC_STATUS,
        .session.last_will.msg = "offline",
        .session.last_will.msg_len = 7,
        .session.last_will.qos = 1,
        .session.last_will.retain = 1,
    };

    s_client = esp_mqtt_client_init(&mqtt_cfg);
    ESP_ERROR_CHECK(esp_mqtt_client_register_event(s_client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL));
    ESP_ERROR_CHECK(esp_mqtt_client_start(s_client));
}

bool mqtt_manager_is_connected(void)
{
    return s_connected;
}

void mqtt_manager_publish_online(void)
{
    mqtt_publish_str(APP_MQTT_TOPIC_STATUS, "online", 1);
}

void mqtt_manager_publish_state(const system_state_t *st)
{
    if (!st) return;

    char env[192];
    snprintf(env, sizeof(env),
             "{\"temp\":%.1f,\"humi\":%d,\"light_pct\":%d,\"light_state\":\"%s\","
             "\"motion\":%d,\"sound\":%d,\"heat_alarm\":%d,\"rain\":%d,"
             "\"device_overheat\":%s}",
             st->temperature, st->humidity, st->light_pct, st->light_level,
             st->motion, st->sound, st->heat_alarm, st->rain,
             st->heat_alarm ? "YES" : "NO");
    mqtt_publish_str(APP_MQTT_TOPIC_STATE_ENV, env, 1);
    mqtt_publish_str(APP_MQTT_TOPIC_STATE_MODE, st->mode, 1);
    mqtt_publish_str(APP_MQTT_TOPIC_STATE_LOCK, st->lock, 1);
    mqtt_publish_str(APP_MQTT_TOPIC_STATE_WINDOW, st->window, 1);
    mqtt_publish_str(APP_MQTT_TOPIC_STATE_HALL_LIGHT, st->hall_light_on ? "ON" : "OFF", 1);
    mqtt_publish_str(APP_MQTT_TOPIC_STATE_ROOM_LIGHT, st->room_light_on ? "ON" : "OFF", 1);
    mqtt_publish_str(APP_MQTT_TOPIC_STATE_ALARM, st->alarm ? "ON" : "OFF", 1);
}

void mqtt_manager_publish_event(const char *message)
{
    if (!message) return;
    char buf[160];
    time_t now = time(NULL);
    struct tm t;
    localtime_r(&now, &t);
    snprintf(buf, sizeof(buf), "%02d:%02d - %s", t.tm_hour, t.tm_min, message);
    mqtt_publish_str(APP_MQTT_TOPIC_EVENT, buf, 0);
}

void mqtt_manager_publish_cmd_result(const cmd_result_t *result)
{
    if (!result) return;
    char payload[128];
    if (result->reason[0] != '\0') {
        snprintf(payload, sizeof(payload),
                 "{\"cmd\":\"%s\",\"result\":\"%s\",\"reason\":\"%s\"}",
                 result->cmd, result->result, result->reason);
    } else {
        snprintf(payload, sizeof(payload),
                 "{\"cmd\":\"%s\",\"result\":\"%s\"}",
                 result->cmd, result->result);
    }
    mqtt_publish_str(APP_MQTT_TOPIC_CMD_RESULT, payload, 0);
}
