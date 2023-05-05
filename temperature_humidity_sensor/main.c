#include "my_lib.h"
#include <regx51.h>

#include <stdio.h>

void delay1()
{
    TMOD = (TMOD & ~T0_MASK_) | (T0_M0_); // TMOD: timer 0 as timer, mode 1 16-bit timer, disable INT
    TH0 = (0x10000 - CYCLE_PER_MS + 17) / 0x100;
    TL0 = (0x10000 - CYCLE_PER_MS + 17) % 0x100;
    TR0 = 1;     // Turn on timer 0
    while (!TF0) // Wait until timer overflow
        ;
    TF0 = 0; // Reset timer 0 flag
    TR0 = 0; // Turn off timer 0
    return;
}

void delay(uint16 ms)
{
    while (ms--)
    {
        delay1();
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
    while (1)
        ;
}

void main()
{
    init_uart();
    while (TRUE)
    {
        delay(1000);
        printf("test\n");
    }
    stop();
    return;
}