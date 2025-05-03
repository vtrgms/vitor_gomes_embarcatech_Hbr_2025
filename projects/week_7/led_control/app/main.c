#include "hal/hal_led.c"

int main()
{
    init();

    while (true)
    {
        toggle_led();
        sleep_ms(1000);
    }
}