#ifndef _MY_LIB_H_
#define _MY_LIB_H_

#include <regx51.h>

#include <stdio.h>

#define TRUE 1
#define FALSE 0
#define CYCLE_PER_MS 921 // 1ms / (12 / 11.0592MHz) = 921.6
#define AM2302_PIN P0_0
#define HIGH 1
#define LOW 0

#define CHECKSUM(x) ((((x)&0xFF) + ((x) >> 8) + ((x) >> 16) + ((x) >> 24)) & 0xFF)

typedef int bool;
typedef char int8_t;
typedef int int16_t;
typedef long int32_t;
typedef unsigned char uint8_t;
typedef unsigned int uint16_t;
typedef unsigned long uint32_t;

typedef struct
{
    int16_t humidity;
    int16_t temperature;
} RH_TEMP;

typedef union
{
    RH_TEMP sensor_data;
    uint8_t raw_data;
} AM2302_DATA;

#endif // _MY_LIB_H_