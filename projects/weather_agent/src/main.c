// Bibliotecas
#include "pico/cyw43_arch.h"
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/pio.h"
#include "lwip/dns.h"
#include "lwip/tcp.h"
#include "ssd1306_font.h"
#include "ws2812.pio.h"

// Definições
#define WIFI_SSID "Fernando 2.4G"
#define WIFI_PASS "orse27187"
#define API_KEY "d1ff7fe5ae43dbd411cd88d8ddff7cca"
#define GEO_SERVER "ip-api.com"
#define GEO_PATH "/json"
#define WEATHER_SERVER "api.openweathermap.org"
#define SSD1306_I2C_ADDR 0x3C
#define LED_PIN 7
#define LED_COUNT 25
#define LED_BRILHO 20
#define RED_LED_PIN 13
#define BLUE_LED_PIN 12
#define GREEN_LED_PIN 11
#define SSD1306_HEIGHT 64
#define SSD1306_WIDTH 128
#define SSD1306_I2C_ADDR 0x3C
#define SSD1306_PAGE_HEIGHT 8
#define SSD1306_NUM_PAGES (SSD1306_HEIGHT / SSD1306_PAGE_HEIGHT)
#define SSD1306_BUF_LEN (SSD1306_NUM_PAGES * SSD1306_WIDTH)

//  Estruturas
typedef struct
{
    struct tcp_pcb *pcb;
    char buffer[2048];
    int len;
    char request[512];
    void (*process_response)(const char *resp);
} connection_t;

struct render_area
{
    uint8_t start_col;
    uint8_t end_col;
    uint8_t start_page;
    uint8_t end_page;
    int buflen;
};

typedef struct
{
    uint8_t R, G, B;
} npLED_t;

// Variáveis
static float lat = 0.0, lon = 0.0;
static bool geo_obtained = false;
static npLED_t leds[LED_COUNT];
static uint8_t oled_buf[128 * 8];
static PIO np_pio;
static uint sm;
static absolute_time_t ultimo_tempo;
static uint8_t onda_pos = 0;
static char prev[64] = {0};
const char *prev_display;
static int temp;
static int umidade;
struct render_area frame_area = {0, 127, 0, 7, 1024};

// Funcão para inicializar o LED RGB
void RGB_LEB_init()
{
    gpio_init(RED_LED_PIN);
    gpio_init(GREEN_LED_PIN);
    gpio_init(BLUE_LED_PIN);
    gpio_set_dir(RED_LED_PIN, GPIO_OUT);
    gpio_set_dir(GREEN_LED_PIN, GPIO_OUT);
    gpio_set_dir(BLUE_LED_PIN, GPIO_OUT);

    // Inicialmente, desligar o LED RGB
    gpio_put(RED_LED_PIN, 0);
    gpio_put(GREEN_LED_PIN, 0);
    gpio_put(BLUE_LED_PIN, 0);
}

// Funções do display
uint8_t buf[SSD1306_BUF_LEN];

void SSD1306_send_cmd(uint8_t cmd)
{
    uint8_t buf[2] = {0x80, cmd};
    i2c_write_blocking(i2c_default, SSD1306_I2C_ADDR, buf, 2, false);
}

void SSD1306_send_cmd_list(uint8_t *cmds, int num)
{
    for (int i = 0; i < num; i++)
    {
        SSD1306_send_cmd(cmds[i]);
    }
}

void SSD1306_send_buf(uint8_t *buf, int buflen)
{
    uint8_t *temp_buf = malloc(buflen + 1);
    temp_buf[0] = 0x40; // Co = 0, D/C = 1
    memcpy(temp_buf + 1, buf, buflen);
    i2c_write_blocking(i2c_default, SSD1306_I2C_ADDR, temp_buf, buflen + 1, false);
    free(temp_buf);
}

void SSD1306_init()
{
    uint8_t cmds[] = {
        0xAE, 0xD5, 0x80, 0xA8, 0x3F, 0xD3, 0x00, 0x40,
        0x8D, 0x14, 0x20, 0x00, 0xA1, 0xC8, 0xDA, 0x12,
        0x81, 0xCF, 0xD9, 0xF1, 0xDB, 0x40, 0xA4, 0xA6, 0xAF};
    SSD1306_send_cmd_list(cmds, sizeof(cmds) / sizeof(cmds[0]));
}

void calc_render_area_buflen(struct render_area *area)
{
    area->buflen = (area->end_col - area->start_col + 1) *
                   (area->end_page - area->start_page + 1);
}

