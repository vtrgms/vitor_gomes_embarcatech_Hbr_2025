## Objetivo

Este projeto tem como objetivo principal validar os conhecimentos adquiridos durante a sétima semana da segunda fase do curso **Embarcatech**. Para isto desenvolvi um programa que testa a fórmula de converção do adc para a temperatura interna da **Raspberry Pi Pico** antes de aplicá-la de fato.

## Funcionamento

Este programa checa se a fórmula esta funcionando usando um valor conhecido, caso esta checagem seja positiva, ele define o valor da temperatura como o valor real lido do adc interno.


## Componentes usados

- Raspberry Pi Pico W



## Como reutilizar

É necessário instalar a [extensão oficial da Raspberry Pi Pico](https://github.com/raspberrypi/pico-vscode) no seu VSCode e criar um novo projeto C/C++.
Clone os arquivos presentes neste diretório e execute com sua placa conectada no modo BOOTSEL para compilar e importar o projeto.

>*Atente-se para a versão do SDK, este programa foi feito usando a 2.1.1*   

## Mídia



## Resultados

Comparando o que se esperava com o que obtive no final, posso concluir que o projeto fez aquilo que eu desejava, o valor do adc real só é lido caso a fórmula passe no teste, para este contexto apresentado talvez não faça muito sentido esta checagem, mas acredito que para projetos futuros vai fazer, já que se trata de uma ferramenta muito útil.