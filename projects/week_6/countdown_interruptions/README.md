## Objetivo

Este projeto tem como objetivo principal validar os conhecimentos adquiridos durante a sexta semana da segunda fase do curso **Embarcatech**. Para isto desenvolvi um programa contador decrescente controlado por interrupções usando uma placa de desenvolvimento [BitDogLab](https://github.com/BitDogLab).


## Funcionamento

Este programa é iniciado quando apertamoos o botão A, assim começando um contador decrescente de 9 a 0, ele registra a quantidade de vezes que o botão B foi pressionado durante a contagem e mostra ao final dela. Se o botão A for apertado novamente durante a contagem ele a reinicia. Todo este processo é mostrado no display.


## Componentes usados

- Raspberry Pi Pico W
- Display OLED SSD1306 → Pinos 14 (GP10 - I2C1 SDA) e 15 (GP11 - I2C1 SCL)
- Botões: A → Pino 7 (GP5) e B → Pino 9 (GP6)



## Como reutilizar

É necessário instalar a [extensão oficial da Raspberry Pi Pico](https://github.com/raspberrypi/pico-vscode) no seu VSCode e criar um novo projeto C/C++.
Clone os arquivos presentes neste diretório e execute com sua placa conectada no modo BOOTSEL para compilar e importar o projeto.

>*Atente-se para a versão do SDK, este programa foi feito usando a 2.1.1*   

## Mídia



## Resultados

Comparando o estipulado pelo professor com o resultado final concluímos que o projeto faz aquilo que desejava, consegue mostrar a contagem decrescente e a quantidade de apertos no display e ser controlado por interrupções.