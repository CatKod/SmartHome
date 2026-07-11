/**
  ******************************************************************************
  * @file    rfid_rc522.c
  * @brief   Driver MFRC522 qua SPI1 (xem rfid_rc522.h).
  ******************************************************************************
  */

#include "rfid_rc522.h"
#include "spi.h"

/* ---- Thanh ghi MFRC522 (datasheet muc 9) ---- */
#define REG_COMMAND        0x01U
#define REG_COM_IRQ        0x04U
#define REG_DIV_IRQ        0x05U
#define REG_ERROR          0x06U
#define REG_FIFO_DATA      0x09U
#define REG_FIFO_LEVEL     0x0AU
#define REG_CONTROL        0x0CU
#define REG_BIT_FRAMING    0x0DU
#define REG_MODE           0x11U
#define REG_TX_CONTROL     0x14U
#define REG_TX_ASK         0x15U
#define REG_CRC_RESULT_H   0x21U
#define REG_CRC_RESULT_L   0x22U
#define REG_T_MODE         0x2AU
#define REG_T_PRESCALER    0x2BU
#define REG_T_RELOAD_H     0x2CU
#define REG_T_RELOAD_L     0x2DU
#define REG_VERSION        0x37U

/* ---- Lenh MFRC522 ---- */
#define CMD_IDLE           0x00U
#define CMD_CALC_CRC       0x03U
#define CMD_TRANSCEIVE     0x0CU
#define CMD_SOFT_RESET     0x0FU

/* ---- Lenh PICC (the) ---- */
#define PICC_REQA          0x26U
#define PICC_ANTICOLL_CL1  0x93U
#define PICC_HALT          0x50U

#define RC522_SPI_TIMEOUT_MS    10U
#define RC522_IRQ_TIMEOUT_MS    36U   /* > timeout 25 ms cua timer noi bo */

/* ---------------------------------------------------------------------------
 * Truy cap thanh ghi qua SPI: byte dia chi = (addr << 1), bit7 = 1 khi doc
 * ------------------------------------------------------------------------- */

static inline void RC522_Select(void)
{
  HAL_GPIO_WritePin(RFID_GPIO_Output_CS_GPIO_Port, RFID_GPIO_Output_CS_Pin, GPIO_PIN_RESET);
}

static inline void RC522_Deselect(void)
{
  HAL_GPIO_WritePin(RFID_GPIO_Output_CS_GPIO_Port, RFID_GPIO_Output_CS_Pin, GPIO_PIN_SET);
}

static void RC522_WriteReg(uint8_t addr, uint8_t value)
{
  uint8_t tx[2] = { (uint8_t)((addr << 1) & 0x7EU), value };

  RC522_Select();
  HAL_SPI_Transmit(&hspi1, tx, 2, RC522_SPI_TIMEOUT_MS);
  RC522_Deselect();
}

static uint8_t RC522_ReadReg(uint8_t addr)
{
  uint8_t tx[2] = { (uint8_t)(((addr << 1) & 0x7EU) | 0x80U), 0x00U };
  uint8_t rx[2] = { 0, 0 };

  RC522_Select();
  HAL_SPI_TransmitReceive(&hspi1, tx, rx, 2, RC522_SPI_TIMEOUT_MS);
  RC522_Deselect();
  return rx[1];
}

static void RC522_SetBits(uint8_t addr, uint8_t mask)
{
  RC522_WriteReg(addr, RC522_ReadReg(addr) | mask);
}

static void RC522_ClearBits(uint8_t addr, uint8_t mask)
{
  RC522_WriteReg(addr, RC522_ReadReg(addr) & (uint8_t)~mask);
}

/* ---------------------------------------------------------------------------
 * Trao doi khung du lieu voi the (lenh Transceive)
 * ------------------------------------------------------------------------- */

/**
 * @brief  Gui `tx_len` byte, nhan toi da `*rx_len` byte tu the.
 * @param  tx_bits  So bit hop le cua byte cuoi (0 = ca 8 bit),
 *                  REQA chi gui 7 bit.
 */
static RC522_Status_t RC522_Transceive(const uint8_t *tx, uint8_t tx_len, uint8_t tx_bits,
                                       uint8_t *rx, uint8_t *rx_len)
{
  RC522_WriteReg(REG_COMMAND, CMD_IDLE);
  RC522_WriteReg(REG_COM_IRQ, 0x7FU);              /* Xoa moi co IRQ        */
  RC522_SetBits(REG_FIFO_LEVEL, 0x80U);            /* Xoa FIFO              */

  for (uint8_t i = 0; i < tx_len; i++)
  {
    RC522_WriteReg(REG_FIFO_DATA, tx[i]);
  }

  RC522_WriteReg(REG_COMMAND, CMD_TRANSCEIVE);
  RC522_WriteReg(REG_BIT_FRAMING, 0x80U | (tx_bits & 0x07U));   /* StartSend */

  /* Cho RxIRq (0x20) hoac IdleIRq (0x10); TimerIRq (0x01) = khong co the */
  uint32_t start = HAL_GetTick();
  uint8_t  irq;
  for (;;)
  {
    irq = RC522_ReadReg(REG_COM_IRQ);
    if ((irq & 0x30U) != 0U)
    {
      break;
    }
    if (((irq & 0x01U) != 0U) || ((HAL_GetTick() - start) > RC522_IRQ_TIMEOUT_MS))
    {
      return RC522_ERR_NO_CARD;
    }
  }

  /* BufferOvfl / CollErr / ParityErr / ProtocolErr */
  if ((RC522_ReadReg(REG_ERROR) & 0x1BU) != 0U)
  {
    return RC522_ERR_COMM;
  }

  uint8_t fifo_len = RC522_ReadReg(REG_FIFO_LEVEL);
  if (fifo_len > *rx_len)
  {
    fifo_len = *rx_len;
  }
  for (uint8_t i = 0; i < fifo_len; i++)
  {
    rx[i] = RC522_ReadReg(REG_FIFO_DATA);
  }
  *rx_len = fifo_len;

  return RC522_OK;
}

