# W07_Entrega-_final

## Integrantes 

- Giselle Alejandra Mojica Rodríguez 

- Cristian David Valenzuela Cruz

- James Mauricio Daza Obando


## Introducción

El repositorio mostrado a continuacion contiene la entrega final del proyecto de la materia Electrónica Digital II dictada en la Universidad Nacional de Colombia. La propuesta del proyecto consiste en un robot cartográfico que recorre un laberinto siendo capaz de identificar el color y la forma figuras que se hayan en el laberinto. Ademas, el robot realiza un mapeo la distribución del laberinto mientras lo recorre. Se utilizan los lenguajes de programación: Verilog, Python y C. 

Cada módulo se encuentra explicado en la documentación dentro de la carpeta module, mientras que en este documento se explicará la integración y el funcionamiento conjunto de los periféricos.

## SoC

![Screenshot](/images/SoCmem.png)

## [ Mapa de Memoria ](https://github.com/unal-edigital2/w07_entrega-_final-grupo11/tree/main/module)

En la presente sección se encuentran los diferentes perífericos que se usaron para la elaboración del robot cartógrafo, asi como también los respectivos espacios en memoria que fueron utilizados. Asi mismo, se incluye el hardware desarrollado para cada uno de estos perifericos y su respectiva implementacíon.

## [ Firmware ](https://github.com/unal-edigital2/w07_entrega-_final-grupo11/tree/main/firmware)

En esta seccion se incluyen los archivos usados para realizar la respectiva integracion de los archivos de hardware a el SOC. Ademas, tambien se incluyen el main.c, la cual se encarga de controlar el robot cartografo desde software, y la respectiva explicacion de funcionamiento para cada una de las funciones usadas.

## Construcción

Para la construcción del carrito se utilizaron los siguientes materiales:
1. Placa de MDF de 15cmx20cm
2. 2 motorreductores junto con sus llantas
3. 1 servomotor
4. 1 ultrasonido HC-SR04
5. 5 sensores infrarrojos con su driver
6. 1 puente H L298N
7. 1 Protoboard


Bosquejo del carrito:

![Screenshot](/images/plano.PNG)


![Screenshot](/images/Carrito.jpeg)

Para el laberinto se estableció un patron con cinta aislante, en cada linea perpendicular al camino principal el carrito se detiene y mide las distancias a su alrededor. Las paredes del laberinto tienen una separación aproximada de 40cm, el grosor de la cinta aislante es de aproximadamente 4cm, y las lineas que cortan el camino son de aproximadamente 25cm de largo:

![Screenshot](/images/Laberinto.PNG)

![Screenshot](/images/lab.jpeg)

## Pruebas funcionales

<a href="http://www.youtube.com/watch?feature=player_embedded&v=XLlC-236xpk
" target="_blank"><img src="http://img.youtube.com/vi/XLlC-236xpk/0.jpg" 
alt="(http://img.youtube.com/vi/XLlC-236xpk/0.jpg)" width="400"/></a>

