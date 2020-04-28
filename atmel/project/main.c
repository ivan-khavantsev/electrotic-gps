
#define F_CPU 16000000UL
#define SSD1306_128_64


#include <avr/io.h>
#include <avr/power.h>
#include <util/delay.h>
#include "font.h"
#include "uart.h"
#include "i2c_master.h"
#include "ssd1306.h"
#include <avr/interrupt.h>

static void testdrawcircle(void)
{
	for (int16_t i=0; i<64; i+=2)
	{
		drawCircle(128/2, 64/2, i, WHITE);
		display();
		_delay_ms(100);
	}
}

static void testdrawlines(void)
{
	for (int16_t i=0; i<64; i+=8)
	{
		drawLine(0, i, 128, i, WHITE);
		display();
		_delay_ms(50);
	}
	
	for (int16_t i=0; i<128; i+=8)
	{
		drawLine(i, 0, i, 64, WHITE);
		display();
		_delay_ms(50);
	}
}

int main(void)
{
	clock_prescale_set(clock_div_1);
	
	//USART_Init();
	//sei();
	
	i2c_init();
	_delay_ms(10);
	ssd1306_begin(SSD1306_EXTERNALVCC);
	
    while (1) 
    {
		
		uint8_t bitmap[5] = {0x3A, 0x44, 0x44, 0x44, 0x3A};
		
		drawBitmap(20,20,bitmap, 7,5, WHITE);
		display();
		_delay_ms(2000);
		testdrawlines();
		
		_delay_ms(2000);
		testdrawcircle();
		_delay_ms(2000);
		clearDisplay();
    }
}