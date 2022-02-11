# Entrega Final - Digital 2 - Grupo 4

## Integrantes 

- Giselle Alejandra Mojica Rodríguez 

- Cristian David Valenzuela Cruz

- James Mauricio Daza Obando


## Introducción

El trabajo mostrado en este repositorio presenta el proyecto final de la asignatura Electrónica Digital 2 dictada en la Universidad Nacional de Colombia.

El proyecto consiste en desarrollar un robot capaz de navegar y trazar un laberinto al mismo tiempo que procesa imágenes por color y forma. Para esto se usa una tarjeta de desarrollo Nexys 4 (FPGA) sobre el cual se integra un SoC basado en arquitectura RISC-V de 32 bits.

Se presenta documentación detallada de hardware y firmware, cada módulo descrito integrando así también el mapa de memoria; todo esto compone una descripción completa sobre el proyecto y se espera que pueda ser de utilidad para futuros trabajos.

<p align="center">
  <img width="720" height="405" src=/images/escenario.png>
</p>

## SoC Robot Cartógrafo

Se basó en un procesador RISC-V conectado mediante un bus de comunicación Wishbone a los módulos cargados en Verilog; Litex incorpora esto al System on Chip que a su vez mantiene conexión con los diferentes periféricos que conforman el proyecto en físico

![Screenshot](/images/SoCmem.jpg)

## [ Mapa de Memoria y Periféricos](https://github.com/unal-edigital2-labs/wp08-2021-2-gr04/tree/main/Mapa)

En esta sección se describen los periféricos usados para la elaboración del robot cartógrafo, sus espacios en memoria, el desarrollo tanto en software como en hardware y código asociado a cada uno de estos. Adicionalmente, se pueden encontrar los datasheets de estos componentes para mayor detalle y enlaces de interés en la carpeta Datasheets y Links.



<p align="center">
  <img width="250" height="400" src=/images/Memorymap.PNG>
</p>

## [ Firmware ](https://github.com/unal-edigital2-labs/wp08-2021-2-gr04/tree/main/Firmware)

En esta seccion se incluyen los archivos usados para realizar la respectiva integracion de los archivos de hardware a el SOC. Ademas, tambien se incluyen el main.c, la cual se encarga de controlar el robot cartografo desde software, y la respectiva explicacion de funcionamiento para cada una de las funciones usadas.

## Problemas presentados

Desarrollar firmware en C manualmente puede no ser muy bueno si no se tienen buenas prácticas de programación y optimización. Pues se puede llegar a hacer un código que funcione en un IDE pero no al implementarlo por cuestiones de capacidad de almacenamiento

## Pruebas funcionales

<a href="http://www.youtube.com/watch?feature=player_embedded&v=XLlC-236xpk
" target="_blank"><img src="http://img.youtube.com/vi/XLlC-236xpk/0.jpg" 
alt="(http://img.youtube.com/vi/XLlC-236xpk/0.jpg)" width="400"/></a>

