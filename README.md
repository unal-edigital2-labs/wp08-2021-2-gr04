# W07_Entrega-_final

## Integrantes 

- Giselle Alejandra Mojica Rodríguez 

- Cristian David Valenzuela Cruz

- James Mauricio Daza Obando


## Introducción

El trabajo mostrado en este repositorio presenta el proyecto final de la asignatura Electrónica Digital 2 dictada en la Universidad Nacional de Colombia.

El proyecto consiste en desarrollar un robot capaz de navegar y trazar un laberinto al mismo tiempo que procesa imágenes por color y forma. Para esto se usa una tarjeta de desarrollo Nexys 4 (FPGA) sobre el cual se integra un SoC basado en arquitectura RISC-V de 32 bits.

Se presenta documentación detallada de hardware y firmware, cada módulo descrito integrando así también el mapa de memoria; todo esto integra una descripción completa sobre el proyecto y se espera que pueda ser de utilidad para futuros trabajos
## SoC

![Screenshot](/images/SoCmem.png)

## [ Mapa de Memoria y Periféricos](https://github.com/unal-edigital2-labs/wp08-2021-2-gr04/tree/main/Mapa)

En esta sección se describen los periféricos usados para la elaboración del robot cartógrafo, sus espacios en memoria, el desarrollo tanto en software como en hardware y código asociado a cada uno de estos. Adicionalmente, se pueden encontrar los datasheets de estos componentes para mayor detalle en la carpeta Datasheets.

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

