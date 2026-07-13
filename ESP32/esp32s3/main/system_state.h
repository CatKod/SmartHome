/**
 * @file system_state.h
 * @brief Trang thai he thong dong bo tu STM32.
 */
#ifndef SYSTEM_STATE_H
#define SYSTEM_STATE_H

#include <stdint.h>
#include <stdbool.h>

typedef struct {
    float   temperature;
    int     humidity;
    int     light_pct;
    char    light_level[12];
    int     motion;
    int     sound;
    int     heat_alarm;
    int     rain;
    char    lock[12];
    char    window[12];
    char    mode[16];
    int     hall_light_on;
    int     room_light_on;
    int     alarm;
    int     risk_score;
} system_state_t;

typedef struct {
    char cmd[32];
    char result[12];
    char reason[32];
} cmd_result_t;

#endif /* SYSTEM_STATE_H */