void render(uint8_t *buf, struct render_area *area)
{
    uint8_t cmds[] = {
        0x21, area->start_col, area->end_col,
        0x22, area->start_page, area->end_page};
    SSD1306_send_cmd_list(cmds, sizeof(cmds) / sizeof(cmds[0]));
    SSD1306_send_buf(buf, area->buflen);
}

int GetFontIndex(uint8_t ch)
{
    if (ch >= 'A' && ch <= 'Z')
        return ch - 'A' + 1;
    if (ch >= 'a' && ch <= 'z')
        return ch - 'a' + 1;
    if (ch >= '0' && ch <= '9')
        return ch - '0' + 27;
    if (ch == ' ')
        return 0;
    if (ch == ':')
        return 37;
    if (ch == '%')
        return 38;
    if (ch == 0xE9)
        return 39; // 'é' em Latin-1
    return 0;
}

void WriteChar(uint8_t *buf, int x, int y, char c)
{
    if (x < 0 || x >= SSD1306_WIDTH || y < 0 || y >= SSD1306_HEIGHT)
        return;

    int font_index = GetFontIndex(c);
    int page = y / 8;
    for (int i = 0; i < 8; i++)
    {
        if (font_index >= 0 && font_index < 54)
        {
            buf[page * SSD1306_WIDTH + x + i] = font[font_index * 8 + i];
        }
    }
}

void WriteString(uint8_t *buf, int x, int y, char *str)
{
    while (*str)
    {
        WriteChar(buf, x, y, *str++);
        x += 8;
    }
}

void DrawCenteredText(uint8_t *buf, int y, char *text)
{
    int text_width = strlen(text) * 8;
    int x = (SSD1306_WIDTH - text_width) / 2;
    WriteString(buf, x < 0 ? 0 : x, y, text);
}

void atualizar_display()
{
    memset(buf, 0, SSD1306_BUF_LEN);

    DrawCenteredText(buf, 0, "Agente");
    DrawCenteredText(buf, 8, "Meteorologico");

    char str_temp[20], str_umid[20], str_prev[20];
    snprintf(str_temp, sizeof(str_temp), "Temp. atual %dC", temp);
    snprintf(str_umid, sizeof(str_umid), "Umidade %d% %", umidade);
    snprintf(str_prev, sizeof(str_prev), "Previsao %s", prev_display);

    DrawCenteredText(buf, 24, str_temp);
    DrawCenteredText(buf, 32, str_umid);
    DrawCenteredText(buf, 40, str_prev);

    render(buf, &frame_area);
}

// Funções dos LEDs
void npSetLED(uint index, uint8_t r, uint8_t g, uint8_t b)
{
    leds[index].R = r;
    leds[index].G = g;
    leds[index].B = b;
}

void npClear()
{
    for (uint i = 0; i < LED_COUNT; i++)
        npSetLED(i, 0, 0, 0);
}

static inline void put_pixel(uint32_t pixel_grb)
{
    pio_sm_put_blocking(np_pio, sm, pixel_grb << 8u);
}

void npWrite()
{
    for (uint i = 0; i < LED_COUNT; i++)
    {
        put_pixel(((uint32_t)(leds[i].R) << 8) |
                  ((uint32_t)(leds[i].G) << 16) |
                  (uint32_t)(leds[i].B));
    }
    sleep_us(100);
}

void npInit()
{
    uint offset = pio_add_program(np_pio, &ws2812_program);
    ws2812_program_init(np_pio, sm, offset, LED_PIN, 800000, false);
    npClear();
}

// Funções de desenho na matriz
void desenhar_sol()
{
    for (uint8_t col = 0; col < 5; col++)
    {
        if (!(col == 4 || col == 3))
        {
            npSetLED(4 * 5 + col, LED_BRILHO, LED_BRILHO, 0);
        }
    }
    npSetLED(3 * 5 + 3, LED_BRILHO, LED_BRILHO, 0);
    npSetLED(3 * 5 + 4, LED_BRILHO, LED_BRILHO, 0);
    npSetLED(2 * 5 + 0, LED_BRILHO, LED_BRILHO, 0);
}

void desenhar_nuvem()
{
    for (uint8_t col = 0; col < 5; col++)
    {
        npSetLED(3 * 5 + col, LED_BRILHO, LED_BRILHO, LED_BRILHO);
        if (!(col == 0 || col == 4))
        {
            npSetLED(4 * 5 + col, LED_BRILHO, LED_BRILHO, LED_BRILHO);
        }
    }
}

