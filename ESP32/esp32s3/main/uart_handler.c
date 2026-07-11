/**
 * @file uart_handler.c
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "driver/uart.h"
#include "esp_log.h"

#include "app_config.h"
#include "uart_protocol.h"
#include "uart_handler.h"

static const char *TAG = "UART";

static QueueHandle_t s_uart_event_queue = NULL;
static uart_frame_cb_t s_frame_cb = NULL;

static SemaphoreHandle_t s_ack_sem = NULL;
static cmd_result_t s_last_ack;
static volatile bool s_ack_pending = false;

static void uart_dispatch_payload(char *payload)
{
    if (!uart_proto_verify_payload(payload)) {
        ESP_LOGW(TAG, "Checksum sai, bo frame");
        return;
    }

    char *star = strrchr(payload, '*');
    if (star) *star = '\0';

    char *comma = strchr(payload, ',');
    if (!comma) return;

    *comma = '\0';
    const char *type = payload;
    const char *body = comma + 1;

    if (strcmp(type, "ACK") == 0 && s_ack_pending) {
        char body_copy[UART_FRAME_MAX];
        strncpy(body_copy, body, sizeof(body_copy) - 1);
        body_copy[sizeof(body_copy) - 1] = '\0';

        char *tok = strtok(body_copy, ",");
        if (tok) {
            strncpy(s_last_ack.cmd, tok, sizeof(s_last_ack.cmd) - 1);
            tok = strtok(NULL, ",");
            if (tok) {
                strncpy(s_last_ack.result, tok, sizeof(s_last_ack.result) - 1);
                tok = strtok(NULL, ",");
                if (tok) {
                    strncpy(s_last_ack.reason, tok, sizeof(s_last_ack.reason) - 1);
                } else {
                    s_last_ack.reason[0] = '\0';
                }
            }
        }
        s_ack_pending = false;
        xSemaphoreGive(s_ack_sem);
    }

    if (s_frame_cb) {
        s_frame_cb(type, body);
    }
}

static void frame_feed_byte(uint8_t byte)
{
    static char frame_buf[APP_FRAME_MAX_LEN];
    static size_t frame_len = 0;
    static bool in_frame = false;

    if (byte == UART_FRAME_STX) {
        in_frame = true;
        frame_len = 0;
        return;
    }
    if (!in_frame) return;

    if (byte == UART_FRAME_ETX) {
        frame_buf[frame_len] = '\0';
        in_frame = false;
        uart_dispatch_payload(frame_buf);
        return;
    }

    if (frame_len < sizeof(frame_buf) - 1) {
        frame_buf[frame_len++] = (char)byte;
    } else {
        in_frame = false;
        frame_len = 0;
    }
}

static void uart_rx_task(void *arg)
{
    uart_event_t event;
    uint8_t rx_chunk[128];

    for (;;) {
        if (xQueueReceive(s_uart_event_queue, &event, portMAX_DELAY) != pdTRUE) {
            continue;
        }
        switch (event.type) {
        case UART_DATA: {
            int len = uart_read_bytes(APP_UART_PORT_NUM, rx_chunk,
                                      sizeof(rx_chunk), pdMS_TO_TICKS(20));
            for (int i = 0; i < len; i++) {
                frame_feed_byte(rx_chunk[i]);
            }
            break;
        }
        case UART_FIFO_OVF:
        case UART_BUFFER_FULL:
            uart_flush_input(APP_UART_PORT_NUM);
            xQueueReset(s_uart_event_queue);
            break;
        default:
            break;
        }
    }
}

void uart_handler_init(uart_frame_cb_t cb)
{
    s_frame_cb = cb;
    s_ack_sem = xSemaphoreCreateBinary();
    configASSERT(s_ack_sem);

    const uart_config_t uart_config = {
        .baud_rate = APP_UART_BAUD_RATE,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };

    ESP_ERROR_CHECK(uart_driver_install(APP_UART_PORT_NUM,
                                        APP_UART_RX_BUF_SIZE,
                                        APP_UART_TX_BUF_SIZE,
                                        16,
                                        &s_uart_event_queue,
                                        0));
    ESP_ERROR_CHECK(uart_param_config(APP_UART_PORT_NUM, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(APP_UART_PORT_NUM,
                                 APP_UART_TX_PIN, APP_UART_RX_PIN,
                                 UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));

    xTaskCreate(uart_rx_task, "uart_rx_task", 4096, NULL, 10, NULL);
    ESP_LOGI(TAG, "UART san sang 2 chieu");
}

bool uart_handler_send_cmd(const char *cmd, cmd_result_t *out_result)
{
    if (!cmd) return false;

    char payload[64];
    snprintf(payload, sizeof(payload), "CMD,%s", cmd);

    char frame[APP_FRAME_MAX_LEN];
    if (!uart_proto_build_frame(frame, sizeof(frame), payload)) {
        return false;
    }

    for (int attempt = 0; attempt <= APP_CMD_RETRY; attempt++) {
        memset(&s_last_ack, 0, sizeof(s_last_ack));
        s_ack_pending = true;
        xSemaphoreTake(s_ack_sem, 0);

        uart_write_bytes(APP_UART_PORT_NUM, frame, strlen(frame));

        if (xSemaphoreTake(s_ack_sem, pdMS_TO_TICKS(APP_CMD_ACK_TIMEOUT_MS)) == pdTRUE) {
            if (out_result) {
                *out_result = s_last_ack;
            }
            return (strcmp(s_last_ack.result, "OK") == 0);
        }
        ESP_LOGW(TAG, "ACK timeout cmd=%s attempt=%d", cmd, attempt);
    }

    s_ack_pending = false;
    if (out_result) {
        strncpy(out_result->cmd, cmd, sizeof(out_result->cmd) - 1);
        strncpy(out_result->result, "TIMEOUT", sizeof(out_result->result) - 1);
        out_result->reason[0] = '\0';
    }
    return false;
}
