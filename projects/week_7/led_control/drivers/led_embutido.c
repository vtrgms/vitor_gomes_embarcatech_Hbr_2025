#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"

void io_init()
{
    stdio_init_all();
}

void led_init()
{
    cyw43_arch_init();
}

void led_set(bool state)
{
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, state);
}