#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h> 

#include <irq.h>
#include <uart.h>
#include <console.h>
#include <generated/csr.h>

#include "delay.h"

//#include <iostream>
#include <math.h>
#define _USE_MATH_DEFINES

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


static int infrarrojo(void){
   
	unsigned int entrada = infrarrojo_infras2_read();
	unsigned int salida=3;
	
	if((entrada == 0x4)||(entrada == 0x5)||(entrada == 0x6)||(entrada == 0x7)||(entrada == 0xC)||(entrada == 0xD)||(entrada == 0xE)||(entrada == 0xF)||(entrada == 0x14)||(entrada == 0x15)||(entrada == 0x16)||(entrada == 0x17)||(entrada == 0x1C)||(entrada == 0x1D)||(entrada == 0x1E)||(entrada == 0x1F)){
		if ((entrada == 0x7)||(entrada == 0xF)||(entrada == 0x17))
		{
			salida = 1;
			printf("Gire a la izquierda: %x \n",entrada);
		}
		else if ((entrada == 0x1C)||(entrada == 0x1D)||(entrada == 0x1E))
		{
			salida = 2;
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


//#include <iostream>
//#include <math.h>
//#define _USE_MATH_DEFINES
//using namespace std;
static void map(int direct) {
  //def matriz(direccion, fin, take)
    //take dice cuándo se agrega un punto a la matriz
    //Dir puede ser adelante, izquierda, derecha
    //dir = 3 ----> take = 1
    int ang = 0;
    int angulos [20][2]={{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0}};
    int coor[20][2]={{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0}};
    int dir;
    int inde = 0;
    int fin = 0;
    int angu = 0;
    int diro = 5;
	int infra = infrarrojo_infras2_read();
    while (inde!=19){
      while(fin!=1){
        //cout<<"Ingrese direccion: ";
        dir=direct;
        if(dir != diro){
          diro = dir;
          //Derecha
          if(dir == 1){
            ang = -90;
            angulos[inde+1][1]=ang;
            angulos[inde+1][0]=1;
          }
          //Izquierda
          if(dir == 2){
            ang = 90;
            angulos[inde+1][1]=ang;
            angulos[inde+1][0]=1;
          }
          //Avanza
          if(dir==0){
            ang = 0;
            angulos[inde+1][1]=ang;
            angulos[inde+1][0]=1;
          }
		  if (infra==0x1F)
		  {
			  fin = 1;
		  }
		  else
		  {
			  fin=0;
		  }
          inde++;
          }
		  //Aquí Borramos fin
        }
      if(fin==1){
      angulos[inde+1][1]=ang;
      angulos[inde+1][0]=0;
      inde++;
        }
        }

  
  int minx = 0;
  int maxx = 0;
  int miny = 0;
  int maxy = 0;
  
  
  for (int i = 1; i <= 19; i++){
      angu = angu + angulos[i][1];
      coor[i][0] = coor[i-1][0] + angulos[i][0]*cos(angu* M_PI / 180);
      coor[i][1] = coor[i-1][1] + angulos[i][0]*sin(angu* M_PI / 180);
    

    if(coor[i][0]>maxx){
      maxx=coor[i][0];
    }
    if(coor[i][0]<minx){
      minx=coor[i][0];
    }
    if(coor[i][1]>maxy){
      maxy=coor[i][1];
    }
    if(coor[i][1]<miny){
      miny=coor[i][1];
    }
  }
  printf("\n");
  char mapa[maxy-miny][maxx-minx];
  for(int l = 0;l <= (maxy-miny);l++){
    for(int j = 0; j<=(maxx-minx);j++){
      mapa[l][j] = 'x';
    }
  }
 char mapafinal[maxy-miny][maxx-minx];
for (int i=0;i<=(maxy-miny);i++){
    for(int j=0;j<=(maxx-minx);j++){
      printf("%c",mapa[i][j]);
    }
    printf("\n");
  }
   printf("\n");
  for (int i=0;i<=19;i++){
      mapa[coor[i][1]-miny][coor[i][0]-minx] = 'o';
  }
for (int i=0;i<=(maxy-miny);i++){
    for(int j=0;j<=(maxx-minx);j++){
      mapafinal[i][j] = mapa[(maxy-miny)-i][j];
    }
  }
  
  for (int i=0;i<=(maxy-miny);i++){
    for(int j=0;j<=(maxx-minx);j++){
      printf("%c",mapafinal[i][j]);
    }
    printf("\n");
  }
  
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
	puts("map                             - map test");
}

static void reboot(void)
{
	ctrl_reset_write(1);
}

static void mapa_test(void){
    unsigned int dir = 5;

    dir = infrarrojo();

    map(dir);

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
	else if(strcmp(token, "map") == 0)
		mapa_test();
	
		
	
	prompt();
}

int main(void)
{
	irq_setmask(0);
	irq_setie(1);
	uart_init();

	puts("\nSoC - RiscV project UNAL 2020-2-- CPU testing software built "__DATE__" "__TIME__"\n");
	help();
	prompt();

	while(1) {
		console_service();
	}
}




