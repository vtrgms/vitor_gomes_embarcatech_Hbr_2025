## Objetivo

Este projeto tem como objetivo principal validar os conhecimentos adquiridos durante a primeira fase do curso **Embarcatech**. Para isto desenvolvi um programa que é capaz de obter dados climáticos através do módulo Wif-fi presente na placa **Raspberry Pi Pico W** e mostra-los usando os componentes visuais presentes na placa de desenvolvimento [BitDogLab](https://github.com/BitDogLab).



## Funcionamento

De início o programa tenta infinitamente estabelecer uma conexão **Wi-fi** com as credenciais definidas no código fonte e inicializa as GPIOS e interfaces necessárias para o restante das funcionalidades. Em seguida obtém a **geolocalização** do usuário baseada na [API de IP](https://ip-api.com), utiliza estes dados para obter o **clima** na região atual usando a [API OpenWeatherMap](https://openweathermap.org/api) e manipula LED`s e um display que mostram como está o tempo.



## Componentes usados

Foram utilizados como respostas visuais:
- Matriz 5x5 de LED`s WS2812B → Pino 10 (GP7)
- LED RGB → Vermelho no pino 17 (GP13), verde no pino 15 (GP11) e azul no pino 16 (GP12)
- Display OLED SSD1306 → Pinos 31 (GP26 - I2C1 SDA) e 32 (GP27 - I2C1 SCL)



## Como reutilizar

É necessário instalar a [extensão oficial da Raspberry Pi Pico](https://github.com/raspberrypi/pico-vscode) no seu VSCode e criar um novo projeto C/C++.
Clone os arquivos presentes neste diretório e execute com sua placa conectada no modo BOOTSEL para compilar e importar o projeto.

>*Atente-se para a versão do SDK, este programa foi feito usando a 2.1.1*   
>*Não esqueça de alterar as credenciais do Wi-fi no [código principal](https://github.com/vtrgms/vitor_gomes_embarcatech_Hbr_2025/blob/main/projects/weather_agent/src/main.c), assim como a key da API OpenWeatherMap*



## Mídia

Situação | BitDogLab
:-------: | -------:
Nublado e agradável | 
Chuvoso e frio | 
Limpo e calor | 



## Resultados

Comparando o esperado com o observado concluímos que o projeto faz aquilo que desejava, consegue obter de maneira dinâmica dados climáticos do lugar onde a placa está e mostra-los de forma intuitiva ao usuário.