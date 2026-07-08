/**
 * @file uart_handler.h
 * @brief Nhan du lieu tu STM32H7 qua UART, tach frame STX/ETX va parse.
 *
 * Luong hoat dong:
 *   ISR/driver UART --> uart_rx_task (doc byte, ghep frame, parse CSV)
 *                   --> day sensor_data_t vao queue cho module khac dung.
 */
#ifndef UART_HANDLER_H
#define UART_HANDLER_H

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "sensor_types.h"

/**
 * @brief Khoi tao driver UART va tao task nhan du lieu.
 *
 * @param[in] out_queue Queue ma task se day sensor_data_t vao
 *                      moi khi nhan duoc mot frame hop le.
 */
void uart_handler_init(QueueHandle_t out_queue);

#endif /* UART_HANDLER_H */
