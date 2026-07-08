/**
 * @file sensor_types.h
 * @brief Kieu du lieu cam bien dung chung giua cac module.
 */
#ifndef SENSOR_TYPES_H
#define SENSOR_TYPES_H

#include <stdint.h>

typedef struct {
    float   temperature;    /* Nhiet do [do C]            */
    int     humidity;       /* Do am [%]                  */
    int     light_status;   /* Trang thai anh sang 0/1    */
    int     motion_status;  /* Trang thai chuyen dong 0/1 */
} sensor_data_t;

#endif /* SENSOR_TYPES_H */
