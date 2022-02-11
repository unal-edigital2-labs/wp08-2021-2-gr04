#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h> 

#include <irq.h>
#include <uart.h>
#include <console.h>
#include <generated/csr.h>

#include "delay.h"

static char *readstr(void)
{
	char c[2];
	static char s[64];
	static int ptr = 0;

	if(readchar_nonblock()) {
		c[0] = readchar();
		c[1] = 0;
		switch(c[0]) {
			case 0x7f:
			case 0x08:
				if(ptr > 0) {
					ptr--;
					putsnonl("\x08 \x08");
				}
				break;
			case 0x07:
				break;
			case '\r':
			case '\n':
				s[ptr] = 0x00;
				putsnonl("\n");
				ptr = 0;
				return s;
			default:
				if(ptr >= (sizeof(s) - 1))
					break;
				putsnonl(c);
				s[ptr] = c[0];
				ptr++;
				break;
		}
	}
	return NULL;
}

static char *get_token(char **str)
{
	char *c, *d;

	c = (char *)strchr(*str, ' ');
	if(c == NULL) {
		d = *str;
		*str = *str+strlen(*str);
		return d;
	}
	*c = 0;
	d = *str;
	*str = c+1;
	return d;
}

static void prompt(void)
{
	printf("RUNTIME>");
}

static void help(void)
{
	puts("Available commands:");
	puts("help                            - this command");
	puts("reboot                          - reboot CPU");
	puts("led                             - led test");
	puts("IR				- IR test");
	puts("w				- wheels test");
	puts("US			- ultrasound test");
	//puts("servo			- servo test");
}

static void reboot(void)
{
	ctrl_reset_write(1);
}

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

static int led_test(void)
{
	unsigned int form = 0;
	unsigned int col = 0;
	unsigned int img = 0;
	//printf("Test del los leds... se interrumpe con el botton 1\n");
	
	//while(!(buttons_in_read()&1)) {

		form = recibirForma();
		col = recibirColor()*8;
		img = form+col;
	//}
	return img;
}

//Sigue blanco
/*
static int infrarrojo(void){
   
	unsigned int entrada = infrarrojo_infras2_read();
	unsigned int salida=3;
	
	if((entrada == 0x4)||(entrada == 0x5)||(entrada == 0x6)||(entrada == 0x7)||(entrada == 0xC)||(entrada == 0xD)||(entrada == 0xE)||(entrada == 0xF)||(entrada == 0x14)||(entrada == 0x15)||(entrada == 0x16)||(entrada == 0x17)||(entrada == 0x1C)||(entrada == 0x1D)||(entrada == 0x1E)||(entrada == 0x1F)){
		if ((entrada == 0x7)||(entrada == 0xF)||(entrada == 0x17))
		{
			salida = 2;
			printf("Gire a la izquierda: %x \n",entrada);
		}
		else if ((entrada == 0x1C)||(entrada == 0x1D)||(entrada == 0x1E))
		{
			salida = 1;
			printf("Gire a la derecha: %x \n",entrada);
		}
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

}*/
/*
static int infrarrojo(void){
   
	unsigned int entrada = infrarrojo_infras2_read();
	unsigned int salida=3;
	
	if((entrada == 0x1B)||(entrada == 0x1A)||(entrada == 0x19)||(entrada == 0x18)||(entrada == 0x13)||(entrada == 0x12)||(entrada == 0x11)||(entrada == 0x10)||(entrada == 0xB)||(entrada == 0xA)||(entrada == 0x9)||(entrada == 0x8)||(entrada == 0x3)||(entrada == 0x2)||(entrada == 0x1)||(entrada == 0x0)){
		if ((entrada == 0x18)||(entrada == 0x10)||(entrada == 0x8))
		{
			salida = 2;
			printf("Gire a la izquierda: %x \n",entrada);
		}
		else if ((entrada == 0x3)||(entrada == 0x2)||(entrada == 0x1))
		{
			salida = 1;
			printf("Gire a la derecha: %x \n",entrada);
		}
		else if ((entrada == 0x0))
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

}*/


