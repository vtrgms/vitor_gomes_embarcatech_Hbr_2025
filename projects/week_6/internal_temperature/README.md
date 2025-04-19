## Objetivo

Este projeto tem como objetivo principal validar os conhecimentos adquiridos durante a sexta semana da segunda fase do curso **Embarcatech**. Para isto desenvolvi um programa capaz de mostrar a temperatura interna da **Raspberry Pi Pico W** usando uma placa de desenvolvimento [BitDogLab](https://github.com/BitDogLab).


## Funcionamento

Faz a leitura do ADC interno e converte os valores para graus Celsius (Fahrenheit opcional) e mostra no display.


## Componentes usados

- Raspberry Pi Pico W
- Display OLED SSD1306 → Pinos 14 (GP10 - I2C1 SDA) e 15 (GP11 - I2C1 SCL)



## Como reutilizar

É necessário instalar a [extensão oficial da Raspberry Pi Pico](https://github.com/raspberrypi/pico-vscode) no seu VSCode e criar um novo projeto C/C++.
Clone os arquivos presentes neste diretório e execute com sua placa conectada no modo BOOTSEL para compilar e importar o projeto.

>*Atente-se para a versão do SDK, este programa foi feito usando a 2.1.1*   

## Mídia



## Resultados

Comparando o estipulado pelo professor com o resultado obtido concluímos que o projeto faz aquilo que desejava, consegue mostrar a temperatura interna do módulo usando ADC no display.