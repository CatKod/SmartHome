/**
 * @file system_logic.h
 * @brief Logic trung tam SmartHome: FSM, cam bien, dieu khien, UART/MQTT bridge.
 */
#ifndef SYSTEM_LOGIC_H
#define SYSTEM_LOGIC_H

#include "system_types.h"

extern SystemData_t g_system;

void System_Init(void);
void System_Loop(void);
void System_OnUartCmd(const char *cmd);

#endif /* SYSTEM_LOGIC_H */