/** @brief Tinh CRC_A bang khoi CRC cua MFRC522. */
static void RC522_CalcCRC(const uint8_t *data, uint8_t len, uint8_t *crc)
{
  RC522_WriteReg(REG_COMMAND, CMD_IDLE);
  RC522_SetBits(REG_FIFO_LEVEL, 0x80U);
  for (uint8_t i = 0; i < len; i++)
  {
    RC522_WriteReg(REG_FIFO_DATA, data[i]);
  }
  RC522_WriteReg(REG_DIV_IRQ, 0x04U);      /* Xoa co CRCIRq */
  RC522_WriteReg(REG_COMMAND, CMD_CALC_CRC);

  uint32_t start = HAL_GetTick();
  while ((RC522_ReadReg(REG_DIV_IRQ) & 0x04U) == 0U)   /* Cho CRCIRq */
  {
    if ((HAL_GetTick() - start) > RC522_IRQ_TIMEOUT_MS)
    {
      break;
    }
  }

  crc[0] = RC522_ReadReg(REG_CRC_RESULT_L);
  crc[1] = RC522_ReadReg(REG_CRC_RESULT_H);
  RC522_WriteReg(REG_COMMAND, CMD_IDLE);
}

/* ---------------------------------------------------------------------------
 * API
 * ------------------------------------------------------------------------- */

void RC522_Init(void)
{
  /* CS nghi o muc HIGH; nha chan reset (gpio.c de mac dinh LOW) */
  RC522_Deselect();
  HAL_GPIO_WritePin(RFID_GPIO_Output_RST_GPIO_Port, RFID_GPIO_Output_RST_Pin, GPIO_PIN_SET);
  HAL_Delay(50);

  RC522_WriteReg(REG_COMMAND, CMD_SOFT_RESET);
  HAL_Delay(50);

  /* Timer noi bo: 13.56MHz/(2*169+1) ~ 40 kHz, reload 1000 => ~25 ms timeout */
  RC522_WriteReg(REG_T_MODE,        0x8DU);
  RC522_WriteReg(REG_T_PRESCALER,   0xA9U);
  RC522_WriteReg(REG_T_RELOAD_H,    0x03U);
  RC522_WriteReg(REG_T_RELOAD_L,    0xE8U);

  RC522_WriteReg(REG_TX_ASK,        0x40U);   /* 100% ASK                    */
  RC522_WriteReg(REG_MODE,          0x3DU);   /* CRC preset 0x6363 (ISO14443A) */

  /* Bat anten (TX1, TX2) */
  RC522_SetBits(REG_TX_CONTROL, 0x03U);
}

uint8_t RC522_GetVersion(void)
{
  return RC522_ReadReg(REG_VERSION);
}

RC522_Status_t RC522_ReadCardUID(uint8_t *uid)
{
  uint8_t buf[8];
  uint8_t rx_len;
  RC522_Status_t status;

  /* --- REQA: danh thuc the (khung ngan 7 bit) --- */
  buf[0] = PICC_REQA;
  rx_len = sizeof(buf);
  status = RC522_Transceive(buf, 1, 7, buf, &rx_len);
  if (status != RC522_OK)
  {
    return status;
  }
  if (rx_len != 2U)                     /* ATQA phai dai 2 byte */
  {
    return RC522_ERR_COMM;
  }

  /* --- Anticollision cap 1: nhan 4 byte UID + 1 byte BCC --- */
  buf[0] = PICC_ANTICOLL_CL1;
  buf[1] = 0x20U;
  rx_len = sizeof(buf);
  RC522_WriteReg(REG_BIT_FRAMING, 0x00U);
  status = RC522_Transceive(buf, 2, 0, buf, &rx_len);
  if (status != RC522_OK)
  {
    return status;
  }
  if (rx_len != 5U)
  {
    return RC522_ERR_COMM;
  }

  /* Kiem tra BCC = XOR 4 byte UID */
  if ((uint8_t)(buf[0] ^ buf[1] ^ buf[2] ^ buf[3]) != buf[4])
  {
    return RC522_ERR_COMM;
  }

  uid[0] = buf[0];
  uid[1] = buf[1];
  uid[2] = buf[2];
  uid[3] = buf[3];
  return RC522_OK;
}

void RC522_Halt(void)
{
  uint8_t buf[4] = { PICC_HALT, 0x00U, 0, 0 };
  uint8_t rx_len = 0;
  uint8_t dummy[1];

  RC522_CalcCRC(buf, 2, &buf[2]);
  /* The o trang thai HALT khong tra loi => bo qua ket qua */
  (void)RC522_Transceive(buf, 4, 0, dummy, &rx_len);
}
