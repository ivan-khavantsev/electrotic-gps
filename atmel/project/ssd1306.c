#include "ssd1306.h"
#include "i2c_master.h"
#include <string.h>
#include <stdlib.h>

#define SSD1306_ADDRESS 0x3C
#define SSD1306_128_64

static uint8_t buffer[SSD1306_LCDHEIGHT * SSD1306_LCDWIDTH / 8];
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
		// Init sequence
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

		ssd1306_command(SSD1306_SETCOMPINS);                    // 0xDA
		ssd1306_command(0x12);
		ssd1306_command(SSD1306_SETCONTRAST);                   // 0x81
		if (vccstate == SSD1306_EXTERNALVCC)
		{ ssd1306_command(0x9F); }
		else
		{ ssd1306_command(0xCF); }
		

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

		ssd1306_command(SSD1306_DISPLAYON);
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