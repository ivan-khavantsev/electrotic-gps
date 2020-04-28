#ifndef F_CPU
#define F_CPU 8000000UL
#endif

#include <avr/io.h>
#include <util/delay.h>
#include "font.h"
#include "uart.h"
#include "i2c_master.h"
#include "ssd1306.h"

static void testdrawcircle(void)
{
	for (int16_t i=0; i<64; i+=2)
	{
		drawCircle(128/2, 64/2, i, WHITE);
		display();
		_delay_ms(1);
	}
}


int main(void)
{
	init_uart(4800);
	i2c_init();
	_delay_ms(10);
	ssd1306_begin(SSD1306_SWITCHCAPVCC);
	
    while (1) 
    {
		uart_putc(123);
		testdrawcircle();
    }
}