static int infrarrojo(void){
   
	unsigned int entrada = infrarrojo_infras2_read();
	unsigned int salida=3;
	
	if((entrada == 0x4)||(entrada == 0x5)||(entrada == 0x6)||(entrada == 0x7)||(entrada == 0xC)||(entrada == 0xD)||(entrada == 0xE)||(entrada == 0xF)||(entrada == 0x14)||(entrada == 0x15)||(entrada == 0x16)||(entrada == 0x17)||(entrada == 0x1C)||(entrada == 0x1D)||(entrada == 0x1E)||(entrada == 0x1F)){
		if ((entrada == 0x7)||(entrada == 0xF)||(entrada == 0x17))
		{
			salida = 1;
			printf("Gire a la derecha: %x \n",entrada);
		}
		else if ((entrada == 0x1C)||(entrada == 0x1D)||(entrada == 0x1E))
		{
			salida = 2;
			printf("Gire a la izquierda: %x \n",entrada);
		}
		else if ((entrada == 0x1F))
		{
			salida = 3;
			printf("Se salió: %x \n",entrada);
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

/*
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
	
	
}*/

static void ultraSound_test(void)
{
	unsigned int distancia;

		distancia = ultrasonido_distancia_read();	//distancia en centimetros
		printf("distancia : %d \n",distancia);

	}


static void servo_test(int intersect)
{
	unsigned int grados = 0;
	//unsigned int intersect = 0;
	unsigned int distancia1=0;
	unsigned int distancia2=0;
	unsigned int distancia3=0;
	//ultrasonido_init_write(0);
	if(intersect == 3){
		switch (grados)
		{
		case 0x00:
			servo_radar_ctr_write(0);
			delay_ms(1000);
				//distancia1 = ultrasonido_distancia_read();
				delay_ms(100);
				printf("distancia 1: %d \n",distancia1);
				distancia1 = 0;		
			grados = 0x01;
			delay_ms(1000);
			
		case 0x01:
			servo_radar_ctr_write(1); 
			//ultrasonido_init_write(1);
			delay_ms(1000);
			//ultrasonido_init_write(1);
			//delay_ms(100);
				//distancia2 = ultrasonido_dist_read();
				delay_ms(100);
				//ultrasonido_init_write(0);
				printf("distancia 2: %d \n",distancia2);
				distancia2 = 0;
			grados = 0x02;
			delay_ms(1000);
		case 0x02:
			servo_radar_ctr_write(2);
			//ultrasonido_init_write(1);
			delay_ms(1000);
			//ultrasonido_init_write(1);
			//delay_ms(100);
				//distancia3 = ultrasonido_dist_read();
				delay_ms(100);
				//ultrasonido_init_write(0);
				printf("distancia 3: %d \n",distancia3);
				distancia3 = 0;
			delay_ms(1000);
			//return;
			//break;
		
		default: grados = 0;
			break;
		}
	}
	else
	{
		servo_radar_ctr_write(2); 
	}
	

}

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
}

static void console_service(void)
{
	char *str;
	char *token;

	str = readstr();
	if(str == NULL) return;
	token = get_token(&str);
	if(strcmp(token, "help") == 0)
		help();
	else if(strcmp(token, "reboot") == 0)
		reboot();
	else if(strcmp(token, "led") == 0)
		led_test();
	else if(strcmp(token, "IR") == 0)
		infrarrojo();
	else if(strcmp(token, "w") == 0)
		w_test();
	else if(strcmp(token, "US") == 0)
		ultraSound_test();
	//else if(strcmp(token, "servo") == 0)
	//	servo_test();
	prompt();
}

int main(void)
{
	irq_setmask(0);
	irq_setie(1);
	uart_init();

	puts("\nSoC - RiscV project UNAL 2021-2-- CPU testing software built "__DATE__" "__TIME__"\n");
	help();
	prompt();

	while(1) {
		console_service();
	}

	return 0;
}
