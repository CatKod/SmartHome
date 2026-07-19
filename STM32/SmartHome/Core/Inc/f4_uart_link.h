#ifndef F4_UART_LINK_H
#define F4_UART_LINK_H

#include "main.h"
#include <stdint.h>
#include <stdbool.h>

// Wrap in extern "C" so C++ files can link to these C functions
#ifdef __cplusplus
extern "C" {
#endif

// Declare all global variables used across files
extern float f4_rx_temperature;
extern int f4_rx_humidity;
extern int f4_rx_light;
extern bool f4_rx_is_logged_in;
extern bool f4_rx_warning_trigger;
extern int f4_rx_hour;
extern int f4_rx_minute;

// Declare functions
void F4_UART_Link_Init(UART_HandleTypeDef *huart);
void F4_UART_SendHomeMode(bool active);
void F4_UART_Link_ReceiveHandler(uint8_t byte);

#ifdef __cplusplus
}
#endif

#endif