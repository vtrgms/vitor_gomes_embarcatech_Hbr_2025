## Objetivo

Este projeto tem como objetivo principal validar os conhecimentos adquiridos durante a sexta semana da segunda fase do curso **Embarcatech**. Para isto desenvolvi um programa capaz de visualizar a direção de um **joystick** usando uma placa de desenvolvimento [BitDogLab](https://github.com/BitDogLab).


## Funcionamento

Este programa faz a leitura de dois ADC's (eixos X e Y) de um joystick e mostra os valores correspondentes da posição em um display.


## Componentes usados

- Raspberry Pi Pico W
- Display OLED SSD1306 → Pinos 14 (GP10 - I2C1 SDA) e 15 (GP11 - I2C1 SCL)
- Joystick → Pinos 31 (GP26 - ADC0) e 32 (GP27 - ADC1)



## Como reutilizar

É necessário instalar a [extensão oficial da Raspberry Pi Pico](https://github.com/raspberrypi/pico-vscode) no seu VSCode e criar um novo projeto C/C++.
Clone os arquivos presentes neste diretório e execute com sua placa conectada no modo BOOTSEL para compilar e importar o projeto.

>*Atente-se para a versão do SDK, este programa foi feito usando a 2.1.1*   

## Mídia



## Resultados

Comparando o estipulado pelo professor com o resultado final concluímos que o projeto faz aquilo que desejava, consegue mostrar a posição atual do joystick usando ADC e o display.