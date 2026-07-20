/**
 * @file uart_link.h
 * @brief Lien ket UART3, UART4 voi ESP32: TX queue + RX frame parser.
 */
#ifndef UART_LINK_H
#define UART_LINK_H

#include <stdint.h>
#include <stdbool.h>

typedef void (*UartCmdHandler_t)(const char *cmd);

void UartLink_Init(UartCmdHandler_t cmd_handler);
void UartLink_StartRx(void);
bool UartLink_SendPayload(const char *payload);
bool UartLink_SendToF4(const char *payload);
void UartLink_Poll(void);
void UartLink_OnRxCplt(void);
void UartLink_OnTxComplete(void);

#endif /* UART_LINK_H */
