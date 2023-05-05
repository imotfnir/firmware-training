#include <regx51.H>

void delay(int count)
{
    int i;
    for (i = 0; i < count; i++);
}

void main()
{
    while (1)
    {
        P0 = ~P0;
        delay(10000);
    }
}