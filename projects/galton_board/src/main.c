#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "inc/ssd1306_i2c.c"

// Definições
#define NUM_BINS 6
#define BIN_WIDTH (ssd1306_width / NUM_BINS)
#define MAX_BALLS 10
#define OBSTACLE_ROWS 5
#define OBSTACLE_SPACING 12
#define OBSTACLE_OFFSET 4
#define DEBOUNCE_MS 10
#define BT_A_PIN 5
#define BT_B_PIN 6

// Estrutura da bola
typedef struct {
    int x;
    int y;
    bool active;
} Ball;


// Variáveis
static uint8_t display_buffer[ssd1306_width * ssd1306_n_pages];
static struct render_area area;
Ball balls[MAX_BALLS];
int bins[NUM_BINS] = {0}; // número de bolas por canaleta
float right_prob = 0.5;
int total_balls = 0;
uint32_t last_button_time = 0;
bool show_histogram = false;

// Desenha os retângulos dos obstáculos
void draw_filled_rect(int x, int y, int w, int h) {
    for(int i = x; i < x + w; i++) {
        for(int j = y; j < y + h; j++) {
            ssd1306_set_pixel(display_buffer, i, j, true);
        }
    }
}

// Inicializa o I2C e o display
void init_display() {
    i2c_init(i2c1, 400000);
    gpio_set_function(14, GPIO_FUNC_I2C);
    gpio_set_function(15, GPIO_FUNC_I2C);
    gpio_pull_up(14);
    gpio_pull_up(15);
    ssd1306_init();
}

// Inicializa e configura os botões
void init_buttons() {
    gpio_init(BT_A_PIN);
    gpio_init(BT_B_PIN);
    gpio_set_dir(BT_A_PIN, GPIO_IN);
    gpio_set_dir(BT_B_PIN, GPIO_IN);
    gpio_pull_up(BT_A_PIN);
    gpio_pull_up(BT_B_PIN);
}

// Desenha os obstáculos
void draw_obstacles() {
    for(int row = 0; row < OBSTACLE_ROWS; row++) {
        int y = 10 + row * OBSTACLE_OFFSET * 3;  // Define Distância entre as linhas com obstáculos
        
        // Calcula a posição inicial centralizada com espaçamento ajustado
        int start_x = (ssd1306_width/2) - (row * (OBSTACLE_SPACING/2));
        
        for(int i = 0; i <= row; i++) {
            int x = start_x + i * OBSTACLE_SPACING; // Posiciona obstáculos com espaçamento maior
            
            // Desenha obstáculo (4x4 pixels)
            draw_filled_rect(x, y, 4, 4);
        }
    }
}

// Adiciona as bolas
void add_ball() {
    for(int i = 0; i < MAX_BALLS; i++) {
        if(!balls[i].active) {
            balls[i].x = ssd1306_width/2; // Centralizado em x
            balls[i].y = 0; // No início do display
            balls[i].active = true;
            total_balls++;
            break;
        }
    }
}

// Atualiza posição da bola com base na colisão
void update_balls() {
    for(int i = 0; i < MAX_BALLS; i++) {
        if(balls[i].active) {
            bool collision = false;
            for(int row = 0; row < OBSTACLE_ROWS; row++) {
                int obstacle_y = 10 + row * OBSTACLE_OFFSET * 3;
                
                // Verifica colisão na área do obstáculo
                if(balls[i].y + 2 >= obstacle_y && balls[i].y <= obstacle_y + 4) {
                    int start_x = (ssd1306_width/2) - (row * (OBSTACLE_SPACING/2));
                    
                    // Verifica todos os obstáculos na linha
                    for(int j = 0; j <= row; j++) {
                        int obstacle_x = start_x + j * OBSTACLE_SPACING;
                        
                        if(balls[i].x + 2 >= obstacle_x && 
                           balls[i].x <= obstacle_x + 4) {
                            collision = true;
                            
                            // Movimento proporcional ao espaçamento dos obstáculos
                            if((float)rand()/RAND_MAX < right_prob) {
                                balls[i].x += OBSTACLE_SPACING/2;
                            } else {
                                balls[i].x -= OBSTACLE_SPACING/2;
                            }
                            break;
                        }
                    }
                    if(collision) break;
                }
            }
            
            balls[i].y += 2; // Atualiza a posição y para a bola descer

            // Alimenta o marcador da canaleta correta
            if(balls[i].y >= ssd1306_height - 4) {
                int bin = (balls[i].x + 1) / BIN_WIDTH;
                if(bin >= 0 && bin < NUM_BINS) {
                    bins[bin]++;
                }
                balls[i].active = false;
            }
        }
    }
}

