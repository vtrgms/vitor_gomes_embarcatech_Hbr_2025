#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "ssd1306_i2c.c"

#define I2C_PORT i2c1
#define PIN_SDA 14
#define PIN_SCL 15
#define PIN_BUTTON_A 5
#define PIN_BUTTON_B 6

// Buffer e área de renderização
static uint8_t display_buffer[ssd1306_width * ssd1306_n_pages];
static struct render_area area;

// Controle acionamento botão
static inline bool button_pressed(uint gpio)
{
    if (!gpio_get(gpio))
    {
        while (!gpio_get(gpio))
        {
            sleep_ms(10);
        }
        return true;
    }
    return false;
}

int main()
{
    stdio_init_all();

    // Inicializa I2C
    i2c_init(I2C_PORT, 100000);
    gpio_set_function(PIN_SDA, GPIO_FUNC_I2C);
    gpio_set_function(PIN_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(PIN_SDA);
    gpio_pull_up(PIN_SCL);

    // Configura botões como entrada com pull-up interno
    gpio_init(PIN_BUTTON_A);
    gpio_set_dir(PIN_BUTTON_A, GPIO_IN);
    gpio_pull_up(PIN_BUTTON_A);

    gpio_init(PIN_BUTTON_B);
    gpio_set_dir(PIN_BUTTON_B, GPIO_IN);
    gpio_pull_up(PIN_BUTTON_B);

    ssd1306_init();

    area.start_column = 0;
    area.end_column = ssd1306_width - 1;
    area.start_page = 0;
    area.end_page = ssd1306_n_pages - 1;
    calculate_render_area_buffer_length(&area);

    while (1)
    {
        if (button_pressed(PIN_BUTTON_A))
        {
            int countB = 0;

            for (int digit = 9; digit >= 0; digit--)
            {
                // Espera 1 s, contando B e reagindo a A
                absolute_time_t start = get_absolute_time();
                while (absolute_time_diff_us(start, get_absolute_time()) < 1000000)
                {
                    if (button_pressed(PIN_BUTTON_B))
                    {
                        countB++;
                    }
                    if (button_pressed(PIN_BUTTON_A))
                    {
                        digit = 10; // força sair do for e reiniciar
                        countB = 0;
                        break;
                    }
                    memset(display_buffer, 0, area.buffer_length); // Limpa o buffer

                    char str_counter[16], str_b_counter[16];
                    snprintf(str_b_counter, sizeof(str_b_counter), "Botao B: %d", countB);
                    snprintf(str_counter, sizeof(str_counter), "Contador: %d", digit);

                    ssd1306_draw_string(display_buffer, 12, 40, str_counter);
                    ssd1306_draw_string(display_buffer, 12, 10, str_b_counter);

                    render_on_display(display_buffer, &area);
                }
            }

            memset(display_buffer, 0, area.buffer_length);

            char final[16];
            snprintf(final, sizeof(final), "Apertou %dx", countB);
            ssd1306_draw_string(display_buffer, 0, 28, final);

            render_on_display(display_buffer, &area);
        }
        sleep_ms(10);
    }
    return 0;
}
