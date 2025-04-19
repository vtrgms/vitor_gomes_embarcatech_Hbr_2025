#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "ssd1306_i2c.h"
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
        printf("Botão apertado");
        sleep_ms(50);
        if (!gpio_get(gpio))
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

            for (int digit = 9; digit >= 0; --digit)
            {
                if (button_pressed(PIN_BUTTON_A))
                { // Se A for pressionado de novo, reinicia o loop
                    printf("Botao apertado dnv");
                    break;
                }

                memset(display_buffer, 0, area.buffer_length); // Limpa o buffer

                char str[8];
                sprintf(str, "%d", digit);
                printf("será que foi?", str);
                ssd1306_draw_string(display_buffer, 60, 28, str);

                render_on_display(display_buffer, &area);

                // Espera 1 s, contando B e reagindo a A
                absolute_time_t start = get_absolute_time();
                while (absolute_time_diff_us(start, get_absolute_time()) < 1000000)
                {
                    if (button_pressed(PIN_BUTTON_B))
                    {
                        countB++;
                        char count[8];
                        sprintf(count, "%d", countB);
                        printf(count);
                    }
                    if (button_pressed(PIN_BUTTON_A))
                    {
                        digit = 10; // força sair do for e reiniciar
                        break;
                    }
                }
            }

            memset(display_buffer, 0, area.buffer_length);
            char summary[5];
            snprintf(summary, sizeof(summary), "%d", countB);
            ssd1306_draw_string(display_buffer, 50, 28, summary);
            render_on_display(display_buffer, &area);
            sleep_ms(2000);
        }
        sleep_ms(10);
    }

    return 0;
}
