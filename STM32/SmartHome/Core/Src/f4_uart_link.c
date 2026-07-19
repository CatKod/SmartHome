#include "main.h"
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#define UART_FRAME_STX 0x02U
#define UART_FRAME_ETX 0x03U

static UART_HandleTypeDef *s_huart = NULL;
static char s_rx_buffer[160];
static size_t s_rx_index = 0;
static uint8_t s_in_frame = 0;

/* Global variables for receiving data from F4 via UART */
float f4_rx_temperature = 0.0f;
int f4_rx_humidity = 0;
int f4_rx_light = 0;
bool f4_rx_is_logged_in = false;
bool f4_rx_warning_trigger = false;
int f4_rx_hour = 0;
int f4_rx_minute = 0;

void F4_UART_Link_Init(UART_HandleTypeDef *huart)
{
    s_huart = huart;
    s_rx_index = 0;
    s_in_frame = 0;
}

/**
  * @brief  Function to parse the received data from F4 to H7
  * @param  payload: Raw string received from H7
  */
void F4_UART_ParseData(char *payload)
{
    if (payload == NULL) return;

    // Parse the environmental data string from H7 including 24h clock data
    if (strncmp(payload, "DATA,", 5) == 0)
    {
        char light_lvl[10], lock_st[10], win_st[10], mode_st[15], hall_st[5], room_st[5];
        unsigned int mot, snd, heat, rain, alarm, risk;
        
        // Match the H7 package structure and extract variables accurately
        sscanf(payload + 5, "%f,%d,%d,%[^,],%u,%u,%u,%u,%[^,],%[^,],%[^,],%[^,],%[^,],%u,%u,%d,%d",
               &f4_rx_temperature, &f4_rx_humidity, &f4_rx_light,
               light_lvl, &mot, &snd, &heat, &rain, lock_st, win_st, mode_st, hall_st, room_st,
               &alarm, &risk, &f4_rx_hour, &f4_rx_minute);
    }
    
    // Parse the environmental data string from H7 (e.g., "ENV,28.5,65,450")
    if (strncmp(payload, "ENV,", 4) == 0)
    {
        sscanf(payload + 4, "%f,%d,%d", &f4_rx_temperature, &f4_rx_humidity, &f4_rx_light);
    }
    // Receive authorization status signal from H7 (e.g., "AUTH,1" - Success, "AUTH,0" - Locked)
    else if (strncmp(payload, "AUTH,", 5) == 0)
    {
        int auth_status = 0;
        sscanf(payload + 5, "%d", &auth_status);
        f4_rx_is_logged_in = (auth_status == 1);
    }
    // Receive security alert signal from physical sensors (e.g., "WARN,1" - Danger)
    else if (strncmp(payload, "WARN,", 5) == 0)
    {
        int warn_status = 0;
        sscanf(payload + 5, "%d", &warn_status);
        f4_rx_warning_trigger = (warn_status == 1);
    }
}

/**
  * @brief  Function to package the frame and send command from F4 screen back to the central processing unit H7
  * @param  active: true if security mode is active, false otherwise
  */
void F4_UART_SendHomeMode(bool active)
{
    if (s_huart == NULL) return;

    char tx_buf[32];
    // Package the frame according to the specified format <STX>CMD,SECURITY,x<ETX>
    int len = snprintf(tx_buf, sizeof(tx_buf), "%cCMD,SECURITY,%d%c", 
                       (char)UART_FRAME_STX, active ? 1 : 0, (char)UART_FRAME_ETX);
                       
    if (len > 0)
    {
        // Send the UART data in traditional polling mode to ensure the command is sent immediately
        HAL_UART_Transmit(s_huart, (uint8_t*)tx_buf, (uint16_t)len, 100);
    }
}

void F4_UART_Link_ReceiveHandler(uint8_t byte)
{
    /* Process the received byte */
    if (byte == UART_FRAME_STX)
    {
        s_in_frame = 1;
        s_rx_index = 0;
        return;
    }
    
    /* Check if we are currently receiving a frame */
    if (s_in_frame == 0) return;
    
    /* Check for the frame end byte */
    if (byte == UART_FRAME_ETX)
    {
        s_rx_buffer[s_rx_index] = '\0';
        s_in_frame = 0;
        
        /* Perform parsing of the received string */
        F4_UART_ParseData(s_rx_buffer);
        return;
    }
    
    /* Append the received byte to the buffer */
    if (s_rx_index < (sizeof(s_rx_buffer) - 1))
    {
        s_rx_buffer[s_rx_index++] = (char)byte;
    }
    else
    {
        /* Clear the index to avoid buffer overflow if the string is too long */
        s_in_frame = 0;
        s_rx_index = 0;
    }
}
