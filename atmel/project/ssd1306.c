#include "ssd1306.h"
#include "i2c_master.h"
#include <string.h>
#include <stdlib.h>
#include "gfxfont.h"
#include <avr/pgmspace.h>

#ifndef _swap_int16_t
#define _swap_int16_t(a, b) { int16_t t = a; a = b; b = t; }
#endif

#ifndef pgm_read_byte
#define pgm_read_byte(addr) (*(const unsigned char *)(addr))
#endif
#ifndef pgm_read_word
#define pgm_read_word(addr) (*(const unsigned short *)(addr))
#endif
#ifndef pgm_read_dword
#define pgm_read_dword(addr) (*(const unsigned long *)(addr))
#endif

#define SSD1306_ADDRESS (0b00111100<<1)|0
#define SSD1306_128_64

static uint8_t buffer[SSD1306_LCDHEIGHT * SSD1306_LCDWIDTH / 8];

int cursor_x = 0;
int cursor_y = 0;

#define ssd1306_swap(a, b) { int16_t t = a; a = b; b = t; }
	
	void drawPixel(int16_t x, int16_t y, uint16_t color)
	{
		if ((x < 0) || (x >= SSD1306_LCDWIDTH) || (y < 0) || (y >= SSD1306_LCDHEIGHT))
		return;
		
		switch (color)
		{
			case WHITE:   buffer[x+ (y/8)*SSD1306_LCDWIDTH] |=  (1 << (y&7)); break;
			case BLACK:   buffer[x+ (y/8)*SSD1306_LCDWIDTH] &= ~(1 << (y&7)); break;
			case INVERSE: buffer[x+ (y/8)*SSD1306_LCDWIDTH] ^=  (1 << (y&7)); break;
		}
	}
	
	
	void ssd1306_command(uint8_t c)
	{
		i2c_start(SSD1306_ADDRESS);
		i2c_write(0x00);      // Co = 0, D/C = 0
		i2c_write(c);
		i2c_stop();
	}

	void ssd1306_begin(uint8_t vccstate)
	{
		ssd1306_command(SSD1306_DISPLAYOFF);                    // 0xAE
		ssd1306_command(SSD1306_SETDISPLAYCLOCKDIV);            // 0xD5
		ssd1306_command(0x80);                                  // the suggested ratio 0x80

		ssd1306_command(SSD1306_SETMULTIPLEX);                  // 0xA8
		ssd1306_command(SSD1306_LCDHEIGHT - 1);

		ssd1306_command(SSD1306_SETDISPLAYOFFSET);              // 0xD3
		ssd1306_command(0x0);                                   // no offset
		ssd1306_command(SSD1306_SETSTARTLINE | 0x0);            // line #0
		ssd1306_command(SSD1306_CHARGEPUMP);                    // 0x8D
		if (vccstate == SSD1306_EXTERNALVCC)
		{ ssd1306_command(0x10); }
		else
		{ ssd1306_command(0x14); }
		ssd1306_command(SSD1306_MEMORYMODE);                    // 0x20
		ssd1306_command(0x00);                                  // 0x0 act like ks0108
		ssd1306_command(SSD1306_SEGREMAP | 0x1);
		ssd1306_command(SSD1306_COMSCANDEC);

		#if defined SSD1306_128_32
		ssd1306_command(SSD1306_SETCOMPINS);                    // 0xDA
		ssd1306_command(0x02);
		ssd1306_command(SSD1306_SETCONTRAST);                   // 0x81
		ssd1306_command(0x8F);
		#elif defined SSD1306_128_64
		ssd1306_command(SSD1306_SETCOMPINS);                    // 0xDA
		ssd1306_command(0x12);
		ssd1306_command(SSD1306_SETCONTRAST);                   // 0x81
		if (vccstate == SSD1306_EXTERNALVCC)
		{ ssd1306_command(0x9F); }
		else
		{ ssd1306_command(0xCF); }
		#elif defined SSD1306_96_16
		ssd1306_command(SSD1306_SETCOMPINS);                    // 0xDA
		ssd1306_command(0x2);   //ada x12
		ssd1306_command(SSD1306_SETCONTRAST);                   // 0x81
		if (vccstate == SSD1306_EXTERNALVCC)
		{ ssd1306_command(0x10); }
		else
		{ ssd1306_command(0xAF); }
		#endif

		ssd1306_command(SSD1306_SETPRECHARGE);                  // 0xd9
		if (vccstate == SSD1306_EXTERNALVCC)
		{ ssd1306_command(0x22); }
		else
		{ ssd1306_command(0xF1); }
		ssd1306_command(SSD1306_SETVCOMDETECT);                 // 0xDB
		ssd1306_command(0x40);
		ssd1306_command(SSD1306_DISPLAYALLON_RESUME);           // 0xA4
		ssd1306_command(SSD1306_NORMALDISPLAY);                 // 0xA6

		ssd1306_command(SSD1306_DEACTIVATE_SCROLL);

		ssd1306_command(SSD1306_DISPLAYON);//--turn on oled panel
	}

