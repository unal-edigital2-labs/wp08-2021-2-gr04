#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <irq.h>
#include <uart.h>
#include <console.h>
#include <generated/csr.h>

#include "delay.h"

#include "i2c.h"



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
    puts("i2c				- i2c test");
    
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

static void led_test(void)
{
	unsigned int form = 0;
	printf("Test del los leds... se interrumpe con el botton 1\n");
	while(!(buttons_in_read()&1)) {

		form = recibirForma();

		leds_out_write(form);
	}
	
}
/*
static void infra_test(void){

	printf("Test de infra ... se interrumpe con el boton 1\n");

	unsigned int seguidor = 0;

	while(!(buttons_in_read()&1)) {
		seguidor = infrarrojo_infras2_read();
		printf("Infrarrojo: %x \n",seguidor);
		delay_ms(1000);
		}

}*/

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

}


static void w_test(void){
	unsigned int state = 3;
	
	while(!(buttons_in_read()&1)){

		state = infrarrojo();
		ruedas_move_write(state);
		delay_ms(1000);
		/*switch(state){
			case 0: 
				ruedas_move_write(0);
				delay_ms(5000);
				state = 1;
				break;
			case 1: 
				ruedas_move_write(1);
				delay_ms(5000);
				state = 2;
				break;
			case 2: 
				ruedas_move_write(2);
				delay_ms(5000);
				state = 3;
				break;
			case 3: 
				ruedas_move_write(3);
				delay_ms(5000);
				state = 4;
				break; 
			case 4: 
				ruedas_move_write(4);
				delay_ms(5000);
				ruedas_move_write(3);
				return;
				break; 

		}*/
	}
}

// This file is Copyright (c) 2020 Antmicro <www.antmicro.com>
#ifdef CSR_I2C_BASE

#define I2C_PERIOD_CYCLES (CONFIG_CLOCK_FREQUENCY / I2C_FREQ_HZ)
#define I2C_DELAY(n)	  cdelay((n)*I2C_PERIOD_CYCLES/4)

static inline void cdelay(int i)
{
	while(i > 0) {
		__asm__ volatile(CONFIG_CPU_NOP);
		i--;
	}
}

static inline void i2c_oe_scl_sda(bool oe, bool scl, bool sda)
{
	i2c0_w_write(
		((oe & 1)  << CSR_I2C_W_OE_OFFSET)	|
		((scl & 1) << CSR_I2C_W_SCL_OFFSET) |
		((sda & 1) << CSR_I2C_W_SDA_OFFSET)
	);
}

// START condition: 1-to-0 transition of SDA when SCL is 1
static void i2c_start(void)
{
	i2c_oe_scl_sda(1, 1, 1);
	I2C_DELAY(1);
	i2c_oe_scl_sda(1, 1, 0);
	I2C_DELAY(1);
	i2c_oe_scl_sda(1, 0, 0);
	I2C_DELAY(1);
}

// STOP condition: 0-to-1 transition of SDA when SCL is 1
static void i2c_stop(void)
{
	i2c_oe_scl_sda(1, 0, 0);
	I2C_DELAY(1);
	i2c_oe_scl_sda(1, 1, 0);
	I2C_DELAY(1);
	i2c_oe_scl_sda(1, 1, 1);
	I2C_DELAY(1);
	i2c_oe_scl_sda(0, 1, 1);
}

// Call when in the middle of SCL low, advances one clk period
static void i2c_transmit_bit(int value)
{
	i2c_oe_scl_sda(1, 0, value);
	I2C_DELAY(1);
	i2c_oe_scl_sda(1, 1, value);
	I2C_DELAY(2);
	i2c_oe_scl_sda(1, 0, value);
	I2C_DELAY(1);
	i2c_oe_scl_sda(0, 0, 0);  // release line
}

// Call when in the middle of SCL low, advances one clk period
static int i2c_receive_bit(void)
{
	int value;
	i2c_oe_scl_sda(0, 0, 0);
	I2C_DELAY(1);
	i2c_oe_scl_sda(0, 1, 0);
	I2C_DELAY(1);
	// read in the middle of SCL high
	value = i2c0_r_read() & 1;
	I2C_DELAY(1);
	i2c_oe_scl_sda(0, 0, 0);
	I2C_DELAY(1);
	return value;
}

