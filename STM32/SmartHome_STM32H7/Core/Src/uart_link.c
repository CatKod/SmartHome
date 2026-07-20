/**
 * @file uart_link.c
 */
#include "uart_link.h"
#include "uart_protocol.h"
#include "system_config.h"
#include "usart.h"
#include <string.h>
#include <stdio.h>

#define UART_TX_QUEUE_DEPTH 4U

typedef struct
{
  char buf[UART_LINK_TX_BUF_LEN];
} UartTxSlot_t;

static UartCmdHandler_t s_cmd_handler = NULL;
static UartTxSlot_t s_tx_queue[UART_TX_QUEUE_DEPTH];
static volatile uint8_t s_tx_head = 0U;
static volatile uint8_t s_tx_tail = 0U;
static volatile uint8_t s_tx_busy = 0U;

static char s_rx_frame[UART_FRAME_MAX];
static size_t s_rx_len = 0U;
static uint8_t s_in_frame = 0U;

static uint8_t s_rx_byte = 0U;

static bool UartLink_TxEnqueue(const char *frame)
{
  uint8_t next = (uint8_t)((s_tx_head + 1U) % UART_TX_QUEUE_DEPTH);
  if (next == s_tx_tail)
  {
    return false;
  }
  strncpy(s_tx_queue[s_tx_head].buf, frame, UART_LINK_TX_BUF_LEN - 1U);
  s_tx_queue[s_tx_head].buf[UART_LINK_TX_BUF_LEN - 1U] = '\0';
  s_tx_head = next;
  return true;
}

static void UartLink_TxKick(void)
{
  if (s_tx_busy != 0U || s_tx_tail == s_tx_head)
  {
    return;
  }

  s_tx_busy = 1U;
  if (HAL_UART_Transmit_IT(&huart3,
                           (uint8_t *)s_tx_queue[s_tx_tail].buf,
                           (uint16_t)strlen(s_tx_queue[s_tx_tail].buf)) != HAL_OK)
  {
    s_tx_busy = 0U;
  }
}

static void UartLink_HandlePayload(char *payload)
{
  if (!UartProto_VerifyPayload(payload))
  {
    return;
  }

  char *star = strrchr(payload, '*');
  if (star != NULL)
  {
    *star = '\0';
  }

  if (strncmp(payload, "CMD,", 4) == 0 && s_cmd_handler != NULL)
  {
    s_cmd_handler(payload + 4);
  }
}

static void UartLink_FeedByte(uint8_t byte)
{
  if (byte == UART_FRAME_STX)
  {
    s_in_frame = 1U;
    s_rx_len = 0U;
    return;
  }
  if (s_in_frame == 0U)
  {
    return;
  }
  if (byte == UART_FRAME_ETX)
  {
    s_rx_frame[s_rx_len] = '\0';
    s_in_frame = 0U;
    UartLink_HandlePayload(s_rx_frame);
    return;
  }
  if (s_rx_len < (sizeof(s_rx_frame) - 1U))
  {
    s_rx_frame[s_rx_len++] = (char)byte;
  }
  else
  {
    s_in_frame = 0U;
    s_rx_len = 0U;
  }
}

void UartLink_Init(UartCmdHandler_t cmd_handler)
{
  s_cmd_handler = cmd_handler;
  s_tx_head = s_tx_tail = 0U;
  s_tx_busy = 0U;
  s_in_frame = 0U;
  s_rx_len = 0U;
}

void UartLink_StartRx(void)
{
  (void)HAL_UART_Receive_IT(&huart3, &s_rx_byte, 1U);
}

bool UartLink_SendPayload(const char *payload)
{
  char frame[UART_LINK_TX_BUF_LEN];
  if (!UartProto_BuildFrame(frame, sizeof(frame), payload))
  {
    return false;
  }
  if (!UartLink_TxEnqueue(frame))
  {
    return false;
  }
  UartLink_TxKick();
  return true;
}

void UartLink_Poll(void)
{
  UartLink_TxKick();
}

void UartLink_OnRxCplt(void)
{
  UartLink_FeedByte(s_rx_byte);
  (void)HAL_UART_Receive_IT(&huart3, &s_rx_byte, 1U);
}

void UartLink_OnTxComplete(void)
{
  if (s_tx_tail != s_tx_head)
  {
    s_tx_tail = (uint8_t)((s_tx_tail + 1U) % UART_TX_QUEUE_DEPTH);
  }
  s_tx_busy = 0U;
  UartLink_TxKick();
}

/* USER CODE BEGIN 1 */
/**
  * @brief  Bổ sung luồng phát dữ liệu đồng bộ (Polling) từ H7 sang màn hình F4 qua UART4
  * @param  payload: Chuỗi nội dung thô cần truyền (Ví dụ: "DATA,...")
  */
bool UartLink_SendToF4(const char *payload)
{
  char frame[UART_LINK_TX_BUF_LEN];
  if (payload == NULL) return false;

  // Đóng gói payload thành khung hoàn chỉnh có <STX> ở đầu và <ETX> ở đuôi qua uart_protocol.c
  if (!UartProto_BuildFrame(frame, sizeof(frame), payload))
  {
    return false;
  }

  // Phát trực tiếp bằng chế độ Polling thời gian chờ 100ms chặn ngắn, không tranh chấp ngắt với ESP32
  if (HAL_UART_Transmit(&huart4, (uint8_t *)frame, (uint16_t)strlen(frame), 100) == HAL_OK)
  {
    return true;
  }
  return false;
}
/* USER CODE END 1 */