// Desenha a bola
void draw_balls (){
    for(int i = 0; i < MAX_BALLS; i++) {
        if(balls[i].active) {
            draw_filled_rect(balls[i].x, balls[i].y, 2, 2);
        }
    }
}

void draw_histogram() {
    const int max_bar_height = 40;  // Altura máxima em pixels
    const int base_y = ssd1306_height - 10;  // Base do histograma
    
    // Desenha linha de base
    for(int x = 0; x < ssd1306_width; x++) {
        ssd1306_set_pixel(display_buffer, x, base_y, true);
    }

    for(int i = 0; i < NUM_BINS; i++) {
        int height = bins[i];
        
        if(height > max_bar_height) height = max_bar_height; // Limita a altura máxima
        
        int y_pos = base_y - height;
        
        // Desenha a barra vertical
        for(int h = 0; h < height; h++) {
            int x_start = i * BIN_WIDTH + 2;
            for(int w = 0; w < BIN_WIDTH - 4; w++) {
                ssd1306_set_pixel(display_buffer, x_start + w, y_pos + h, true);
            }
        }
        
        // Escreve número abaixo da barra
        char buf[4];
        snprintf(buf, sizeof(buf), "%d", bins[i]);
        ssd1306_draw_string(display_buffer, 
            i * BIN_WIDTH + 2, 
            base_y + 2,
            buf);
    }
}


// Desenha as estásticas de quantidade de bolas e porcentagem da aleatoriedade
void draw_stats() {
    char buf[20];
    snprintf(buf, sizeof(buf), "P:%.0f%%", right_prob*100);
    ssd1306_draw_string(display_buffer, 0, 0, buf);
    
    snprintf(buf, sizeof(buf), "T:%d", total_balls);
    ssd1306_draw_string(display_buffer, ssd1306_width-50, 0, buf);
}

int main() {
    stdio_init_all();
    init_display();
    init_buttons();

    area.start_column = 0;
    area.end_column = ssd1306_width - 1;
    area.start_page = 0;
    area.end_page = ssd1306_n_pages - 1;
    calculate_render_area_buffer_length(&area);

    while(true) {
        update_balls();
        
        uint32_t now = to_ms_since_boot(get_absolute_time());
        bool both_pressed = (!gpio_get(BT_A_PIN) && !gpio_get(BT_B_PIN));
        
        // Verifica botões pressionados juntos
        if(both_pressed && (now - last_button_time) > DEBOUNCE_MS) {
            show_histogram = !show_histogram;  // Alterna estado da tela
            last_button_time = now;
        }

        // Verifica botões individualmente
        else {
            if(!gpio_get(BT_A_PIN) && (now - last_button_time) > DEBOUNCE_MS) {
                add_ball();
                last_button_time = now;
            }
            if(!gpio_get(BT_B_PIN) && (now - last_button_time) > DEBOUNCE_MS) {
                right_prob += 0.2f;
                if(right_prob > 0.7f) right_prob = 0.3f;
                last_button_time = now;
            }
        }

        memset(display_buffer, 0, area.buffer_length);
        
        if(show_histogram) {
            draw_histogram();
        } else {
            draw_obstacles();
        }
        
        draw_stats();     
        draw_balls ();

        render_on_display(display_buffer, &area); // Renderiza no display
        sleep_ms(50);
    }
}