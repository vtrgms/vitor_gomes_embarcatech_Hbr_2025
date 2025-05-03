#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "math.h"
#include "inc/converter.h"

const uint adc = 4;
float temperature;

bool check_temperature(float expected, float actual, float tolerance)
{
    if (fabs(expected - actual) <= tolerance)
    {
        return true;
    }
    else
    {
        return false;
    }
}

int main(void)
{
    stdio_init_all();
    adc_init();
    adc_set_temp_sensor_enabled(true);
    adc_select_input(adc);
    uint16_t adc_test_val = (uint16_t)((0.706f / 3.3f) * 4095.0f);

    while (1)
    {
        temperature = adc_to_celsius(adc_test_val);
        if (check_temperature(27.0, temperature, 0.5))
        {
            printf("Teste aprovado!\t");
            uint16_t adc_temp = adc_read();
            temperature = adc_to_celsius(adc_temp);
            printf("Temperatura: %.2f\n", temperature);
        }
        else
        {
            printf("Teste não aprovado!\n");
        }
        sleep_ms(500);
    }
}