#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "ssd1306_i2c.c"
#include "hardware/adc.h"

#define I2C_PORT i2c1
#define PIN_SDA 14
#define PIN_SCL 15

// Buffer e área de renderização
static uint8_t display_buffer[ssd1306_width * ssd1306_n_pages];
static struct render_area area;

void draw_circle(int x, int y)
{
    // Desenha um ponto 3x3
    for (int dx = -1; dx <= 1; dx++)
    {
        for (int dy = -1; dy <= 1; dy++)
        {
            ssd1306_set_pixel(display_buffer, x + dx, y + dy, true);
        }
    }
}

int main()
{
    stdio_init_all();
    adc_init();
    adc_gpio_init(26); // Eixo Y
    adc_gpio_init(27); // Eixo X

    // Inicializa I2C
    i2c_init(I2C_PORT, 100000);
    gpio_set_function(PIN_SDA, GPIO_FUNC_I2C);
    gpio_set_function(PIN_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(PIN_SDA);
    gpio_pull_up(PIN_SCL);

    // Inicializa display
    ssd1306_init();

    area.start_column = 0;
    area.end_column = ssd1306_width - 1;
    area.start_page = 0;
    area.end_page = ssd1306_n_pages - 1;
    calculate_render_area_buffer_length(&area);

    while (1)
    {
        // Lê valores do joystick
        adc_select_input(1);
        uint adc_x_raw = adc_read();
        adc_select_input(0);
        uint adc_y_raw = adc_read();

        // Converte para coordenadas do display
        int x = (adc_x_raw * (ssd1306_width - 1)) / 4095;
        int y = ((4095 - adc_y_raw) * (ssd1306_height - 1)) / 4095; // Inverte Y

        memset(display_buffer, 0, area.buffer_length);

        draw_circle(x, y);

        render_on_display(display_buffer, &area);

        sleep_ms(16);
    }
}