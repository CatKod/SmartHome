/**
 * @file uart_protocol.h
 * @brief Giao thuc UART <STX>TYPE,payload*CS<ETX> (STM32 <-> ESP32).
 */
#ifndef UART_PROTOCOL_H
#define UART_PROTOCOL_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#define UART_FRAME_STX   0x02U
#define UART_FRAME_ETX   0x03U
#define UART_FRAME_MAX   160U

uint8_t uart_proto_xor_checksum(const char *payload);
bool uart_proto_build_frame(char *out, size_t out_size, const char *payload);
bool uart_proto_verify_payload(const char *payload_with_cs);

#endif /* UART_PROTOCOL_H */
