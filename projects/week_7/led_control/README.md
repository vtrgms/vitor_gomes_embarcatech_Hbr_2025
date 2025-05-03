## Objetivo

Este projeto tem como objetivo principal validar os conhecimentos adquiridos durante a sétima semana da segunda fase do curso **Embarcatech**. Para isto desenvolvi um programa capaz de controlar o LED embutido placa **Raspberry Pi Pico** usando estruturação de código para arquitetura modular.


## Funcionamento

Este programa alterna o estado do LED interno a cada segundo, para isso usa uma esruturação modular ded código, com drivers lidando com a parte mais próxima do hardware, hal interfaceando drivers com aplicação, e a aplicação em si que se torna mais modular.


## Componentes usados

- Raspberry Pi Pico W



## Como reutilizar

É necessário instalar a [extensão oficial da Raspberry Pi Pico](https://github.com/raspberrypi/pico-vscode) no seu VSCode e criar um novo projeto C/C++.
Clone os arquivos presentes neste diretório e execute com sua placa conectada no modo BOOTSEL para compilar e importar o projeto.

>*Atente-se para a versão do SDK, este programa foi feito usando a 2.1.1*   

## Mídia



## Resultados

Comparando o estipulado pelo professor com o resultado final concluímos que o projeto faz aquilo que desejava, consegui entender este tipo de estruturação e aplicar de forma simples e eficaz.