void desenhar_chuva()
{
    if (absolute_time_diff_us(ultimo_tempo, get_absolute_time()) > 350000)
    {
        ultimo_tempo = get_absolute_time();
        npClear();
        desenhar_nuvem();

        for (uint8_t i = 0; i < 4; i++)
        {
            uint8_t pos = (onda_pos + i) % 5;
            uint8_t intensidade = LED_BRILHO - (i * 25);

            for (uint8_t j = 0; j < 3; j++)
            {
                uint8_t linha = 2 - j;
                uint8_t index = linha * 5 + (4 - pos);
                if (intensidade > 5)
                {
                    npSetLED(index, 0, 0, intensidade);
                }
            }
        }
        npWrite();
        onda_pos = (onda_pos + 1) % 5;
    }
}

// Configurações Wi-fi
// Callback genérico de recepção: acumula os dados e, ao final, chama a função de processamento
static err_t tcp_recv_cb(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err)
{
    connection_t *conn = (connection_t *)arg;
    if (p == NULL)
    { // fim da resposta
        if (conn->process_response)
        {
            conn->process_response(conn->buffer);
        }
        tcp_close(tpcb);
        free(conn);
        return ERR_OK;
    }
    if (conn->len + p->tot_len < sizeof(conn->buffer))
    {
        pbuf_copy_partial(p, conn->buffer + conn->len, p->tot_len, 0);
        conn->len += p->tot_len;
    }
    tcp_recved(tpcb, p->tot_len);
    pbuf_free(p);
    return ERR_OK;
}

// Callback de erro simples
static void tcp_err_cb(void *arg, err_t err)
{
    connection_t *conn = (connection_t *)arg;
    printf("TCP error: %d\n", err);
    free(conn);
}

// Callback do DNS – chamado quando a resolução for concluída
static void dns_found_cb(const char *name, const ip_addr_t *ipaddr, void *arg)
{
    connection_t *conn = (connection_t *)arg;
    if (ipaddr == NULL)
    {
        printf("Falha na resolução DNS para %s\n", name);
        free(conn);
        return;
    }
    printf("DNS resolvido: %s -> %s\n", name, ipaddr_ntoa(ipaddr));
    struct tcp_pcb *pcb = tcp_new();
    if (!pcb)
    {
        printf("Erro ao criar PCB\n");
        free(conn);
        return;
    }
    conn->pcb = pcb;
    tcp_arg(pcb, conn);
    tcp_err(pcb, tcp_err_cb);
    tcp_recv(pcb, tcp_recv_cb);
    if (tcp_connect(pcb, ipaddr, 80, NULL) != ERR_OK)
    {
        printf("Erro na conexão com %s\n", name);
        free(conn);
        return;
    }
    tcp_write(pcb, conn->request, strlen(conn->request), TCP_WRITE_FLAG_COPY);
}

// Função para conectar a um servidor e enviar um GET
static void connect_and_get(const char *server, uint16_t port, const char *request, void (*process_response)(const char *))
{
    connection_t *conn = calloc(1, sizeof(connection_t));
    if (!conn)
    {
        printf("Erro de alocação\n");
        return;
    }
    conn->process_response = process_response;
    conn->len = 0;
    strncpy(conn->request, request, sizeof(conn->request) - 1);

    ip_addr_t ip;
    err_t err_dns = dns_gethostbyname(server, &ip, dns_found_cb, conn);
    if (err_dns == ERR_OK)
    {
        dns_found_cb(server, &ip, conn);
    }
    else if (err_dns != ERR_INPROGRESS)
    {
        printf("DNS error: %d\n", err_dns);
        free(conn);
    }
}

// Processa a resposta da API de geolocalização e obtem lat/lon
static void process_geo(const char *resp)
{
    char *p = strstr(resp, "\"lat\":");
    if (p)
    {
        lat = atof(p + 6);
    }
    p = strstr(resp, "\"lon\":");
    if (p)
    {
        lon = atof(p + 6);
    }
    if (lat != 0.0 && lon != 0.0)
    {
        geo_obtained = true;
        printf("Geolocalização: lat = %.4f, lon = %.4f\n", lat, lon);
    }
    else
    {
        printf("Falha ao obter geolocalização\n");
    }
}