void display(void)
{
	ssd1306_command(SSD1306_COLUMNADDR);
	ssd1306_command(0);   // Column start address (0 = reset)
	ssd1306_command(SSD1306_LCDWIDTH-1); // Column end address (127 = reset)
	ssd1306_command(SSD1306_PAGEADDR);
	ssd1306_command(0); // Page start address (0 = reset)
	ssd1306_command(7); // Page end address

	for (uint8_t packet = 0; packet < 64; packet++) {
		i2c_start(SSD1306_ADDRESS);
		i2c_write(0x40);
		for (uint8_t packet_byte = 0; packet_byte < 16; ++packet_byte) {
			i2c_write(buffer[packet*16+packet_byte]);
		}
		i2c_stop();
	}
}

// clear everything
void clearDisplay(void)
{
	memset(buffer, 0, (SSD1306_LCDWIDTH*SSD1306_LCDHEIGHT/8));
	cursor_x = 0;
	cursor_y = 0;
}


// Draw a circle outline
void drawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color) 
{
	int16_t f = 1 - r;
	int16_t ddF_x = 1;
	int16_t ddF_y = -2 * r;
	int16_t x = 0;
	int16_t y = r;

	drawPixel(x0  , y0+r, color);
	drawPixel(x0  , y0-r, color);
	drawPixel(x0+r, y0  , color);
	drawPixel(x0-r, y0  , color);

	while (x<y) {
		if (f >= 0) {
			y--;
			ddF_y += 2;
			f += ddF_y;
		}
		x++;
		ddF_x += 2;
		f += ddF_x;

		drawPixel(x0 + x, y0 + y, color);
		drawPixel(x0 - x, y0 + y, color);
		drawPixel(x0 + x, y0 - y, color);
		drawPixel(x0 - x, y0 - y, color);
		drawPixel(x0 + y, y0 + x, color);
		drawPixel(x0 - y, y0 + x, color);
		drawPixel(x0 + y, y0 - x, color);
		drawPixel(x0 - y, y0 - x, color);
	}
}

// Bresenham's algorithm - thx wikpedia
void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color) 
{
	int16_t steep = abs(y1 - y0) > abs(x1 - x0);
	if (steep) {
		_swap_int16_t(x0, y0);
		_swap_int16_t(x1, y1);
	}

	if (x0 > x1) {
		_swap_int16_t(x0, x1);
		_swap_int16_t(y0, y1);
	}

	int16_t dx, dy;
	dx = x1 - x0;
	dy = abs(y1 - y0);

	int16_t err = dx / 2;
	int16_t ystep;

	if (y0 < y1) {
		ystep = 1;
		} else {
		ystep = -1;
	}

	for (; x0<=x1; x0++) {
		if (steep) {
			drawPixel(y0, x0, color);
			} else {
			drawPixel(x0, y0, color);
		}
		err -= dy;
		if (err < 0) {
			y0 += ystep;
			err += dx;
		}
	}
}

