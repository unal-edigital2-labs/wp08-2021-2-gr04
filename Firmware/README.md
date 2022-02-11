
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
En la segunda parte, primero deifimos un if con todos los posible valores que puede tomar entrada, los cuales van de 0 a 31 en decimales, en caso de que entrada no este entre estos valores, se considerara que ocurrio un error y la variable salida mantendra su valor 3, que significa no hacer movimiento.
``` c
	if((entrada == 0x4)||(entrada == 0x5)||(entrada == 0x6)||(entrada == 0x7)||(entrada == 0xC)||(entrada == 0xD)||(entrada == 0xE)||(entrada == 0xF)||(entrada == 0x14)||(entrada == 0x15)||(entrada == 0x16)||(entrada == 0x17)||(entrada == 0x1C)||(entrada == 0x1D)||(entrada == 0x1E)||(entrada == 0x1F)){
``` 
Para realizar un giro hacia la izquierda, se consideran 3 posibles valores para entrada 00111,01111,10111. Los cuales nos quieren decir que al menos uno los infrarojos localizados en la izquierda ha detectado un movimiento.

``` c
		if ((entrada == 0x7)||(entrada == 0xF)||(entrada == 0x17))

		{
			salida = 2;
			printf("Gire a la izquierda: %x \n",entrada);
		}
		
```

Para realizar un giro hacia la derecha, se consideran 3 posibles valores para entrada 11100,11110,11101. Los cuales nos quieren decir que al menos uno los infrarojos localizados en la derecha ha detectado un movimiento.

``` c
		else if ((entrada == 0x1C)||(entrada == 0x1D)||(entrada == 0x1E))
		{
			salida = 1;
			printf("Gire a la derecha: %x \n",entrada);
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
	else
	{
		salida = 3;
			printf("Está quieto: %x \n",entrada);
	}
	
	
	
	return salida;

}

``` 
<a name="Infrarojo"></a>
## Radar

Para la implementación del radar en C, se realizan 2 funciones.Por una parte, una para el ultrasonido que unicamente  realizara una medición de distancia. Por otra parte, la segunda funcion hace uso de la funcion de ultrasonido mencionada anteriormente y con la ayuda del servomotor se realizan tres mediciones, una para cada dirección. Para la primera:


``` c
static int ultraSound_test(void)
{
	unsigned int d = 0;	
	bool done = false;
        while(!(buttons_in_read()&1)){
		ultrasonido_init_write(1);
		delay_ms(2);
		while(!done){
			delay_ms(2);
			done = ultrasonido_done_read();
			delay_ms(2);
			if(done == 1){
				d=ultrasonido_dist_read();
				printf("La distancia es %x \n",d);
			}
		}
		
		//ultrasonido_init_write(0);
		}
		return d;
	
	
}
```

Para la implementación con el servomotor:

``` c
static int * US(void){
	int state = 0;
	static int d[3];
	while(true){
		switch(state){
			case 0: 
				// Se mueve el servomotor a 0° (mirando a la derecha)
				PWMUS_cntrl_pos_write(0);
				//Se da tiempo a que el servotor se posicione
				delay_ms(1000);
				//Se llama a la función ultraSound_test() y se guarda en la primera posición del array
				d[0] = ultraSound_test();
				state = 1;
				break;
			case 1: 
				// Se repite el proceso pero a 90°
				PWMUS_cntrl_pos_write(1);
				delay_ms(1000);
				d[1] = ultraSound_test();
				state = 2;
				break;
			case 2: 
				PWMUS_cntrl_pos_write(2);
				delay_ms(1000);
				d[2] = ultraSound_test();
				state = 3;
				break;
			case 3: 
				// Se repite el proceso a 180°
				PWMUS_cntrl_pos_write(0);
				delay_ms(1000);
				// Se imprimen las distancias por el serial y por el bluetooth
				char distances[3];
				printf("----------\n");
				bluetooth_write("----------\n");
				for(int i = 2; i>=0; i--){
					printf("%d", d[i]);
					sprintf(distances, "%d", d[i]);
					bluetooth_write(distances);
					if(i>0){
						printf(" - ");
						bluetooth_write(" - ");
					}
				}
				bluetooth_write("\n");
				printf("\n");
				// Se retorna el arreglo con las 3 mediciones
				return d;
				break; 
		}
	}
}
```



<a name="movimiento"></a>
## Movimiento

El movimiento en el robot incluye las ruedas, los infrarrojos, el ultrasonido y el mp3. La sección de dirección en el main.c describe el funcionamiento en conjunto de estos periféricos. 

Para comenzar el código le indica a las ruedas que avancen hacia adelante, se inicializan variables como rotate y orientation, y se inicializa la matriz del mapa del laberinto.

