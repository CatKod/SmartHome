#ifndef UART_HANDLER_H
#define UART_HANDLER_H

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "system_state.h"

typedef void (*uart_frame_cb_t)(const char *type, const char *body);

void uart_handler_init(uart_frame_cb_t cb);
bool uart_handler_send_cmd(const char *cmd, cmd_result_t *out_result);

#endif /* UART_HANDLER_H */
