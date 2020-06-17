#define F_CPU 8000000UL
#define SSD1306_128_64

#include <avr/io.h>
#include <avr/power.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "font.h"
#include "i2c_master.h"
#include "ssd1306.h"
#include <avr/pgmspace.h>
#include "gfxfont.h"
#include "uart.h"
#include <string.h>

#define USART0__RX_vect            _VECTOR(18)

#define rx_buffer_size 128
unsigned char rx_buffer[rx_buffer_size];
volatile unsigned char rx_read_pos=0;
volatile unsigned char available=0;

ISR(USART0__RX_vect)
{
	 while(!(UCSR0A & (1 << RXC0)));
	rx_buffer[rx_read_pos]=UDR0;
	rx_read_pos++;
	
	if(rx_read_pos>=rx_buffer_size)
	{
		rx_read_pos=0;
	}
	
	available++;
}

char serialRead()
{
	char c=0;
	while(available<=0);
	if(rx_read_pos>=available)
	{
		c=rx_buffer[rx_read_pos-available];
	}
	else
	{
		c=rx_buffer[rx_buffer_size+rx_read_pos-available];
	}
	available--;
	return c;
}

unsigned char command_buffer[rx_buffer_size];
volatile unsigned char command_buffer_position=0;

void handleCommand(){
	if(strncmp("$GPRMC", (char *)command_buffer, 6) == 0){
		clearDisplayBuffer();
		printString((char *)command_buffer);
		display();
	}
}

int main(void)
{
	clock_prescale_set(clock_div_1);
	uart_init(9600);
	sei();
	i2c_init();
	_delay_ms(10);
	ssd1306_begin(SSD1306_SWITCHCAPVCC);
	
    while (1) 
    {
		 unsigned char received = serialRead();
		 
		 command_buffer[command_buffer_position] = received;
		 command_buffer_position++;
		 if(received == '\n'){
			handleCommand(); // Обрабатываем команду
			memset(command_buffer, 0, rx_buffer_size); // Очищаем буфер комманды
			command_buffer_position = 0; // Возвращаем указатель вначало
		 }

    }
}