// Draw a PROGMEM-resident 1-bit image at the specified (x,y) position,
// using the specified foreground color (unset bits are transparent).
void drawBitmap(int16_t x, int16_t y, const uint8_t bitmap[], int16_t w, int16_t h, uint16_t color) 
{
	int16_t byteWidth = (w + 7) / 8; // Bitmap scanline pad = whole byte
	uint8_t byte = 0;
	for(int16_t j=0; j<h; j++, y++) {
		for(int16_t i=0; i<w; i++) {
			if(i & 7) byte <<= 1;
			else      byte   = pgm_read_byte(&bitmap[j * byteWidth + i / 8]);
			if(byte & 0x80) drawPixel(x+i, y, color);
		}
	}
}

// Draw PROGMEM-resident XBitMap Files (*.xbm), exported from GIMP,
// Usage: Export from GIMP to *.xbm, rename *.xbm to *.c and open in editor.
// C Array can be directly used with this function.
// There is no RAM-resident version of this function; if generating bitmaps
// in RAM, use the format defined by drawBitmap() and call that instead.
void drawXBitmap(int16_t x, int16_t y, const uint8_t bitmap[], int16_t w, int16_t h, uint16_t color) 
{
	int16_t byteWidth = (w + 7) / 8; // Bitmap scanline pad = whole byte
	uint8_t byte = 0;
	for(int16_t j=0; j<h; j++, y++) {
		for(int16_t i=0; i<w; i++ ) {
			if(i & 7) byte >>= 1;
			else      byte   = pgm_read_byte(&bitmap[j * byteWidth + i / 8]);
			// Nearly identical to drawBitmap(), only the bit order
			// is reversed here (left-to-right = LSB to MSB):
			if(byte & 0x01) drawPixel(x+i, y, color);
		}
	}
}

// (x,y) is topmost point; if unsure, calling function
// should sort endpoints or call drawLine() instead
void drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color) 
{
    // Update in subclasses if desired!
    drawLine(x, y, x, y+h-1, color);
}

void drawFillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) 
{
	for (int16_t i=x; i<x+w; i++) {
		drawFastVLine(i, y, h, color);
	}
}

void drawChar(int16_t x, int16_t y, unsigned char c, uint16_t color, uint16_t bg, uint8_t size) 
{
		if((x >= SSD1306_LCDWIDTH)            || // Clip right
		(y >= SSD1306_LCDWIDTH)           || // Clip bottom
		((x + 6 * size - 1) < 0) || // Clip left
		((y + 8 * size - 1) < 0))   // Clip top
		return;

		//if(!_cp437 && (c >= 176)) c++; // Handle 'classic' charset behavior

		for(int8_t i=0; i<5; i++ ) { // Char bitmap = 5 columns
			uint8_t line = pgm_read_byte(&font[c * 5 + i]);
			for(int8_t j=0; j<8; j++, line >>= 1) {
				if(line & 1) {
					if(size == 1)
					drawPixel(x+i, y+j, color);
					else
					drawFillRect(x+i*size, y+j*size, size, size, color);
					} else if(bg != color) {
					if(size == 1)
					drawPixel(x+i, y+j, bg);
					else
					drawFillRect(x+i*size, y+j*size, size, size, bg);
				}
			}
		}
}



void print(unsigned char c){
	if(cursor_y > 64-8){
		clearDisplay();
		cursor_x = 0;
		cursor_y = 0;
	}
	
	if(c != 13 && c != '\n'){
		drawChar(cursor_x, cursor_y, c, WHITE, WHITE, 1);	
		cursor_x += 6;
	} 
	if(c == '\n')
	{
		cursor_x = 0;
		cursor_y += 8;
	}
	
	if(cursor_x > 128-6){
		cursor_x = 0;
		cursor_y += 8;
	}
}
