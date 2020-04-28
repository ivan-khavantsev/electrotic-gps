// Font structures for newer Adafruit_GFX (1.1 and later).
// Example fonts are included in 'Fonts' directory.
// To use a font in your Arduino sketch, #include the corresponding .h
// file and pass address of GFXfont struct to setFont().  Pass NULL to
// revert to 'classic' fixed-space bitmap font.
#ifndef _GFXFONT_H_
#define _GFXFONT_H_

typedef struct { // Data stored PER GLYPH
	u16 bitmapOffset;     // Pointer into GFXfont->bitmap
	u8  width, height;    // Bitmap dimensions in pixels
	u8  xAdvance;         // Distance to advance cursor (x axis)
	s8  xOffset, yOffset; // Dist from cursor pos to UL corner
} GFXglyph;

typedef struct { // Data stored for FONT AS A WHOLE:
	u8  *bitmap;      // Glyph bitmaps, concatenated
	GFXglyph *glyph;       // Glyph array
	u8  first, last; // ASCII extents
	u8  yAdvance;    // Newline distance (y axis)
} GFXfont;

extern const unsigned char font[];

#endif // _GFXFONT_H_
