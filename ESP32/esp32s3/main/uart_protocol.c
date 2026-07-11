#include "uart_protocol.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

uint8_t uart_proto_xor_checksum(const char *payload)
{
    uint8_t cs = 0;
    if (!payload) return 0;
    for (size_t i = 0; payload[i] != '\0'; i++) {
        cs ^= (uint8_t)payload[i];
    }
    return cs;
}

bool uart_proto_build_frame(char *out, size_t out_size, const char *payload)
{
    if (!out || !payload || out_size < 8) return false;
    uint8_t cs = uart_proto_xor_checksum(payload);
    int len = snprintf(out, out_size, "%c%s*%02X%c",
                       (char)UART_FRAME_STX, payload, (unsigned)cs, (char)UART_FRAME_ETX);
    return (len > 0) && ((size_t)len < out_size);
}

bool uart_proto_verify_payload(const char *payload_with_cs)
{
    if (!payload_with_cs) return false;
    const char *star = strrchr(payload_with_cs, '*');
    if (!star || star == payload_with_cs) return false;

    unsigned int rx_cs = 0;
    if (sscanf(star + 1, "%2X", &rx_cs) != 1) return false;

    size_t body_len = (size_t)(star - payload_with_cs);
    char body[UART_FRAME_MAX];
    if (body_len >= sizeof(body)) return false;

    memcpy(body, payload_with_cs, body_len);
    body[body_len] = '\0';
    return uart_proto_xor_checksum(body) == (uint8_t)rx_cs;
}