// Processa a resposta do OpenWeatherMap: obtem temperatura, umidade e previsão do tempo
static void process_weather(const char *resp)
{
    // Obtem a temperatura
    char *p = strstr(resp, "\"temp\":");
    if (p)
    {
        temp = atoi(p + 7);
        printf("Temperatura: %d °C\n", temp);
    }
    else
    {
        printf("Falha ao obter dados de temperatura\n");
    }

    // Extrair a umidade
    char *h = strstr(resp, "\"humidity\":");
    if (h)
    {
        umidade = atoi(h + 11);
        printf("Umidade: %d%%\n", umidade);
    }
    else
    {
        printf("Falha ao obter dados de umidade\n");
    }

    // Extrair a descrição da previsão (primeira ocorrência na seção "weather")
    char *d = strstr(resp, "\"description\":");
    if (d)
    {
        if (sscanf(d, "\"description\": \"%63[^\"]\"", prev) == 1)
        {
            printf("Previsão: %s\n", prev);
        }
        else
        {
            printf("Falha ao obter a previsão\n");
        }
    }
    else
    {
        printf("Falha ao obter a descrição da previsão\n");
    }
}

// Função principal
int main()
{
    stdio_init_all();

    // Inicializa hardware
    i2c_init(i2c_default, 400000);
    gpio_set_function(PICO_DEFAULT_I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C);
    SSD1306_init();
    RGB_LEB_init();

    // Inicializa o Wi-Fi
    if (cyw43_arch_init())
    {
        printf("Erro ao inicializar Wi-Fi\n");
        return 1;
    }
    cyw43_arch_enable_sta_mode();

    // Tenta conectar ao Wi-Fi repetidamente até obter sucesso
    printf("Conectando ao Wi-Fi...\n");
    while (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASS, CYW43_AUTH_WPA2_AES_PSK, 10000))
    {
        printf("Falha ao conectar ao Wi-Fi. Tentando novamente...\n");
        sleep_ms(5000);
    }
    printf("Wi-Fi conectado!\n");

    // Obtem geolocalização
    char geo_req[128];
    snprintf(geo_req, sizeof(geo_req),
             "GET %s HTTP/1.1\r\nHost: %s\r\nConnection: close\r\n\r\n",
             GEO_PATH, GEO_SERVER);
    connect_and_get(GEO_SERVER, 80, geo_req, process_geo);

    sleep_ms(5000);

    np_pio = pio0;
    sm = pio_claim_unused_sm(np_pio, true);
    npInit();

    uint8_t counter = 0;
    while (true)
    {
        cyw43_arch_poll();
        sleep_ms(100);
        if (counter >= 10)
        {
            counter = 0;
            if (geo_obtained)
            {
                char weather_req[256];
                snprintf(weather_req, sizeof(weather_req),
                         "GET /data/2.5/weather?lat=%.4f&lon=%.4f&units=metric&appid=%s HTTP/1.1\r\n"
                         "Host: %s\r\nConnection: close\r\n\r\n",
                         lat, lon, API_KEY, WEATHER_SERVER);
                connect_and_get(WEATHER_SERVER, 80, weather_req, process_weather);
            }
            else
            {
                printf("Geolocalização não obtida.\n");
            }
        }

        // Atualiza LEDs
        if (strstr(prev, "rain"))
        {
            npClear();
            desenhar_chuva();
            npWrite();
            prev_display = "chuva";
        }
        else if (strstr(prev, "cloud"))
        {
            npClear();
            desenhar_nuvem();
            npWrite();
            prev_display = "nuvem";
        }
        else if (strstr(prev, "clear"))
        {
            npClear();
            desenhar_sol();
            npWrite();
            prev_display = "limpo";
        }

        atualizar_display();

        // Controle do LED RGB com base na temperatura
        if (20 < temp < 28)
        {
            gpio_put(GREEN_LED_PIN, 1);
            gpio_put(BLUE_LED_PIN, 0);
            gpio_put(RED_LED_PIN, 0);
        }
        else if (temp < 20)
        {
            gpio_put(GREEN_LED_PIN, 0);
            gpio_put(BLUE_LED_PIN, 1);
            gpio_put(RED_LED_PIN, 0);
        }
        else
        {

            gpio_put(GREEN_LED_PIN, 0);
            gpio_put(BLUE_LED_PIN, 0);
            gpio_put(RED_LED_PIN, 1);
        }
        counter += 1;
        sleep_ms(100);
    }
}