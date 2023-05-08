#ifndef _MY_LIB_H_
#define _MY_LIB_H_

#define TRUE 1
#define FALSE 0

#define CYCLE_PER_MS 921 // 1ms / (12 / 11.0592MHz) = 921.6
#define AM2302_PIN P2_0
#define HIGH 1
#define LOW 0

typedef int bool;
typedef char int8_t;
typedef int int16_t;
typedef long int32_t;
typedef unsigned char uint8_t;
typedef unsigned int uint16_t;
typedef unsigned long uint32_t;

typedef struct
{
    uint16_t humidity;
    uint16_t temperature;
} RH_TEMP;

typedef struct
{
    RH_TEMP sensor_data;
    uint8_t checksum;
} AM2302_DATA;

#endif // _MY_LIB_H_