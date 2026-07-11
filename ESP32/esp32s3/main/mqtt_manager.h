#ifndef MQTT_MANAGER_H
#define MQTT_MANAGER_H

#include <stdbool.h>
#include "system_state.h"

typedef void (*mqtt_cmd_cb_t)(const char *cmd);

void mqtt_manager_init(mqtt_cmd_cb_t cmd_cb);
bool mqtt_manager_is_connected(void);
void mqtt_manager_publish_online(void);
void mqtt_manager_publish_state(const system_state_t *st);
void mqtt_manager_publish_event(const char *message);
void mqtt_manager_publish_cmd_result(const cmd_result_t *result);

#endif /* MQTT_MANAGER_H */
