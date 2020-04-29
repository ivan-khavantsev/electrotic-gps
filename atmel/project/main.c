
#define F_CPU 16000000UL
#define SSD1306_128_64


#include <avr/io.h>
#include <avr/power.h>
#include <util/delay.h>
#include "font.h"
#include "i2c_master.h"
#include "ssd1306.h"
#include <avr/pgmspace.h>
#include "gfxfont.h"

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


static void testdrawchar()
{
	int x = 1;
	int y = 1;
	for(int i=0; i<128; i++){
		
		drawChar(x,y,i,WHITE,WHITE,1);		
		x+=6;
		if(x > 128-6){
			x = 1;
			y += 8;
		}
		display();
	}

	
}

const unsigned char bitmap[] PROGMEM = {
	0b01100110,
	0b10011001,
	0b10000001,
	0b01000010,
	0b00100100,
	0b00100100,
	0b00011000,
	0b00000000
};

static void testdrawbitmap(void)
{
	drawBitmap(56,56,bitmap,8,8,WHITE);
	display();
}

int main(void)
{
	clock_prescale_set(clock_div_1);
	
	i2c_init();
	_delay_ms(10);
	ssd1306_begin(SSD1306_EXTERNALVCC);
	
    while (1) 
    {
		testdrawbitmap();
		_delay_ms(2000);
		testdrawchar();
		_delay_ms(10000);
		testdrawlines();
		_delay_ms(2000);
		testdrawcircle();
		_delay_ms(2000);
		clearDisplay();
    }
}