``` c

static void direction(void){
	wheels_cntrl_state_write(0);
	int orientation = 0;
	int posV = 0;
    	int posH = 0;
	int map[10][10];

	//Inicialización de la matriz del laberinto
	for(int i=0;i<10;i++){
        for(int k=0;k<10;k++){  
            map[i][k] = 0;
        }    
    }

```
Luego, se tienen las variables que leen los registros del infrarrojo para procesar qué es lo que "observa" el robot; además se incluye una función para resetear las respectivas variables en las que se incluyen la rotación, la orientación y la posición.

``` c
char *tempMap = "A";
while(true){
	bool L = IR_cntrl_L_read();
	bool LC = IR_cntrl_LC_read();
	bool C = IR_cntrl_C_read();
	bool RC = IR_cntrl_RC_read();
	bool R = IR_cntrl_R_read();

	if(buttons_in_read()&2){
		orientation = 0;
		posV = 1;
		posH = 0;
		for(int i=0;i<10;i++){
			for(int k=0;k<10;k++){
				map[i][k] = 0;
				printf("%i",  map[i][k]);
				sprintf(tempMap, "%d", map[i][k]);
				bluetooth_write(tempMap);
			}
			printf("\n");
			bluetooth_write("\n");
		}
		wheels_cntrl_state_write(0);
	}		
```

En los estados de rotate se define el movimiento del robot basándose en la lectura de los registros de cada infrarrojo. Por ejemplo, basandose en los valores de L, LC, C, RC y R, dependiendo de cuáles de ellos estén activos, el robot realizará los movimientos definidos en el módulo de las ruedas.

Considerando el siguiente extracto del código:

``` c
if(((L==0 && C==1 && R==0) || (L==1 && C==1 && R==1)) && L==0 && R==0){
	wheels_cntrl_state_write(0);
}else if((LC==1 && C==0) || (L==1 && C==0 && R==0) || (L==1 && C==1 && R==0) ){
	wheels_cntrl_state_write(1);
}else if((C==0 && RC==1) || (L==0 && C==0 && R==1) || (L==0 && C==1 && R==1)){
	wheels_cntrl_state_write(2);
}else if(L==1 && LC==1 && C==1 && RC==1 && R==1){
	wheels_cntrl_state_write(3);
```

Cuando todos los infrarrojos se encuentren activos, el robot entiende que está en una intersección, esto hace que el robot se quede quieto y llame la función del ultrasonido, donde este mide hacia todas las direcciones y registra las distancias en cada una, tambien envía los datos por bluetooth y muestras las distancias en los displays de 7 segmentos. Esto se muestra en el siguiente extracto del código:

``` c
}else if(L==1 && LC==1 && C==1 && RC==1 && R==1){
	wheels_cntrl_state_write(3);
	int *d = US();
	showD(d);
```

De donde se ejecuta la siguiente sección que se encarga de comparar las mediciones de los registros y decidir qué dirección debe tomar el robot. Cuando elije una dirección, la anuncia por la bocina del MP3:

``` c
//Rotación
//d[0] derecha - d[1] centro - d[2] izquierda 
if(d[0] >= 35){
	mp3(1);
	wheels_cntrl_state_write(2);
	delay_ms(400);
	orientation = orientation + 1;
}else if(d[0] < 35 && d[1] >= 35){
	wheels_cntrl_state_write(0);
	delay_ms(200);
	mp3(3);
}else if(d[0] < 35 && d[1] < 35 && d[2] >= 35){
	mp3(2);
	wheels_cntrl_state_write(1);
	delay_ms(400);
	orientation = orientation - 1;
}else{			
	mp3(4);
	wheels_cntrl_state_write(5);
	delay_ms(200);
	wheels_cntrl_state_write(4);
	delay_ms(500);
	orientation = orientation + 2;
}
```

La variable orientation define en que dirección se esta moviendo el carro, de esta manera se puede establecer cual valor de la matriz se coloca en 1, dado que solo hay 4 direcciones, cuando realiza un giro completo se reinicia la dirección:

``` c
if(orientation >= 4)
	orientation = orientation - 4; 
else if(orientation < 0) 
	orientation = orientation + 4;
```

Finalmente se actualizan los datos de la matriz para ser enviada por bluetooth. Ejemplo de los datos que se envían:




Cuando se reciben los datos, se pueden mostrar de forma visual por medio de algún programa que interprete los strings que se obtienen por bluetooth, en este caso, se uso el lenguaje de programación Processing, de forma que el laberinto se ve de la siguiente manera:


![Alt text](/images/labP.png)


