#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/i2c.h"
#include "ssd1306_i2c.c"

#define I2C_PORT i2c1
#define PIN_SDA 14
#define PIN_SCL 15
char str_temp[8];

// Buffer e área de renderização
static uint8_t display_buffer[ssd1306_width * ssd1306_n_pages];
static struct render_area area;

float read_internal_temperature(const char unit)
{
    const float conversionFactor = 3.3f / (1 << 12);

    float adc = (float)adc_read() * conversionFactor;
    float tempC = 27.0f - (adc - 0.706f) / 0.001721f;

    if (unit == 'C')
    {
        return tempC;
    }
    else if (unit == 'F')
    {
        return tempC * 9 / 5 + 32;
    }

    return -1.0f;
}

int main()
{
    // Inicializa ADC
    adc_init();
    adc_set_temp_sensor_enabled(true);
    adc_select_input(4);

    // Inicializa I2C
    i2c_init(I2C_PORT, 100000);
    gpio_set_function(PIN_SDA, GPIO_FUNC_I2C);
    gpio_set_function(PIN_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(PIN_SDA);
    gpio_pull_up(PIN_SCL);

    // Inicializa Display
    ssd1306_init();
    area.start_column = 0;
    area.end_column = ssd1306_width - 1;
    area.start_page = 0;
    area.end_page = ssd1306_n_pages - 1;
    calculate_render_area_buffer_length(&area);

    while (true)
    {
        memset(display_buffer, 0, area.buffer_length); // Limpa o buffer

        float temperature = read_internal_temperature('C'); // Faz a leitura de temperatura

        snprintf(str_temp, sizeof(str_temp), "%.02f graus", temperature);
        ssd1306_draw_string(display_buffer, 15, 0, "temperatura");
        ssd1306_draw_string(display_buffer, 20, 20, "interna");
        ssd1306_draw_string(display_buffer, 12, 40, str_temp);

        render_on_display(display_buffer, &area); // Printa no display
    }
}
