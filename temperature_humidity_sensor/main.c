#include "my_lib.h"
#include <regx51.h>

#include <stdio.h>

void delay_13us() // This function use 12 clock cycle =~ 13 us
{
    uint8_t i;
    for (i = 0; i < 2; i++)
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

AM2302_DATA get_am2302_data()
{
    AM2302_DATA result = {0};
    uint32_t value = 0;
    uint8_t checksum = 0;
    uint8_t bits = 0;
    for (bits = 0; bits < 32; bits++)
    {

        while (AM2302_PIN == LOW) // Sensor pulls low 50us
            ;
        delay_13us();
        delay_13us();
        delay_13us(); // 28us < Sensor pull high <70us, output 1, otherwise output 0
        value <<= 1;
        if (AM2302_PIN == HIGH)
        {
            value |= 1;
        }
    }
    for (bits = 0; bits < 8; bits++)
    {

        while (AM2302_PIN == LOW)
            ;
        delay_13us();
        delay_13us();
        delay_13us();
        checksum <<= 1;
        if (AM2302_PIN == HIGH)
        {
            checksum |= 1;
        }
    }
    result.sensor_data = *(RH_TEMP *)&value;
    result.checksum = checksum;
    return result;
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

void main()
{
    AM2302_DATA value = {0};

    init_uart();
    delay_ms(3000);

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
        return;
    }

    while (AM2302_PIN == LOW)
        ;
    while (AM2302_PIN == HIGH)
        ;
    value = get_am2302_data();

    printf("AM2302 data = 0x%llX\n", *(RH_TEMP *)&value.sensor_data);
    printf("AM2302 checksum = %x\n", value.checksum & 0xFF);
    printf("humidity = %u\n", value.sensor_data.humidity);
    printf("temperature = %u\n", value.sensor_data.temperature);
    if (!is_data_valid(value))
    {
        printf("CHECKSUM WRONG");
    }
    // stop();
    return;
}