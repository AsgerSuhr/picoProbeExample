#include <stdio.h>
#include "pico/stdlib.h"



int main()
{
    stdio_init_all();

    int count = 0;
    while (true)
    {
        puts("Hello, world!");
        printf("%d\n", count);
        sleep_ms(500);
        count++;
    }

    return 0;
}
