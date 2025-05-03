// #include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "drivers/led_embutido.c"

static bool led_state = false;

void init()
{
    io_init();
    led_init();
}

void toggle_led()
{
    led_state = !led_state;
    led_set(led_state);
}