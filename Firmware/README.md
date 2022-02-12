
1. [ Cámara. ](#camara)
2. [ Ultrasonido ](#us)
3. [ Infrarojo ](#Infrarojo)
4. [ Movimiento. ](#movimiento)

## Camara
Para realizar la implementacion de camara en software, la cual se encarga del reconocimiento de color y forma, no fue necesario usar un codigo muy complejo en C, debido a que el procesamiento de los datos se realizo por hardware.El codigo consiste de dos partes principales, una se encarga de obtener los datos y la otra de mostrarlo a traves de leds. La primera parte es la siguiente:

``` c
static int recibirForma(void)
{

		unsigned int Forma = 0;

		Forma = VGA_Mapa_Forma_read();

		return Forma;	
		
}

static int recibirColor(void)
{

		unsigned int Color = 0;

		Color = VGA_Mapa_PromedioColor_read();

		return Color;	
		
}
``` 

La siguiente funcion, hace uso de las dos funciones anteriores y muestra cual es la forma asi como el color que capta la camara. 

``` c
static void led_test(void)
{
	unsigned int form = 0;
	unsigned int col = 0;
	printf("Test del los leds... se interrumpe con el botton 1\n");
	while(!(buttons_in_read()&1)) {

		form = recibirForma();
		col = recibirColor()*8;
		leds_out_write(form+col);
	}
	
}
``` 
<a name="camara"></a>



## Infrarojo 

En la primera parte definimos la variable entrada la cual almacenara los valores leido a traves del driver infrarojo, asi como tambien definimos la variable salida a la cual se asignaran valores en concordancia con la programacion de la funcion de movimiento.

``` c
static int infrarrojo(void){
   
	unsigned int entrada = infrarrojo_infras2_read();
	unsigned int salida=3;

``` 
En la segunda parte, primero deifimos un if con todos los posible valores que puede tomar entrada cuando el infrorojo de la mitad detecta negro.
``` c
	if((entrada == 0x4)||(entrada == 0x5)||(entrada == 0x6)||(entrada == 0x7)||(entrada == 0xC)||(entrada == 0xD)||(entrada == 0xE)||(entrada == 0xF)||(entrada == 0x14)||(entrada == 0x15)||(entrada == 0x16)||(entrada == 0x17)||(entrada == 0x1C)||(entrada == 0x1D)||(entrada == 0x1E)||(entrada == 0x1F)){
``` 
Para realizar un giro hacia la derecha, se consideran 3 posibles valores para entrada 00111,01111,10111. Los cuales nos quieren decir que al menos uno los infrarojos localizados en la derecha ha detectado un movimiento.

``` c
		if ((entrada == 0x7)||(entrada == 0xF)||(entrada == 0x17))

		{
			salida = 2;
			printf("Gire a la derecha: %x \n",entrada);
		}
		
```

Para realizar un giro hacia la derecha, se consideran 3 posibles valores para entrada 11100,11110,11101. Los cuales nos quieren decir que al menos uno los infrarojos localizados en la derecha ha detectado un movimiento.

``` c
		else if ((entrada == 0x1C)||(entrada == 0x1D)||(entrada == 0x1E))
		{
			salida = 1;
			printf("Gire a la izquierda: %x \n",entrada);
		}
```
Para el caso de parada, se considera solo un valor para entrada 11111. Asi tambien para cualquier otro caso que no se de los mencionados para parada o girar, se avanzara.

``` c
		else if ((entrada == 0x1F))
		{
			salida = 3;
			printf("Pare: %x \n",entrada);
		}
		else
		{
			salida = 0;
			printf("Siga: %x \n",entrada);
		}
		
			
	}
	else if (entrada == 0x00)
	{
		salida = 3;
		printf("Intersección: %x \n",entrada);
	}
	
	else
	{
		salida = 0;
			printf("Está quieto: %x \n",entrada);
	}
	
	
	return salida;

}

``` 
<a name="Infrarojo"></a>
## Movimiento




<a name="movimiento"></a>
## Movimiento
El movimiento del robot se programo en c, tal y como se muestra a continuacion. Este se basa unicamente en el infrarojo para realizar su navegacion.

``` c
static void w_test(void){
	unsigned int state = 3;
	unsigned int cam = 0;
	
	while(!(buttons_in_read()&1)){

		state = infrarrojo();
		ruedas_move_write(state);
		delay_ms(12);
		ruedas_move_write(3);
		servo_test(state);
		cam = led_test();
		leds_out_write(cam);
		delay_ms(100);		
	}

``` 
}


<p align="center">
  <img width="150" height="75" src=/images/salidamapa.PNG>
</p>