// Send data byte and return 1 if slave sends ACK
static bool i2c_transmit_byte(unsigned char data)
{
	int i;
	int ack;

	// SCL should have already been low for 1/4 cycle
	i2c_oe_scl_sda(0, 0, 0);
	for (i = 0; i < 8; ++i) {
		// MSB first
		i2c_transmit_bit((data & (1 << 7)) != 0);
		data <<= 1;
	}
	ack = i2c_receive_bit();

	// 0 from slave means ack
	return ack == 0;
}

// Read data byte and send ACK if ack=1
static unsigned char i2c_receive_byte(bool ack)
{
	int i;
	unsigned char data = 0;

	for (i = 0; i < 8; ++i) {
		data <<= 1;
		data |= i2c_receive_bit();
	}
	i2c_transmit_bit(!ack);

	return data;
}

// Reset line state
void i2c_reset(void)
{
	int i;
	i2c_oe_scl_sda(1, 1, 1);
	I2C_DELAY(8);
	for (i = 0; i < 9; ++i) {
		i2c_oe_scl_sda(1, 0, 1);
		I2C_DELAY(2);
		i2c_oe_scl_sda(1, 1, 1);
		I2C_DELAY(2);
	}
	i2c_oe_scl_sda(0, 0, 1);
	I2C_DELAY(1);
	i2c_stop();
	i2c_oe_scl_sda(0, 1, 1);
	I2C_DELAY(8);
}

/*
 * Read slave memory over I2C starting at given address
 *
 * First writes the memory starting address, then reads the data:
 *   START WR(slaveaddr) WR(addr) STOP START WR(slaveaddr) RD(data) RD(data) ... STOP
 * Some chips require that after transmiting the address, there will be no STOP in between:
 *   START WR(slaveaddr) WR(addr) START WR(slaveaddr) RD(data) RD(data) ... STOP
 */
bool i2c_read(unsigned char slave_addr, unsigned char addr, unsigned char *data, unsigned int len, bool send_stop)
{
	int i;

	i2c_start();

	if(!i2c_transmit_byte(I2C_ADDR_WR(slave_addr))) {
		i2c_stop();
		return false;
	}
	if(!i2c_transmit_byte(addr)) {
		i2c_stop();
		return false;
	}

	if (send_stop) {
		i2c_stop();
	}
	i2c_start();

	if(!i2c_transmit_byte(I2C_ADDR_RD(slave_addr))) {
		i2c_stop();
		return false;
	}
	for (i = 0; i < len; ++i) {
		data[i] = i2c_receive_byte(i != len - 1);
	}

	i2c_stop();

	return true;
}

/*
 * Write slave memory over I2C starting at given address
 *
 * First writes the memory starting address, then writes the data:
 *   START WR(slaveaddr) WR(addr) WR(data) WR(data) ... STOP
 */
bool i2c_write(unsigned char slave_addr, unsigned char addr, const unsigned char *data, unsigned int len)
{
	int i;

	i2c_start();

	if(!i2c_transmit_byte(I2C_ADDR_WR(slave_addr))) {
		i2c_stop();
		return false;
	}
	if(!i2c_transmit_byte(addr)) {
		i2c_stop();
		return false;
	}
	for (i = 0; i < len; ++i) {
		if(!i2c_transmit_byte(data[i])) {
			i2c_stop();
			return false;
		}
	}

	i2c_stop();

	return true;
}

/*
 * Poll I2C slave at given address, return true if it sends an ACK back
 */
bool i2c_poll(unsigned char slave_addr)
{
    bool result;

    i2c_start();
    result = i2c_transmit_byte(I2C_ADDR_RD(slave_addr));
    i2c_stop();

    return result;
}

#endif /* CSR_I2C_BASE */

static void sht(void){

        unsigned char valor;
        unsigned char dato;
        bool p_ack = false;

        valor = i2c0_w_sda_read();

        //p_ack = i2c_transmit_byte(valor);
        dato = i2c_receive_byte(p_ack);
        printf("El dato es: %u \n",dato);
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
    else if(strcmp(token, "i2c") == 0){
        sht();
    }
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

	return 0;
}
