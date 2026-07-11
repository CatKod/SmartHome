/**
 * @file uart_protocol.c
 */
#include "uart_protocol.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

uint8_t UartProto_XorChecksum(const char *payload)
{
  uint8_t cs = 0U;

  if (payload == NULL)
  {
    return 0U;
  }

  for (size_t i = 0; payload[i] != '\0'; i++)
  {
    cs ^= (uint8_t)payload[i];
  }
  return cs;
}

bool UartProto_BuildFrame(char *out, size_t out_size, const char *payload)
{
  if (out == NULL || payload == NULL || out_size < 8U)
  {
    return false;
  }

  uint8_t cs = UartProto_XorChecksum(payload);
  int len = snprintf(out, out_size, "%c%s*%02X%c",
                     (char)UART_FRAME_STX, payload, (unsigned)cs, (char)UART_FRAME_ETX);
  return (len > 0) && ((size_t)len < out_size);
}

bool UartProto_VerifyPayload(const char *payload_with_cs)
{
  if (payload_with_cs == NULL)
  {
    return false;
  }

  const char *star = strrchr(payload_with_cs, '*');
  if (star == NULL || star == payload_with_cs)
  {
    return false;
  }

  unsigned int rx_cs = 0U;
  if (sscanf(star + 1, "%2X", &rx_cs) != 1)
  {
    return false;
  }

  size_t body_len = (size_t)(star - payload_with_cs);
  char body[UART_FRAME_MAX];

  if (body_len >= sizeof(body))
  {
    return false;
  }

  memcpy(body, payload_with_cs, body_len);
  body[body_len] = '\0';
  return UartProto_XorChecksum(body) == (uint8_t)rx_cs;
}
