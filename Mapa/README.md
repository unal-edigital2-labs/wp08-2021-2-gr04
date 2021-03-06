# Mapa de Memoria y Periféricos
## Mapa de memoria:

Se muestra la distribución general de la memoria por periféricos y sus respectivas direcciones:

<p align="center">
  <img width="250" height="400" src=/images/Memorymap.PNG>
</p>

## Periféricos

En esta sección se describen los periféricos usados para la elaboración del robot cartógrafo, sus espacios en memoria, el desarrollo tanto en software como en hardware y código asociado a cada uno de estos. A continuación puede dirigirse al módulo que prefiera para dar con la documentación ligada:




1. [ Radar. ](#radar)
2. [ Ruedas. ](#ruedas)
3. [ InfraRojo. ](#InfraRojo)
4. [ Cámara. ](#Camara)

<a name="radar"></a>



# Radar
El modulo del radar esta compuesto por el ultrasonido y un servomotor, cada registro se encuentra en el espacio de memoria 0x82001800 y 0x82001000 respectivamente. A continuación se explica como es su implementación en hardware:

![Screenshot](/images/radar.JPG)



<a name="us"></a>
## Ultrasonido

Este sensor permite medir la distancia a la cual se encuentra un objeto mediante una señal ultrasónica cuya frecuencia de trabajo es de 40kHz. Su principio de funcionamiento se basa en calcular el tiempo que tarda la señal en llegar hasta el objeto y regresar, considerando la velocidad del sonido en el aire y a partir de ello determinar la ditancia.
Los pines que posee este elemento son VCC(5V), Trig, Echo y GND. En primer lugar el módulo dispara la onda mediante "Trig" o disparador, y esta onda al encontrarse con el objeto rebotará y será recibida por "Echo".
      
      
 Así, conociendo el tiempo que tarda la onda en regresar y la velocidad a la que lo hace, es posible determinar la distancia mediante la ecuación d = v*t. No obstante, este resultado corresponderá a la distancia total recorrida por la onda al ir hasta el objeto y regresar, por lo tanto, para determinar la distancia a la cual se encuentra situado el objeto es necesario dividir este resultado entre 2. De este modo, TRIG se encarga de enviar la señal y su estado permanece en ALTO durante 10us, momento en el que envía una señal de 8 ciclos a 40kHz. Una vez se ha enviado la onda el pin ECHO pasará al estado ALTO y permanecerá así hasta que reciba la onda de regreso.
      
      
  Un factor importante a considerar durante la medición es el ángulo en el que se encuentra posicionado el objeto con respecto al sensor, ya que esto puede incidir en especial en la señal de retorno, de manera que la medición será más eficiente cuanto más alineado se encuentre el objeto respecto del sensor. Una desviación de 15 a 30 grados es aceptable de acuerdo con la hoja de especificaciones de este elemento.
      

En esta sección se establece que cuando la señal init esta en 1, los registros done, counter, y distance se colocan en 0 y se pasa al siguiente estado.

``` verilog
module ultrasonido(clk, trig, echo, dist, init, done);
input init;
input clk;
input echo;
output reg trig;
output reg [8:0] dist;
output reg done;
reg [26:0] cont;
reg [26:0] cont_t;

initial begin 
	cont = 0;
	trig = clk;
	dist = 0;
	cont_t = 0;
	done = 0;
end

always @(posedge clk) begin
if(init) begin
	cont = cont + 1;
	
	if (cont<1024)
		trig = 1'b1;
	else begin
		trig = 1'b0;
		
		if (echo == 1)begin
			
			cont_t = cont_t + 1;
			dist = (cont_t*34/200_000);
		
			end
		else if (echo == 0)begin
			
			cont_t = 0;
			done = 1;
			
			end
		end
	end	
	else cont = 0;
		
end

endmodule

```



Para la implementación en python, se establece que init es un registro tipo Storage con el fin de poderlo editar por software, la distancia y donde se colocan como status, y las señales echo y trig se establecen como salidas de la FPGA.
``` python
class US(Module, AutoCSR):
    def __init__(self, echo, trig):
        self.clk = ClockSignal()
        self.init = CSRStorage(1)
        self.echo = echo
        self.trig = trig
        self.dist = CSRStatus(9)
        self.done = CSRStatus(1)

        self.specials += Instance("ultrasonido",
            i_clk = self.clk,
            i_init = self.init.storage,
            i_echo = self.echo,
            o_trig = self.trig,
            o_dist = self.dist.status,
            o_done = self.done.status)
```


En el archivo buildSoCproject se crea el submodulo y se importan los pines de entrada y salida del modulo:

``` python
SoCCore.add_csr(self, "ultrasonido")
		self.submodules.ultrasonido = ultrasonido.US(platform.request("trig"), platform.request("echo"))
```

<a name="pwmus"></a>
## Servomotor del ultra sonido

Para este modulo se utiliza el servomotor sg90. Tal y como se indica en el  [datasheet](../datasheets/sg90_datasheet.pdf) la señal pwm debe ser de 50 Hz.A partir de lo anterior, se puede concluir que para mantener una  posicion a 0° debe tener un ancho de pulso de 1 ms, mientras que para una pocision a 180° el ancho debe ser de 2 ms. Con esto en mente, se implementa el siguiente codigo en verilog:


En el caso de el servomotor que se utilizo, los anchos de pulso varian, por lo que los tiempos utilizados a 0° y 180° varian por 2ms. Debido a esto, se establece un contador hasta un millon para de esta manera tener un pwm con un periodo de 20 ms. Ademas, es importante mencionar que  el ancho de pulso del pwm es controlado a traves de la entrada CTR, cuando se llega a 50.000 se coloca el servo en 0°, cuando el contador llega a 150.000 se coloca en 90°, y cuando se llega a 250.000 se coloca en 180°.

``` verilog
always@(posedge clk)begin
	contador = contador + 1;
	if(contador =='d1_000_000) begin
	   contador = 0;
	end
	
	case(ctr)
        2'b00:  servo = (contador < 'd50_000) ? 1:0;
        
        2'b01:  servo = (contador < 'd150_000) ? 1:0;
        
        2'b10:  servo = (contador < 'd250_000) ? 1:0;
        
        default:servo = (contador < 'd50_000) ? 1:0;
    endcase

end
```

En python se importa el modulo donde se maneja el registro CTR como Storage para que se pueda editar desde C, y la señal servo como una conexion externa en la FPGA.

``` python
class servoUS(Module, AutoCSR):
    def __init__(self, servo):
            self.clk = ClockSignal()
            self.ctr = CSRStorage(2)
            self.servo = servo

            self.specials += Instance("servo_radar",
                i_clk = self.clk,
                i_ctr = self.ctr.storage,
                o_servo = self.servo,
)

```

Luego se añade el submodulo al soc y se importan los pines.

``` python
#servo
SoCCore.add_csr(self,"servo_radar")
		self.submodules.servo_radar = pwm.servoUS(platform.request("servo"))
```



<a name="ruedas"></a>
# Ruedas

Para las ruedas se utilizó un puente H (modelo L298N) conectado a los motores en vez de un PWM. el resgistro correspondiente a este periferico  se encuentra en el espacio de memoria 0x82002800.

![Screenshot](/images/ruedas1.png)

El mapa de memoria para las ruedas es el que se muestra a continuación

![Screenshot](/images/ruedas.JPG)

Se toma en cuenta un único registro ubicado en el espacio de memoria 0x820058, este registro se refiere al estado que define cuál será el movimiento de las ruedas, así como se describe más adelante.

La conexión se realiza partiendo de los 4 pines de la fpga que se conectan a 4 puertos de entrada del puente H, entonces se asignan 2 pines de salida a la rueda izquierda y los otros  2 pines de salida  a la rueda derech. Siendo mas espeficos, para los dos pares de pines asignados a cada rueda, uno de los pines es el encargado de mover la rueda en una dirección (hacia adelante/sentido horario) y otro para moverlo en la dirección contraria (hacia atrás/sentido antihorario), estos pines son activados con señales en 1 y 0, por lo que no pueden estar ambos pines en 1 a la misma vez.

![Screenshot](/images/puenteh.png)

El módulo en verilog para el puente H es de la siguiente forma:

``` verilog

 // Recto
        3'b000:  begin
                    right[0] = 1;
                    right[1] = 0;
                    left[0]  = 1;
                    left[1]  = 0;
                end
```

Donde se indica a cuál dirección se moverá cada una de las ruedas, right o left, (siendo right[0] hacia adelante y right[1] hacia atrás). En este caso se activa el movimiento de las mismas de acuerdo a como se quiera mover el robot.

Como segundo ejemplo se puede observar el código:

``` verilog
        // Giro 180
        3'b100:  begin
                    right[0] = 0;
                    right[1] = 1;
                    left[0]  = 1;
                    left[1]  = 0;
                end
```

Como se muestra, para girar 180 grados se activa la rueda derecha girando hacia atrás y la rueda izquierda girando hacia delante. Para escoger estos movimientos, se ingresa un estado a la variable de entrada move que puede tomar valores del 0 al 5, de esta manera se escoge cuál rueda se activa y qué tipo de movimiento tiene. Los movimientos considerados son: Recto, derecha, izquierda, quieto, giro 180 y retroceder.

El código elaborado en python es el siguiente:


``` python

from migen import *
from migen.genlib.cdc import MultiReg
from litex.soc.interconnect.csr import *
from litex.soc.interconnect.csr_eventmanager import *

class wheels(Module, AutoCSR):
    def __init__(self, right, left):
            self.clk = ClockSignal()
            self.move = CSRStorage(3)
            self.right = right
            self.left = left

            self.specials += Instance("ruedas",
                i_clk = self.clk,
                i_move = self.move.storage,
                o_right = self.right,
                o_left = self.left)
```

Se agrega la señal del reloj y la variable ctr, esta variable se define tipo storage para poder controlarla desde software. Las señales right y left son las que se usan en los pines del puente H para controlar las ruedas. 

Para la implementación en el BuildSoCproject se elaboró el siguiente código:

``` python

SoCCore.add_csr(self,"ruedas")
		right = Cat(*[platform.request("right", i) for i in range(2)])
		left = Cat(*[platform.request("left", i) for i in range(2)])
		self.submodules.ruedas = motores.wheels(right, left)
```

Con esto se realiza un barrido de 1 a 2 en left y right debido a que cada uno tiene 2 pines, esto evita que se haga manualmente. En wheels_cntrl se definen los pines a usar, en este caso los elegidos anteriormente: right y left


En la sección de firmware de esta documentación se puede conseguir la explicación del funcionamiento de las ruedas junto a los infrarrojos y el ultrasonido.




<a name="InfraRojo"></a>
# InfraRojo

Este dispositivo se basa en un fotodiodo emisor de luz y un fototransistor. Su funcionamiento consiste en la emision de luz por el fotodiodo por el cual al recibir esta energia en el fototransistor a traves de la base para permitir un flujo de corriente a traves del colector y el emisor. La luz no se puede reflejar a traves de una superficie negra, por lo que al incidir sobre esta no es posiblle generar la corriente en el colector, de modo que para obtener una señal activa es necesario invertir el funcionamiento de los infrarojos para que estos operenc bajo una superficie negra. En este proyecto en particular, se hizo uso de un seguidor de liena que funciona a traves de 5 infrarojos. Ademas, cabe mencionar que cada sensor solo cuenta con un unico pin de entrada y salida. 

![Screenshot](/images/facil.jpeg)

El modulo del infrarojo esta compuesto por una serie de registros que se encuentran en el espacio de memoria 0x82002000. A continuación se explica como es su implementación en hardware:

![Screenshot](/images/infrarojo.JPG)

El módulo en verilog para el seguidor de linea es de la siguiente forma:


``` verilog  
module infrarrojo(
      input [4:0] infras,
      output [4:0] infras2
    );

     assign infras2 = infras;

    endmodule  
```

 El desarrollo de este modulo es muy simple, ya que se declaran 5 pines de la FPGA como entradas infras y luego se igualan a los 5 pines de salida infras2 tal y como se puede ver en las lieneas anteriores.
 
 El código elaborado en  es el siguiente:
 ``` python
 # Modulo Principal
class Infrarrojo(Module,AutoCSR):
    def __init__(self, infras):
     
        self.infras = infras
        self.infras2 = CSRStatus(5)

        self.specials +=Instance("infrarrojo",
            i_infras = self.infras,
            o_infras2 = self.infras2.status,
        )
Se define a la variable infras2 como de tipo status, debido a que solo nos interesa ser capaces de leer el valor almacenado de esta variable desde sofware. 

Luego se añade el submodulo y se importan los pines.

 ``` python
	SoCCore.add_csr(self,"infrarrojo")
		infras_in = Cat(*[platform.request("infras", i) for i in range(5)])
		self.submodules.infrarrojo = infrarrojo.Infrarrojo(infras_in)
``` 

<a name="Camara"></a>
# Cámara
Conozcamos primero la cámara modelo OV7970 sin FIFO:

-Puede llegar a una resolución máxima de 640x480, este tamaño es ajustable

-Cuenta, entre otros, con RGB565/555/444 como formato de salida

-Cuenta con protocolo I2C

<p align="center">
  <img width="220" height="205" src=/images/OV7670.png>
</p>


La limitación de memoria de la tarjeta de desarrollo hace que optemos por usar el formato 640x480 con RGB444, de esta manera cada píxel requerirá 12 bits para un total de 450kB para la representación de la imagen. Decidimos hacer el procesamiento de imagen por Hardware.

En primer lugar, veamos cómo obtenemos la información de la cámara:

Cuando tenemos la salida en formato RGB444 la cámara usa un primer ciclo de PCLK para enviar un byte de información, donde la primer mitad son los bits correspondientes a Red y la otra mitad no tiene relevancia, en el siguiente ciclo de PCLK la cámara envía un segundo byte donde la primer mitad son los bits correspondientes a Green, y la otra mitad a Blue, teniendo así la información de un pixel en 2 ciclos de PCLK; este proceso se repite sucesivamente durante 1280 ciclos de reloj, el equivalente a 640 pixeles. Durante este periodo la señal HREF es un 1 lógico, indicando que se está en toma de información de una hilera de pixeles.


<p align="center">
  <img width="720" height="405" src=/images/RGB444.png>
</p>

Ahora se repite este proceso 480 veces con un delay de 288 ciclos de PCLK de por medio en el cual los valores tomados son inválidos, estos ciclos se dividen un margen margen anterior de 38 ciclos de reloj, luego 160 ciclos donde HSYNC toma valor de 0 y por último 90 de margen posterior como lo sugiere el datasheet. Una vez obtenidas las 480 líneas se toman 10 valores más de hileras pero con datos inválidos luego de lo cual y durante el tiempo que tomaría sacar 3 hileras más de valores VSYNC toma valor de 1 lógico. De este modo HSYNC sincroniza horizontalmente la imagen en el monitor (cada hilera de pixeles), y VSYNC sincroniza la representación de frames (paquetes de 480 hileras).

<p align="center">
  <img width="720" height="405" src=/images/VGATiming.png>
</p>

Para información más detallada consultar el datasheet

## Identificacion de forma y color
### Identificacion de forma
Para identificar que hay un objeto uniforme en la cámara se evalúa la matriz de datos de pixeles, se hace agrupaciones horizontales de 3 pixeles: si estos son idénticos, es decir, tienen el mismo color, la agrupación es válida, si 3 pixeles adyacentes no tienen el mismo color es una agrupación inválida. Las agrupaciones se van sumando en secuencia en la fila, una vez la fila ha terminado obtenemos el número de tríos de pixeles válidos, valor que llamaremos Ancho. Iterando en cada hilera de pixeles podemos guardar el valor máximo de tríos que haya tenido alguna hilera, lo que podemos llamar como Ancho Medio; además podemos saber si el ancho ha incrementado de una fila a otra y llevar un valor representativo de este cambio a lo largo de todas las filas que llamaremos Ancho Incremental, el cual se hace más grande si el ancho actual es mayor al ancho anterior y decrece si es lo contrario.

<p align="center">
  <img width="720" height="405" src=/images/ejmterna.png>
</p>


Así, el triángulo se diferencia de las demás figuras en que el ancho incremental es mayor que 100; el cuadrado se diferencia del círculo en que su Ancho Medio es semejante a su Ancho Incremental, mientras que el del círculo su Ancho Medio es más grande que su Ancho incremental, en otras palabras, el cuadrado a diferencia del círculo varía muy poco su ancho.

<p align="center">
  <img width="720" height="405" src=/images/formas.png>
</p>

### Identificación de color

Se lleva en un registro la sumatoria de los componentes cromáticos de cada grupo de pixeles válidos que hay en la matriz, al final, la componente cuyo valor sea mayor será el color predominante y la salida para este aspecto.

### CamaraVGADriver

Este es el modulo top entregado al cual se le agrego el modulo OV767, VGA_driver y los modulos correspondientes al buffer de la camara.


``` verilog
module CamaraVGADriver(
	input Clock,
	input Reset,
	
	//Camara
	input Pclock,
	input Href,
	input Vsync_cam,
	input [7:0] Data,
	output XClock,
	
	//Mapa Ram
	input [3:0] MapaData,
	input [5:0] MapaAddr,
	input MapaWrite,
		
	//Procesamiento de imagen
	output wire [1:0] Forma,
	output wire [1:0] PromedioColor,
	
	//VGA
	output [11:0] RGB,
	output Hsync,
	output Vsync
);

//Write Cam
wire [11:0] PixelDataCam;
wire [14:0] PixelAddrCam;
wire WPixelCam;

//VGA-Cam
wire [11:0] DataCamOut;
wire [14:0] AddrCamOut; 

//VGA 
wire [11:0] PixelData;
wire [9:0] Xpixel;
wire [8:0] Ypixel;



reg [11:0] Colores=1'b0;
reg [2:0] CounterX=1'b0;
reg [2:0] CounterY=1'b0;
wire [5:0] AddrMapaOut;
wire [3:0] DataMapaOut;



reg [11:0] ColoresProm=1'b0;
reg [11:0] ColoresForma=1'b0;



assign AddrMapaOut=(((Xpixel<10'd450)&&(Xpixel>10'd9)) &&	((Ypixel<10'd460)&&(Ypixel>10'd19)))?(CounterX+CounterY*4'd8):1'b0;
assign AddrCamOut=(((Xpixel<10'd629)&&(Xpixel>10'd452)) &&	((Ypixel<9'd311)&&(Ypixel>9'd166)))?((Xpixel-10'd453)+(Ypixel-9'd167)*9'd176):1'b0; //Miramos si el pixel esta en el recuadro VALIDO Y solicitamos la dirección de memoria del pixel valido

//Descomentar esto para activar en la pantalla el resultado del procesamiento de imagen

assign PixelData=(((Xpixel==10'd591)&&(Ypixel<9'd271) && (Ypixel>9'd188)) || ((Xpixel==10'd490)&&(Ypixel<9'd271) && (Ypixel>9'd188)) || ((Ypixel==9'd271)&&(Xpixel<10'd591) && (Xpixel>10'd490)) || ((Ypixel==9'd188)&&(Xpixel<10'd591) && (Xpixel>10'd490)))?12'hF00:(((Xpixel<10'd629)&&(Xpixel>10'd452)) && ((Ypixel<9'd311)&&(Ypixel>9'd166)))?DataCamOut:((((Xpixel<10'd450)&&(Xpixel>10'd9)) &&	((Ypixel<10'd460)&&(Ypixel>10'd19)))?Colores:(Ypixel>10'd240)?ColoresProm:ColoresForma);

//Primer condiciconal Zona valida 
//Segundod condicional identifica pixel a dentro o fuera. Colores--> espacio restante abajo y luego otro espacio restante forma (arriba). 

ImBuffer #(12, 15,"Im.mem") Camara (PixelDataCam, AddrCamOut, PixelAddrCam, WPixelCam, XClock, Pclock, DataCamOut);
ImBufferv2 #(4, 6,"Mapa.mem") Mapa (MapaData, AddrMapaOut, MapaAddr, MapaWrite, Clock, Clock, DataMapaOut);


VGA_Driver640x480 VGA(Reset, XClock, PixelData, RGB, Hsync, Vsync, Xpixel, Ypixel);

OV7670 Cam(Reset, PixelDataCam, PixelAddrCam, WPixelCam, Pclock, Href, Vsync_cam, Data, PromedioColor,Forma);

clk24_25_nexys4
  clk25_24(
  .CLK_IN1(Clock),
  .CLK_OUT1(XClock),
  .CLK_OUT2(Clock24),
  .RESET(Reset)
 );


//A continuación se hace un Zoom:v de pixeles
always @ (Xpixel, Ypixel) begin
		

		if (((Xpixel<10'd450)&&(Xpixel>10'd9)) &&	((Ypixel<10'd460)&&(Ypixel>10'd19))) begin
	
			if(Xpixel<10'd65) CounterX=3'd0;
			else if (Xpixel<10'd120) CounterX=3'd1;
			else if (Xpixel<10'd175) CounterX=3'd2;
			else if (Xpixel<10'd230) CounterX=3'd3;
			else if (Xpixel<10'd285) CounterX=3'd4;
			else if (Xpixel<10'd340) CounterX=3'd5;
			else if (Xpixel<10'd395) CounterX=3'd6;
			else if (Xpixel<10'd450) CounterX=3'd7;
			
			if(Ypixel<10'd75) CounterY=3'd0;
			else if (Ypixel<10'd130) CounterY=3'd1;
			else if (Ypixel<10'd185) CounterY=3'd2;
			else if (Ypixel<10'd240) CounterY=3'd3;
			else if (Ypixel<10'd295) CounterY=3'd4;
			else if (Ypixel<10'd350) CounterY=3'd5;
			else if (Ypixel<10'd405) CounterY=3'd6;
			else if (Ypixel<10'd460) CounterY=3'd7;
			
		end
			
		case(DataMapaOut)
		
		3'b000:Colores=12'h000;
		3'b001:Colores=12'h5FF;
		3'b010:Colores=12'hAFc;
		3'b011:Colores=12'hA0A;
		3'b100:Colores=12'hA00;
		3'b101:Colores=12'h0A0;
		3'b110:Colores=12'h00A;
		default:Colores=12'h000;
		
		endcase
		
		case(PromedioColor)
		
		2'b00:ColoresProm=12'h000;
		2'b01:ColoresProm=12'hF00;
		2'b10:ColoresProm=12'h0F0;
		2'b11:ColoresProm=12'h00F;
		default:ColoresProm=12'h000;
		
		endcase
		
		case(Forma)
		
		2'b00:ColoresForma=12'h000;
		2'b01:ColoresForma=12'hF00;
		2'b10:ColoresForma=12'h0F0;
		2'b11:ColoresForma=12'h00F;
		default:ColoresForma=12'h000;
		
		endcase


			
end


endmodule
```

### OV767

Este modulo se encarga de recolectar los datos y enviarlos a la memoria.

``` verilog
module OV7670(
	input Reset,
	output reg [11:0]  PixelData=1'b0, // Salida de los 12 bits (4R, 4G, 4B) ya concatenados.
	output [14:0] PAddress, // Guardar en memoría la dirección asignada. 
	output reg WPixel=1'b0,
	input Pclock,
	input Href, // Protocolo VGA
	input Vsync, // Protocolo VGA
	input [7:0] Data, // Info de la camara (8 bits) --> envia en dos ciclos 4 -4 y luego en otro 4 y otro no validso
	output reg [1:0] Promedio=1'b0, //Color promedio
	output reg [1:0] Forma=1'b0 //Forma de la figura
);


reg Href_prev=1'b0;//Saber cuando Href cambia
	reg [7:0] XPixel=1'b0; //Información de la posición horizontal del pixel
	reg [7:0] YPixel=1'b0; //Información de la posición vertical del pixel
	reg [11:0] PromedioColorR=1'b0; //Aquí se realiza el registro de los trios de pixeles rojos que se encuentran en la imagen
	reg [11:0] PromedioColorG=1'b0; //Aquí se realiza el registro de los trios de pixeles verdes que se encuentran en la imagen
	reg [11:0] PromedioColorB=1'b0; //Aquí se realiza el registro de los trios de pixeles azules que se encuentran en la imagen
reg VSync_prev=1'b0;//Saber cuando Vsinc pasa de 0 - 1 
	reg Sync=1'b0; // Sincronización para pasar al siguiente pixel (nueva posición X)


	reg [1:0] Fila_Valida_R=1'b0; //Verifica si hay 3 pixeles seguidos del mismo color (Rojo) 
	reg [1:0] Fila_Valida_G=1'b0; //Verifica si hay 3 pixeles seguidos del mismo color (Verde) 
	reg [1:0] Fila_Valida_B=1'b0; //Verifica si hay 3 pixeles seguidos del mismo color (Azul) 

	reg [5:0] Ancho_R=1'b0; //Nos indica cuantos trios de pixeles rojos validos hay por fila
	reg [5:0] Ancho_G=1'b0; //Nos indica cuantos trios de pixeles verdes validos hay por fila
	reg [5:0] Ancho_B=1'b0; //Nos indica cuantos trios de pixeles azules validos hay por fila
	reg [5:0] Ancho_R_Prev=1'b0; //Nos almacena la información de Ancho_R una vez se ha terminado una fila, antes de asignar a Ancho_R a 0 
reg [5:0] Ancho_G_Prev=1'b0; 
reg [5:0] Ancho_B_Prev=1'b0; 
	reg [7:0] Inc_Ancho_R=1'b0; //Nos indica si el ancho de la fila anterior es mayor o menor que a fila actual
reg [7:0] Inc_Ancho_G=1'b0;
reg [7:0] Inc_Ancho_B=1'b0;
	reg [7:0] MID_Ancho_R=1'b0; //Nos indica el ancho mayor de la figura. 
reg [7:0] MID_Ancho_G=1'b0;
reg [7:0] MID_Ancho_B=1'b0;


assign PAddress = XPixel + YPixel*8'd176; //Se guarde en memoría la dirección asignada



	always @(posedge Pclock)begin //Inicia la máquina de estados


		if(Reset==1'b1)begin //Inicializamos los registros en 0 
	WPixel=1'b0;
	Href_prev=1'b0;
	VSync_prev=1'b0;
	XPixel=1'b0;
	YPixel=1'b0;
	Sync=1'b0;
	PixelData=1'b0;
		
	PromedioColorR=1'b0;
	PromedioColorG=1'b0;
	PromedioColorB=1'b0;
	Promedio=1'b0;
	
	Fila_Valida_R=1'b0;
	Fila_Valida_G=1'b0;
	Fila_Valida_B=1'b0;
	
	Ancho_R=1'b0; 
	Ancho_G=1'b0;
	Ancho_B=1'b0;
	Ancho_R_Prev=1'b0; 
	Ancho_G_Prev=1'b0; 
	Ancho_B_Prev=1'b0; 
	Inc_Ancho_R=1'b0;
	Inc_Ancho_G=1'b0;
	Inc_Ancho_B=1'b0;
	MID_Ancho_R=1'b0;
	MID_Ancho_G=1'b0;
	MID_Ancho_B=1'b0;

end
else begin

	if(Vsync==1'b1)begin // Si Vsync esta en 1 no se realiza ninguna acción
		Sync=1'b0;
		WPixel=1'b0;
		XPixel=1'b0;
		YPixel=1'b0;
		
	end
	else if(Href==1'b0 && Href_prev==1'b1)begin //Si se da un cambio en Href entonces iniciamos el procesamiento
		Sync=1'b0;
		WPixel=1'b0;
		XPixel=1'b0;
		YPixel=YPixel+1'b1;
		
		
		
		//Incrementos del contador de Ancho teniendo en cuenta el color
		if(Ancho_R>Ancho_R_Prev)Inc_Ancho_R=Inc_Ancho_R+1'b1;
		else if(Ancho_R<Ancho_R_Prev)Inc_Ancho_R=Inc_Ancho_R-1'b1;
		
		if(Ancho_G>Ancho_G_Prev)Inc_Ancho_G=Inc_Ancho_G+1'b1;
		else if(Ancho_G<Ancho_G_Prev)Inc_Ancho_G=Inc_Ancho_G-1'b1;
		
		if(Ancho_B>Ancho_B_Prev)Inc_Ancho_B=Inc_Ancho_B+1'b1;
		else if(Ancho_B<Ancho_B_Prev)Inc_Ancho_B=Inc_Ancho_B-1'b1;
		
		//Mayor ancho
		if(Inc_Ancho_R>MID_Ancho_R)MID_Ancho_R=Inc_Ancho_R;
		if(Inc_Ancho_G>MID_Ancho_G)MID_Ancho_G=Inc_Ancho_G;
		if(Inc_Ancho_B>MID_Ancho_B)MID_Ancho_B=Inc_Ancho_B;
		//Ancho MID = Ancho Máx
		
		//Reset Ancho cada Hsync dentro del VSync, es decir cada cambio de fila.
		if(Vsync==1'b0 && VSync_prev==1'b0)begin
			Ancho_R_Prev=Ancho_R;
			Ancho_G_Prev=Ancho_G; 
			Ancho_B_Prev=Ancho_B;
			Ancho_R=1'b0; 
			Ancho_G=1'b0; 
			Ancho_B=1'b0; 		
		end

		
	end
	else if(Href==1'b1)begin
		if(Sync==1'b0)begin
			PixelData[11:4]=Data;
			Sync=1'b1;
			WPixel=1'b0;
		end
		else begin
			PixelData[3:0]=Data[7:4];
			XPixel=XPixel+1'b1;
			Sync=1'b0;
			WPixel=1'b1;
		end
	end
	
	
	
	
	
	//Si pixel valido y estamos en el recuadro de 100x100 valido entonces:
	if(WPixel==1'b1 && (XPixel<8'd138) && (XPixel>8'd38) && (YPixel<8'd122) && (YPixel>8'd22))begin
	
	
	//Conteo de 3 pixeles de igual proporcion de color seguidos
		if((PixelData[3:0]>PixelData[7:4]) && (PixelData[3:0]>PixelData[11:8]))begin
			Fila_Valida_B=Fila_Valida_B+1'b1;
			Fila_Valida_G=1'b0;
			Fila_Valida_R=1'b0;
		end
		else if((PixelData[7:4]>PixelData[3:0]) && (PixelData[7:4]>PixelData[11:8]))begin
			Fila_Valida_G=Fila_Valida_G+1'b1;
			Fila_Valida_B=1'b0;
			Fila_Valida_R=1'b0;
			
		end
		else if((PixelData[11:8]>PixelData[3:0]) && (PixelData[11:8]>PixelData[7:4]))begin
			Fila_Valida_R=Fila_Valida_R+1'b1;
			Fila_Valida_G=1'b0;
			Fila_Valida_B=1'b0;		
		end
		
		
		
		//Si existen 3 pixeles de proporcion de color seguidos
		if(Fila_Valida_R==2'b11)begin
					Fila_Valida_R=1'b0;
					PromedioColorR=PromedioColorR+1'b1;
					Ancho_R=Ancho_R+1'b1;
		end
		if(Fila_Valida_G==2'b11)begin
					Fila_Valida_G=1'b0;
					PromedioColorG=PromedioColorG+1'b1;
					Ancho_G=Ancho_G+1'b1;
		end
		if(Fila_Valida_B==2'b11)begin
					Fila_Valida_B=1'b0;
					PromedioColorB=PromedioColorB+1'b1;
					Ancho_B=Ancho_B+1'b1;
		end

	end
	
	
	//Final de cada Imagen 
	if(Vsync==1'b1 && VSync_prev==1'b0)begin
		if(PromedioColorR>PromedioColorG && PromedioColorR>PromedioColorB)begin
			Promedio=2'b01;

			if(Inc_Ancho_R<(9'd100+9'd12) && Inc_Ancho_R>(9'd100-9'd12))begin
				if(Inc_Ancho_R<(MID_Ancho_R-9'd12))Forma=2'b10;
				else Forma=2'b01;			   
			end
			else Forma=2'b11;			
			
		end
		else if(PromedioColorG>PromedioColorR && PromedioColorG>PromedioColorB)begin
			Promedio=2'b10;
			
			if(Inc_Ancho_G<(9'd100+9'd12) && Inc_Ancho_G>(9'd100-9'd12))begin
				if(Inc_Ancho_G<(MID_Ancho_G-9'd12))Forma=2'b10;
				else Forma=2'b01;			   
			end
			else Forma=2'b11;

		end
		else if(PromedioColorB>PromedioColorR && PromedioColorB>PromedioColorG)begin
			Promedio=2'b11;
			if(Inc_Ancho_B<(9'd100+9'd12) && Inc_Ancho_B>(9'd100-9'd12))begin
				if(Inc_Ancho_B<(MID_Ancho_B-9'd12))Forma=2'b10;
				else Forma=2'b01;			   
			end
			else Forma=2'b11;

		end
		else begin
			Promedio=2'b00;
			Forma=2'b00;
			
		end
		
		PromedioColorR=1'b0;
		PromedioColorG=1'b0;
		PromedioColorB=1'b0;
		Ancho_R=1'b0; 
		Ancho_G=1'b0;
		Ancho_B=1'b0;
		Ancho_R_Prev=1'b0; 
		Ancho_G_Prev=1'b0; 
		Ancho_B_Prev=1'b0; 
		Inc_Ancho_R=8'd100;
		Inc_Ancho_G=8'd100;
		Inc_Ancho_B=8'd100;
		MID_Ancho_R=1'b0;
		MID_Ancho_G=1'b0;
		MID_Ancho_B=1'b0;
	
	end
	
	
	VSync_prev=Vsync;
	Href_prev=Href;
	
	if ((XPixel>8'd176-1'b1) || (YPixel>8'd144-1'b1)) PixelData=1'b0;


end



end



endmodule
```


### ImBuffer

Este modulo describe el dual port de la memoria ram la cual recibe los datos de la cámara.

``` verilog
module ImBuffer
#(parameter DATA_WIDTH=12, parameter ADDR_WIDTH=15, parameter imageFILE="Im.mem")
(
	input [(DATA_WIDTH-1):0] data,
	input [(ADDR_WIDTH-1):0] read_addr, write_addr,
	input we, read_clock, write_clock,
	output reg [(DATA_WIDTH-1):0] q
);
	
	// Declare the RAM variable
	reg [DATA_WIDTH-1:0] ram[2**ADDR_WIDTH-1:0];
	
	
	
	initial begin
		$readmemh(imageFILE, ram);
		ram[15'b111111111111111]=12'b00000000;
	end
	
	always @ (posedge write_clock)
	begin
		// Write
		if (we)
			ram[write_addr] <= data;
	end
	
	always @ (negedge read_clock)
	begin
		// Read 
		q <= ram[read_addr];
	end
	
endmodule
```

### ImBufferv2

Este modulo describe el dual port de la memoria ram la cual recibe los datos de la cámara.

``` verilog
module ImBufferv2
#(parameter DATA_WIDTH=4, parameter ADDR_WIDTH=6, parameter imageFILE="Mapa2.mem")
(
	input [(DATA_WIDTH-1):0] data,
	input [(ADDR_WIDTH-1):0] read_addr, write_addr,
	input we, read_clock, write_clock,
	output reg [(DATA_WIDTH-1):0] q
);
	
	// Declare the RAM variable
	reg [DATA_WIDTH-1:0] ram[2**ADDR_WIDTH-1:0];
	
	
	
	initial begin
		$readmemh(imageFILE, ram);
	end
	
	always @ (posedge write_clock)
	begin
		// Write
		if (we)
			ram[write_addr] <= data;
	end
	
	always @ (negedge read_clock)
	begin
		// Read 
		q <= ram[read_addr];
	end
	
endmodule
```


### VGA_driver

Esta diseñado para que la transmisión de datos no sea superior a RGB444 por este motivo se elige este formato.

``` verilog
module VGA_Driver640x480 (
	input rst,
	input clk, 				// 25MHz  para 60 hz de 640x480
	input  [11:0] pixelIn, 	// entrada del valor de color  pixel 
	
	output  [11:0] pixelOut, // R, G y B --> salida del valor pixel a la VGA - Es el mismo de entrada. 
	output  Hsync_n,		// señal de sincronización en horizontal negada
	output  Vsync_n,		// señal de sincronización en vertical negada 
	output  [9:0] posX, 	// posicion en horizontal del pixel siguiente
	output  [8:0] posY 		// posicion en vertical  del pixel siguiente
);

localparam SCREEN_X = 640; 	// tama�o de la pantalla visible en horizontal 
localparam FRONT_PORCH_X =16;  
localparam SYNC_PULSE_X = 96;
localparam BACK_PORCH_X = 28;
localparam TOTAL_SCREEN_X = SCREEN_X+FRONT_PORCH_X+SYNC_PULSE_X+BACK_PORCH_X; 	// total pixel pantalla en horizontal 


localparam SCREEN_Y = 480; 	// tama�o de la pantalla visible en Vertical 
localparam FRONT_PORCH_Y =10;  
localparam SYNC_PULSE_Y = 2;
localparam BACK_PORCH_Y = 33;
localparam TOTAL_SCREEN_Y = SCREEN_Y+FRONT_PORCH_Y+SYNC_PULSE_Y+BACK_PORCH_Y; 	// total pixel pantalla en Vertical 


reg  [9:0] countX=SCREEN_X; // no se deja en cero para hacer mas rapida la simulaci�n
reg  [8:0] countY=SCREEN_Y; // no se deja en cero para hacer mas rapida la simulaci�n

assign posX    = countX;
assign posY    = countY;

assign pixelOut = (countX<SCREEN_X) ? (pixelIn) : (12'd0) ;

assign Hsync_n = ~((countX>=SCREEN_X+FRONT_PORCH_X) && (countX<SCREEN_X+SYNC_PULSE_X+FRONT_PORCH_X)); 
assign Vsync_n = ~((countY>=SCREEN_Y+FRONT_PORCH_Y) && (countY<SCREEN_Y+FRONT_PORCH_Y+SYNC_PULSE_Y));


always @(posedge clk) begin
	if (rst) begin
		countX <= SCREEN_X; // no se deja en cero para hacer mas rapida la simulaci�n
		countY <= SCREEN_Y; // no se deja en cero para hacer mas rapida la simulaci�n
	end
	else begin 
		if (countX >= (TOTAL_SCREEN_X-1)) begin
			countX <= 0;
			if (countY >= (TOTAL_SCREEN_Y-1)) begin
				countY <= 0;
			end 
			else begin
				countY <= countY + 1;
			end
		end 
		else begin
			countX <= countX + 1;
			countY <= countY;
		end
	end
end

endmodule
```
se crea el archivo camara.py la cual describe las entradas y salidas utilizadas por la cámara para realizar la correcta integración del modulo en el SoC, a continuación el código:


``` python
from migen import *
from migen.genlib.cdc import MultiReg
from litex.soc.interconnect.csr import *
from litex.soc.interconnect.csr_eventmanager import *
# Modulo Principal
class Camara(Module,AutoCSR):
    def __init__(self, CAM_xclk,CAM_pclk,cam_data_in, CAM_vsync, CAM_href):
        self.clk = ClockSignal()   
        #self.rst = ResetSignal()  
        self.CAM_xclk = CAM_xclk
        
        self.CAM_pclk = CAM_pclk
        self.CAM_vsync= CAM_vsync
        self.CAM_href= CAM_href
        self.CAM_px_data = cam_data_in
        self.data_mem = CSRStatus(12)
        self.DP_RAM_addr_out = CSRStorage(14)
        self.specials +=Instance("test_cam",
            i_clk = self.clk,
            #i_rst = self.rst,
            o_CAM_xclk = self.CAM_xclk,
            i_CAM_pclk = self.CAM_pclk,
            i_CAM_px_data=self.CAM_px_data,
            o_data_mem=self.data_mem.status,
            o_CAM_vsync= self.CAM_vsync,
            o_CAM_href= self.CAM_href,
            i_DP_RAM_addr_out= self.DP_RAM_addr_out.storage)
```

Tenemos entonces el mapa de memoria detallado para el módulo general VGA_MAPA

<p align="center">
  <img width="520" height="305" src=/images/mpdcamara.PNG>
</p>
