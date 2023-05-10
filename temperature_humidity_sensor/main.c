#include "my_lib.h"

#include <regx51.h>
#include <limits.h>
#include <stdio.h>

void delay_22us() // 21 clock cycle
{
    uint8_t i;
    for (i = 0; i < 5; i++)
        ;
}

void delay_29us() // 27 clock cycle
{
    uint8_t i;
    for (i = 0; i < 7; i++)
        ;
}

void delay_52us() // 28 clock cycle
{
    uint8_t i;
    for (i = 0; i < 14; i++)
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
    REN = 1;                              // Enable receive mode
    TH1 = 0xFD;                           // Set baud rate, TH1 = 0x100 - 2^SMOD * F_osc / (32 * 12 * Bard rate)
    TR1 = 1;                              // Trun-on timer 1
    ES = 1;                               // Enable UART interrupt
    ET1 = 1;                              // Enable timer 1 interrupt
    EA = 1;                               // Enable Global Interrupt bit
    TI = 1;                               // UART ready
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
        delay_22us(); // 28us < Sensor pull high <70us, output 1, otherwise output 0
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
        delay_29us(); // 28us < Sensor pull high <70us, output 1, otherwise output 0
        if (AM2302_PIN == HIGH)
        {
            checksum |= 1; //  2 clock cycle
            while (AM2302_PIN == HIGH)
                ;
        }
    }

    printf("raw data = 0x%llX, ", value);
    printf("get checksum = 0x%X, ", (int16_t)checksum);
    printf("cal checksum = 0x%X, \n", (int16_t)CHECKSUM(value));
    if (CHECKSUM(value) == checksum)
    {
        return value;
    }
    return 0xFFFFFFFF;
}

bool request_am2302_data()
{
    volatile uint32_t value = {0};
    delay_ms(2000);
    AM2302_PIN = LOW;
    delay_ms(20);
    AM2302_PIN = HIGH;
    delay_52us();
    if (AM2302_PIN == HIGH)
    {
        printf("AM2302 no response\n");
        return FALSE;
    }

    while (AM2302_PIN == LOW)
        ;
    while (AM2302_PIN == HIGH)
        ;
    if ((value = get_am2302_data()) == ULONG_MAX)
    {
        printf("Checksum Wrong!\n");
        return FALSE;
    }

    printf("    Humidity = %.1f %%\n", (float)(value >> 16) / 10);
    printf("    Temperature = %.1f Celsius\n", (float)(value & 0xFFFF) / 10);
    return TRUE;
}

void serial_isr() interrupt 4 using 0
{
    if (RI == 1)
    {
        RI = 0; // Clear the receive interrupt flag
        printf("%c", SBUF);
        if (request_am2302_data())
            return;
        if (request_am2302_data())
            return;
        if (request_am2302_data())
            return;
    }
}

void main()
{
    init_uart();
    while (TRUE) // Waiting for UART receive interrupt
        ;
    return;
}