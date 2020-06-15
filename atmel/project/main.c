
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
	
	for (int16_t i=0; i<128; i+=6)
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
	for(int i=0; i<128+19+21; i++){	
		drawChar(x,y,i,WHITE,WHITE,1);		
		x+=6;
		if(x > 128-6){
			x = 1;
			y += 8;
		}
		display();
	}
}

const unsigned char testbitmap[] PROGMEM = {
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x0F,0x00,0x00,0x00,0x00,0x00,0x00,0x07,0x07,0x80,0x00,0x00,0x00,
	0x00,0x00,0x00,0x7F,0xC0,0x00,0x00,0x00,0x00,0x00,0x3F,0xFF,0xF0,0x00,0xF0,0x10,
	0x00,0x00,0x03,0xF0,0x40,0x00,0x00,0x00,0x00,0x01,0xF9,0xE1,0xFE,0x03,0x90,0x20,
	0x00,0x00,0x07,0xE1,0xC0,0x00,0x00,0x00,0x00,0x03,0xE3,0x60,0x7F,0x07,0x10,0xC0,
	0x00,0x00,0x1F,0x83,0x80,0x00,0x00,0x00,0x00,0x0F,0x84,0x60,0x1F,0xDE,0x33,0x80,
	0x00,0x00,0x3F,0x03,0x80,0x00,0x00,0x00,0x00,0x1F,0x18,0x40,0x07,0xBC,0x7E,0x00,
	0x00,0x00,0x7E,0x02,0x00,0x00,0x00,0x00,0x00,0x3E,0x30,0xC0,0x03,0x7B,0xFC,0x00,
	0x00,0x01,0xF8,0x00,0x00,0x00,0x00,0x00,0x00,0xF8,0x20,0x80,0x00,0xF7,0xF0,0x00,
	0x00,0x01,0xF8,0x03,0x80,0x1C,0x03,0x80,0x01,0xF0,0x41,0x80,0x00,0xEF,0xC0,0x00,
	0x00,0x03,0xE0,0x1F,0xC0,0xFE,0x0F,0xF8,0xE3,0xF0,0x43,0x00,0x03,0xC1,0x00,0x00,
	0x00,0x0F,0xC0,0x3C,0x41,0xE2,0x1E,0x79,0xE7,0xC0,0x86,0x00,0x07,0x82,0x00,0x00,
	0x00,0x0F,0xC0,0x78,0x43,0xCE,0x3C,0xF0,0x0F,0xC0,0x8C,0x00,0x07,0x04,0x00,0x00,
	0x00,0x1F,0x80,0xF8,0x47,0x9C,0x71,0xE0,0x1F,0x80,0xF8,0x00,0x0F,0x0C,0x00,0x00,
	0x00,0x3F,0x01,0xEC,0xDF,0x18,0xF1,0xC0,0x1F,0x00,0x00,0x00,0x1E,0x10,0x00,0x00,
	0x00,0x3E,0x03,0xE7,0xFE,0x01,0xE3,0xC0,0x3E,0x00,0x00,0x00,0x3C,0x20,0x00,0x00,
	0x00,0x7E,0x07,0xC1,0x3C,0x03,0xC7,0x80,0x7C,0x00,0x00,0x00,0x38,0x40,0x00,0x00,
	0x00,0x7C,0x07,0x82,0x78,0x07,0x87,0x18,0xFC,0x1E,0x00,0xF0,0x79,0x87,0xF8,0x00,
	0x00,0xFC,0x0F,0x02,0x78,0x0F,0x8F,0x10,0xF8,0x7F,0x03,0xD8,0xF3,0x1F,0xF8,0x00,
	0x00,0xF8,0x0F,0x0C,0xF8,0x1F,0x1E,0x21,0xF0,0xF1,0x07,0x19,0xE6,0x3C,0xF0,0x00,
	0x01,0xF8,0x1F,0x18,0xF8,0x3F,0x3C,0xC1,0xF1,0xE1,0x1F,0x11,0xF8,0xF8,0xF0,0x00,
	0x01,0xF0,0x1F,0x30,0xF0,0x7E,0x7D,0xC3,0xE3,0xC1,0x3F,0x13,0xF0,0xF1,0xE0,0x00,
	0x01,0xF0,0x1F,0xE0,0xF9,0x9F,0xFF,0x03,0xE3,0xC2,0x3B,0x03,0xC1,0xE3,0xC0,0x00,
	0x01,0xF0,0x1F,0xC0,0xFF,0x1F,0xFE,0x07,0xE3,0xC2,0x79,0xE7,0x83,0xC3,0x80,0x00,
	0x01,0xF0,0x0F,0x00,0x7C,0x1E,0x38,0x07,0xE7,0x06,0xF0,0x7F,0x87,0x87,0x80,0x00,
	0x01,0xF0,0x00,0x00,0x00,0x00,0x00,0x07,0xC4,0x0D,0xE0,0xCF,0x07,0x0F,0x10,0x00,
	0x01,0xF0,0x00,0x00,0x00,0x00,0x00,0x07,0xC0,0x09,0xE0,0x8F,0x0F,0x1F,0x30,0x00,
	0x01,0xF0,0x00,0x00,0x00,0x00,0x00,0x07,0xC0,0x11,0xC1,0x1E,0x1F,0x1E,0x60,0x00,
	0x01,0xF8,0x00,0x00,0x3F,0xFF,0xC0,0x07,0xC0,0x63,0xC3,0x1E,0x3E,0x3C,0xC0,0x00,
	0x00,0xF8,0x00,0x01,0xFF,0xFF,0xF0,0x07,0xE0,0xE3,0xC6,0x1E,0x7E,0x7D,0x80,0x00,
	0x00,0x7E,0x00,0x7E,0x01,0xFF,0xFC,0x07,0xFF,0x83,0xF8,0x1F,0xFF,0xFF,0x00,0x00,
	0x00,0x3F,0xFF,0x00,0x00,0x3E,0x0E,0x01,0xFE,0x03,0xF0,0x1F,0x3F,0x7C,0x00,0x00,
	0x00,0x0F,0xF8,0x00,0x00,0x18,0x01,0x00,0xFC,0x01,0xC0,0x1E,0x1E,0x38,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};

const unsigned char testbitmap2[] PROGMEM = {
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
	drawBitmap(0,0,testbitmap,128,64,WHITE);
	display();
}

void test(){
	testdrawbitmap();
	_delay_ms(3000);
	testdrawchar();
	_delay_ms(1000);
	clearDisplay();
	testdrawchar();
	_delay_ms(3000);
	testdrawlines();
	_delay_ms(2000);
	clearDisplay();
	testdrawcircle();
	_delay_ms(2000);
	clearDisplay();
}

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









int main(void)
{
	clock_prescale_set(clock_div_1);
	
	uart_init(9600);
	
	sei();
	i2c_init();
	_delay_ms(10);
	ssd1306_begin(SSD1306_SWITCHCAPVCC);
	
	int count = 0;
	
	//test();
	
    while (1) 
    {
		
		
		 unsigned char received = serialRead();
		 uart_putc(received);
		 count++;
		 print(received);
		 if(received == '\n'){
			display();
			clearDisplay();
			count = 0;
		 }
		  
		 if(count >= 74){
			display();
			count = 0; 
		 }
			
		
    }
}