#include "my_lib.h"
#include <regx51.h>

#include <stdio.h>

void delay_13us() // This function use 12 clock cycle =~ 13 us
{
    uint8_t i;
    for (i = 0; i < 2; i++)
        ;
}

void delay_21us()
{
    uint8_t i;
    for (i = 0; i < 5; i++)
        ;
}

void delay_30us()
{
    uint8_t i;
    for (i = 0; i < 8; i++)
        ;
}

void delay_ms(uint16_t ms) // this function use 16 clock cycle (without delay1)
{
    while (ms--) // while use 6 clock cycle
    {
        TMOD = (TMOD & ~T0_MASK_) | (T0_M0_);        // TMOD: timer 0 as timer, mode 1 16-bit timer, disable INT
        TH0 = (0x10000 - CYCLE_PER_MS + 16) / 0x100; // this function use 16 clock cycle to run
        TL0 = (0x10000 - CYCLE_PER_MS + 16) % 0x100;
        TR0 = 1;     // Turn on timer 0
        while (!TF0) // Wait until timer overflow
            ;
        TF0 = 0; // Reset timer 0 flag
        TR0 = 0; // Turn off timer 0
    }
}

void init_uart()
{
    TMOD = (TMOD & ~T1_MASK_) | (T1_M1_); // TMOD: timer 1 as timer, mode 2, 8-bit reload, disable INT
    SM0 = 0;                              // SCON: SM0 = 0, SM1 = 1 -> mode1
    SM1 = 1;                              // SCON: serial port mode 1
    TH1 = 0xFD;                           // Set baud rate, TH1 = 0x100 - 2^SMOD * F_osc / (32 * 12 * Bard rate)
    TR1 = 1;                              // Trun-on timer 1
    REN = 1;                              // Enable revicer mode
    ES = 1;                               // Enable UART interrupt
    ET1 = 1;                              // Enable timer 1 interrupt
    TI = 1;                               // UART ready
}

void stop()
{
    while (TRUE)
        ;
}

uint32_t get_am2302_data()
{
    uint32_t value = 0;
    uint8_t checksum = 0;
    int8_t bits = 32;

    while (bits--)
    {
        value <<= 1;              //  9 clock cycle
        while (AM2302_PIN == LOW) // Sensor pulls low 50us
            ;
        delay_21us(); // 28us < Sensor pull high <70us, output 1, otherwise output 0
        if (AM2302_PIN == HIGH)
        {
            value |= 1; //  9 clock cycle
            while (AM2302_PIN == HIGH)
                ;
        }
    }
    bits = 8;
    while (bits--)
    {
        checksum <<= 1;           //  3 clock cycle
        while (AM2302_PIN == LOW) // Sensor pulls low 50us
            ;
        delay_30us(); // 28us < Sensor pull high <70us, output 1, otherwise output 0
        if (AM2302_PIN == HIGH)
        {
            checksum |= 1; //  2 clock cycle
            while (AM2302_PIN == HIGH)
                ;
        }
    }

    printf("data = 0x%llX, ", value);
    printf("rh = %d, ", (int16_t)(value >> 16));
    printf("temp = %d, ", (int16_t)(value & 0xFFFF));
    printf("get checksum = 0x%X, ", (int16_t)checksum);
    printf("cal checksum = 0x%X, \n", (int16_t)CHECKSUM(value));
    if (CHECKSUM(value) == checksum)
    {
        return value;
    }
    return 0xFFFFFFFF;
}

bool is_data_valid(AM2302_DATA value)
{
    uint8_t result = 0;
    result = (uint8_t)((value.sensor_data.humidity & 0xFF) + ((value.sensor_data.humidity >> 8) & 0xFF) + (value.sensor_data.temperature & 0xFF) + ((value.sensor_data.temperature >> 8) & 0xFF));

    printf("result = %x\n", result & 0xFF);
    if (result != value.checksum)
    {
        return FALSE;
    }

    return TRUE;
}

bool request_am2302_data()
{
    volatile uint32_t value = {0};
    AM2302_PIN = LOW;
    delay_ms(20);
    AM2302_PIN = HIGH;
    delay_13us();
    delay_13us();
    delay_13us();
    delay_13us();
    if (AM2302_PIN == HIGH)
    {
        printf("AM2302 no response\n");
        return FALSE;
    }

    while (AM2302_PIN == LOW)
        ;
    while (AM2302_PIN == HIGH)
        ;
    value = get_am2302_data();

    printf("AM2302 data = 0x%llX\n", value);
}

void main()
{
    init_uart();
    while (TRUE)
    {
        delay_ms(4000);
        request_am2302_data();
    }
    // stop();
    return;
}