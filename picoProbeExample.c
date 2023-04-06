#include <stdio.h>
#include "pico/stdlib.h"


int main()
{
    stdio_init_all();
    const uint LED_PIN = PICO_DEFAULT_LED_PIN;
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    int count = 0;
    while (true)
    {
        gpio_put(LED_PIN, 1);
        puts("Hello, world!");
        printf("%d\n", count);
        sleep_ms(250);
        gpio_put(LED_PIN, 0);
        sleep_ms(250);
        count++;
    }

    return 0;
}
