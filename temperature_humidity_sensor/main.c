#include <regx51.h>

#include <stdio.h>

void delay(int count)
{
    int i;
    for (i = 0; i < count; i++)
        ;
}

void init_uart()
{
    TMOD = (TMOD & T0_MASK_) | (T1_M1_); // TMOD: timer 1 as timer, mode 2, 8-bit reload, disable INT
    SM0 = 0;
    SM1 = 1;    // SCON: serial port mode 1
    TH1 = 0xFD; // Set baud rate, reload value =
    TR1 = 1;    // Trun-on timer 1
    REN = 1;    // Enable revicer mode
    ES = 1;     // Enable UART interrupt
    ET1 = 1;    // Enable timer 1 interrupt
    TI = 1;     // UART ready
}

void main()
{
    init_uart();
    while (1)
    {
        P0 = ~P0;
        printf("hello world\n");
        delay(10000);
    }
}