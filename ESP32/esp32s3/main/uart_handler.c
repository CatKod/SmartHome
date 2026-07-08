/**
 * @file uart_handler.c
 * @brief Doc UART bat dong bo (event queue cua driver), tach frame
 *        <STX>Temp,Humi,Light,Motion<ETX> va parse thanh sensor_data_t.
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/uart.h"
#include "esp_log.h"

#include "app_config.h"
#include "uart_handler.h"

static const char *TAG = "UART_HANDLER";

/* Queue nhan event tu driver UART (du lieu den, loi, tran buffer...) */
static QueueHandle_t s_uart_event_queue = NULL;

/* Queue dau ra: gui sensor_data_t sang module MQTT */
static QueueHandle_t s_sensor_queue = NULL;

/* ---------------------------------------------------------------------- */
/*                          Parse frame CSV                                */
/* ---------------------------------------------------------------------- */

/**
 * @brief Parse chuoi "27.5,65,1,0" thanh sensor_data_t.
 *
 * @return true neu parse du 4 truong hop le.
 */
static bool parse_sensor_payload(const char *payload, sensor_data_t *out)
{
    float temp;
    int humi, light, motion;

    int matched = sscanf(payload, "%f,%d,%d,%d", &temp, &humi, &light, &motion);
    if (matched != 4) {
        ESP_LOGW(TAG, "Frame sai dinh dang (parse duoc %d/4 truong): '%s'",
                 matched, payload);
        return false;
    }

    out->temperature   = temp;
    out->humidity      = humi;
    out->light_status  = light;
    out->motion_status = motion;
    return true;
}

/* ---------------------------------------------------------------------- */
/*                    May trang thai ghep frame STX/ETX                    */
/* ---------------------------------------------------------------------- */

/**
 * @brief Nap tung byte vao may trang thai. Khi gap du cap STX..ETX thi
 *        parse va day ket qua vao queue dau ra.
 *
 * Cach hoat dong:
 *   - Gap STX  : reset buffer, bat dau thu thap.
 *   - Gap ETX  : ket thuc frame -> parse.
 *   - Byte khac: neu dang trong frame thi luu vao buffer.
 * Frame qua dai (nhieu/lech khung) se bi huy de dong bo lai tu STX ke tiep.
 */
static void frame_feed_byte(uint8_t byte)
{
    static char   frame_buf[APP_FRAME_MAX_LEN];
    static size_t frame_len  = 0;
    static bool   in_frame   = false;

    if (byte == APP_FRAME_STX) {
        in_frame  = true;
        frame_len = 0;
        return;
    }

    if (!in_frame) {
        return;                      /* Bo qua rac ngoai frame */
    }

    if (byte == APP_FRAME_ETX) {
        frame_buf[frame_len] = '\0';
        in_frame = false;

        sensor_data_t data;
        if (parse_sensor_payload(frame_buf, &data)) {
            ESP_LOGI(TAG, "Nhan: temp=%.1f humi=%d light=%d motion=%d",
                     data.temperature, data.humidity,
                     data.light_status, data.motion_status);

            /* Neu queue day thi bo ban ghi cu, uu tien du lieu moi nhat */
            if (xQueueSend(s_sensor_queue, &data, 0) != pdTRUE) {
                sensor_data_t dropped;
                xQueueReceive(s_sensor_queue, &dropped, 0);
                xQueueSend(s_sensor_queue, &data, 0);
            }
        }
        return;
    }

    if (frame_len < sizeof(frame_buf) - 1) {
        frame_buf[frame_len++] = (char)byte;
    } else {
        ESP_LOGW(TAG, "Frame qua dai, huy va cho STX moi");
        in_frame  = false;
        frame_len = 0;
    }
}

/* ---------------------------------------------------------------------- */
/*                             Task nhan UART                              */
/* ---------------------------------------------------------------------- */

/**
 * @brief Task ngu tren event queue cua driver UART (khong polling).
 *        Khi co du lieu den thi doc het ra va nap vao may trang thai frame.
 */
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
            /* Tran buffer: xoa sach va dong bo lai tu frame ke tiep */
            ESP_LOGW(TAG, "UART overflow/buffer full, flush");
            uart_flush_input(APP_UART_PORT_NUM);
            xQueueReset(s_uart_event_queue);
            break;

        default:
            break;
        }
    }
}

/* ---------------------------------------------------------------------- */
/*                               Khoi tao                                  */
/* ---------------------------------------------------------------------- */

void uart_handler_init(QueueHandle_t out_queue)
{
    s_sensor_queue = out_queue;

    const uart_config_t uart_config = {
        .baud_rate  = APP_UART_BAUD_RATE,
        .data_bits  = UART_DATA_8_BITS,
        .parity     = UART_PARITY_DISABLE,
        .stop_bits  = UART_STOP_BITS_1,
        .flow_ctrl  = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };

    ESP_ERROR_CHECK(uart_driver_install(APP_UART_PORT_NUM,
                                        APP_UART_RX_BUF_SIZE,
                                        0,                    /* khong dung TX buffer */
                                        16,                   /* do sau event queue   */
                                        &s_uart_event_queue,
                                        0));
    ESP_ERROR_CHECK(uart_param_config(APP_UART_PORT_NUM, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(APP_UART_PORT_NUM,
                                 APP_UART_TX_PIN, APP_UART_RX_PIN,
                                 UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));

    xTaskCreate(uart_rx_task, "uart_rx_task", 4096, NULL, 10, NULL);

    ESP_LOGI(TAG, "UART%d san sang (TX=%d RX=%d baud=%d)",
             APP_UART_PORT_NUM, APP_UART_TX_PIN, APP_UART_RX_PIN,
             APP_UART_BAUD_RATE);
}
