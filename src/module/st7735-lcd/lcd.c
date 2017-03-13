/*
 * lcd.c
 *
 *  Created on: 16 Mar 2016
 *      Author: lcc
 */

#include <stdlib.h>
#include <string.h>
#include "util/util.h"
#include "util/status.h"
#include "util/bit.h"
#include "module/st7735-lcd/gfxfont.h"
#include "module/st7735-lcd/lcd.h"
#include "module/st7735-lcd/st7735.h"

/* Internal Macros */
#ifndef pgm_read_byte
 #define pgm_read_byte(addr) (*(const unsigned char *)(addr))
#endif
#ifndef pgm_read_word
 #define pgm_read_word(addr) (*(const unsigned short *)(addr))
#endif
#ifndef pgm_read_dword
 #define pgm_read_dword(addr) (*(const unsigned long *)(addr))
#endif

// For 16-bit
//#define pgm_read_pointer(addr) ((void *)pgm_read_word(addr))

// For 32-bit
//#define pgm_read_pointer(addr) ((void *)pgm_read_dword(addr))

extern const unsigned char font[];

int32_t lcd_create(lcd_t **inst, st7735_t *st7735_inst) {
	lcd_t *pLcd;
	pLcd = (lcd_t*) malloc(sizeof(lcd_t));

	if (NULL == pLcd) {
		return STATUS_ERROR_MALLOC;
	}

	*inst = pLcd;
	pLcd->st7735_inst = st7735_inst;
	pLcd->gfxFont = NULL;
	pLcd->textColor = LCD_COLOR_BLACK;
	pLcd->bgColor = LCD_COLOR_WHITE;
	pLcd->isBgOpaque = 1;
	pLcd->textSize = 1;
	pLcd->isWrap = 1;
	if (lcd_setOrientation(pLcd, LCD_ORIENT_PORTRAIT_NORMAL) != STATUS_OK) {
		return STATUS_ERROR;
	}

	return STATUS_OK;
}


void lcd_destroy(lcd_t **inst) {
	lcd_t *p = *inst;
	if (NULL != p) {
		free(p);
		p = NULL;
	}
}


int32_t lcd_setOrientation(lcd_t *inst, uint8_t newOrient) {
	if (st7735_panel_setOrientation(inst->st7735_inst, newOrient) != ST7735_STATUS_OK) {
		return STATUS_ERROR;
	}
	inst->orientation = newOrient;

	if (BIT_isBitClr(newOrient, ST7735_PANEL_EXCHANGE_XY_pos)) {
	    /* Normal */
	    inst->width = ST7735_WIDTH;
	    inst->height = ST7735_HEIGHT;
	} else {
	    /* X-Y swapped */
	    inst->width = ST7735_HEIGHT;
	    inst->height = ST7735_WIDTH;
	}

	return STATUS_OK;
}


void lcd_setFont(lcd_t *inst, const GFXfont *newFont) {
	inst->gfxFont = newFont;
}


void lcd_setTextWrap(lcd_t *inst, uint8_t isWrap) {
	inst->isWrap = isWrap;
}


void lcd_setTextStroke(lcd_t *inst, uint32_t color, uint32_t bgColor, uint8_t isBgOpaque, uint8_t size) {
	inst->textColor = color;
	inst->textSize = size;
	inst->bgColor = bgColor;
	inst->isBgOpaque = isBgOpaque;
}


void lcd_drawPixel(lcd_t *inst, uint16_t x, uint16_t y, uint32_t color) {
	st7735_setAddress(inst->st7735_inst, x, y, x+1, y+1);
	st7735_panel_pushColor(inst->st7735_inst, color, 1);
}


void lcd_drawFastVLine(lcd_t *inst, uint16_t x, uint16_t y, uint16_t len, uint32_t color) {
	st7735_setAddress(inst->st7735_inst, x, y, x, y+len-1);
	st7735_panel_pushColor(inst->st7735_inst, color, len);
}


void lcd_drawFastHLine(lcd_t *inst, uint16_t x, uint16_t y, uint16_t len, uint32_t color) {
	st7735_setAddress(inst->st7735_inst, x, y, x+len-1, y);
	st7735_panel_pushColor(inst->st7735_inst, color, len);
}


void lcd_drawRect(lcd_t *inst, uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color) {
	lcd_drawFastHLine(inst, x, y, width, color);
	lcd_drawFastHLine(inst, x, y + height - 1, width, color);
	lcd_drawFastVLine(inst, x, y, height, color);
	lcd_drawFastVLine(inst, x + width - 1, y, height, color);
}


void lcd_fillRect(lcd_t *inst, uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color) {
	st7735_setAddress(inst->st7735_inst, x, y, x+width-1, y+height-1);
	st7735_panel_pushColor(inst->st7735_inst, color, width * height);
}


void lcd_drawCircle(lcd_t *inst, uint16_t x0, uint16_t y0, uint16_t r, uint32_t color) {
	int16_t f = 1 - r;
	int16_t ddF_x = 1;
	int16_t ddF_y = -2 * r;
	int16_t x = 0;
	int16_t y = r;

	lcd_drawPixel(inst, x0  , y0+r, color);
	lcd_drawPixel(inst, x0  , y0-r, color);
	lcd_drawPixel(inst, x0+r, y0  , color);
	lcd_drawPixel(inst, x0-r, y0  , color);

	while (x<y) {
		if (f >= 0) {
		  y--;
		  ddF_y += 2;
		  f += ddF_y;
		}
		x++;
		ddF_x += 2;
		f += ddF_x;

		lcd_drawPixel(inst, x0 + x, y0 + y, color);
		lcd_drawPixel(inst, x0 - x, y0 + y, color);
		lcd_drawPixel(inst, x0 + x, y0 - y, color);
		lcd_drawPixel(inst, x0 - x, y0 - y, color);
		lcd_drawPixel(inst, x0 + y, y0 + x, color);
		lcd_drawPixel(inst, x0 - y, y0 + x, color);
		lcd_drawPixel(inst, x0 + y, y0 - x, color);
		lcd_drawPixel(inst, x0 - y, y0 - x, color);
	}
}


void lcd_drawLine(lcd_t *inst, uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint32_t color) {
	int16_t steep = abs(y1 - y0) > abs(x1 - x0);

	if (steep) {
		UTIL_swapInt(x0, y0);
		UTIL_swapInt(x1, y1);
	}

	if (x0 > x1) {
		UTIL_swapInt(x0, x1);
		UTIL_swapInt(y0, y1);
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
			lcd_drawPixel(inst, y0, x0, color);
		} else {
			lcd_drawPixel(inst, x0, y0, color);
		}
	err -= dy;
		if (err < 0) {
			y0 += ystep;
			err += dx;
		}
	}
}


void lcd_drawPolygon(lcd_t *inst, const uint16_t* x, const uint16_t* y, uint32_t count, uint32_t color) {
    uint32_t i;
    for (i = 0; i < (count - 1); i++) {
        lcd_drawLine(inst, x[i], y[i], x[i+1], y[i+1], color);
    }

    lcd_drawLine(inst, x[i], y[i], x[0], y[0], color);
}


void lcd_drawChar(lcd_t *inst, uint16_t x, uint16_t y, char c) {
	uint8_t i, j;

	if (NULL == inst->gfxFont) {
		/* 'Classic' built-in font */
		if((x >= inst->width)       || // Clip right
		   (y >= inst->height)      || // Clip bottom
		   ((x + 6 * inst->textSize - 1) < 0) || // Clip left
		   ((y + 8 * inst->textSize - 1) < 0))   // Clip top
			return;

//		if(!_cp437 && (c >= 176)) c++; // Handle 'classic' charset behavior

		for (i = 0; i < 6; i++) {
			uint8_t line;
			if (i < 5)
				line = pgm_read_byte(font + (c*5)+i);
			else
				line = 0x0;
			for (j = 0; j < 8; j++, line >>= 1) {
				if (line & 0x1) {
					if(inst->textSize == 1)
						lcd_drawPixel(inst, x+i, y+j, inst->textColor);
					else
						lcd_fillRect(inst, x+(i*inst->textSize), y+(j*inst->textSize), inst->textSize, inst->textSize, inst->textColor);
				} else if(inst->isBgOpaque && (inst->bgColor != inst->textColor)) {
					if (inst->textSize == 1)
						lcd_drawPixel(inst, x+i, y+j, inst->bgColor);
					else
						lcd_fillRect(inst, x+i*inst->textSize, y+j*inst->textSize, inst->textSize, inst->textSize, inst->bgColor);
				}
			}
		}

	} else { // Custom font
		// Character is assumed previously filtered by write() to eliminate
		// newlines, returns, non-printable characters, etc.  Calling drawChar()
		// directly with 'bad' characters of font may cause mayhem!

		c -= pgm_read_byte(&inst->gfxFont->first);
		GFXglyph *glyph  = &(((GFXglyph *) (&inst->gfxFont->glyph))[(uint8_t)c]);
		uint8_t  *bitmap = (uint8_t *) (&inst->gfxFont->bitmap);

		uint16_t bo = pgm_read_word(&glyph->bitmapOffset);
		uint8_t  w  = pgm_read_byte(&glyph->width),
			 h  = pgm_read_byte(&glyph->height);
//			 xa = pgm_read_byte(&glyph->xAdvance);
		int8_t   xo = pgm_read_byte(&glyph->xOffset),
			 yo = pgm_read_byte(&glyph->yOffset);
		uint8_t  xx, yy, bits, bit;
		int16_t  xo16, yo16;

		bits = bit = xo16 = yo16 = 0;
		if(inst->textSize > 1) {
			xo16 = xo;
			yo16 = yo;
		}

		// Todo: Add character clipping here

		// NOTE: THERE IS NO 'BACKGROUND' COLOR OPTION ON CUSTOM FONTS.
		// THIS IS ON PURPOSE AND BY DESIGN.  The background color feature
		// has typically been used with the 'classic' font to overwrite old
		// screen contents with new data.  This ONLY works because the
		// characters are a uniform size; it's not a sensible thing to do with
		// proportionally-spaced fonts with glyphs of varying sizes (and that
		// may overlap).  To replace previously-drawn text when using a custom
		// font, use the getTextBounds() function to determine the smallest
		// rectangle encompassing a string, erase the area with fillRect(),
		// then draw new text.  This WILL infortunately 'blink' the text, but
		// is unavoidable.  Drawing 'background' pixels will NOT fix this,
		// only creates a new set of problems.  Have an idea to work around
		// this (a canvas object type for MCUs that can afford the RAM and
		// displays supporting setAddrWindow() and pushColors()), but haven't
		// implemented this yet.

		for(yy=0; yy<h; yy++) {
			for(xx=0; xx<w; xx++) {
				if(!(bit++ & 7)) {
					bits = pgm_read_byte(&bitmap[bo++]);
				}
				if(bits & 0x80) {
					if(inst->textSize == 1) {
						lcd_drawPixel(inst, x+xo+xx, y+yo+yy, inst->textColor);
					} else {
						lcd_fillRect(inst, x+(xo16+xx)*inst->textSize, y+(yo16+yy)*inst->textSize, inst->textSize, inst->textSize, inst->textColor);
					}
				}
				bits <<= 1;
			}
		}

	} // End classic vs custom font
}


uint32_t lcd_writeText(lcd_t *inst, uint16_t x, uint16_t y, char* str, int32_t count) {
	uint32_t numChar = 0;
	char c;
	uint16_t cursor_x, cursor_y;
	const GFXfont *gfxFont;

	cursor_x = x;
	cursor_y = y;
	if (NULL == inst->gfxFont) { // 'Classic' built-in font
		c = *str++;
		while (c != '\0' && !(count > 0 && numChar >= count)) {
			if (c == '\n') {
				cursor_y += inst->textSize * 8;
				cursor_x  = x;
			} else if(c == '\r') {
				// skip em
			} else {
			  if(inst->isWrap && ((cursor_x + inst->textSize * 6) > inst->width)) { // Heading off edge?
				  cursor_x  = x;            // Reset x to zero
				  cursor_y += inst->textSize * 8; // Advance y one line
			  }
			  lcd_drawChar(inst, cursor_x, cursor_y, c);
			  cursor_x += inst->textSize * 6;
			}

			numChar++;
			c = *str++;
		}

	} else { // Custom font
		gfxFont = inst->gfxFont;
		cursor_y += (int16_t)inst->textSize *
					(uint8_t)pgm_read_byte(&gfxFont->yAdvance);
		c = *str++;
		while (c != '\0' && !(count > 0 && numChar >= count)) {
			if(c == '\n') {
				cursor_x = x;
				cursor_y += (int16_t)inst->textSize *
							(uint8_t)pgm_read_byte(&gfxFont->yAdvance);
			} else if (c != '\r') {
				uint8_t first = pgm_read_byte(&gfxFont->first);
				if ((c >= first) && (c <= (uint8_t)pgm_read_byte(&gfxFont->last))) {
					uint8_t   c2    = c - pgm_read_byte(&gfxFont->first);
					GFXglyph *glyph = &(((GFXglyph *)(&gfxFont->glyph))[c2]);
					uint8_t   w     = pgm_read_byte(&glyph->width),
							  h     = pgm_read_byte(&glyph->height);
					if ((w > 0) && (h > 0)) { // Is there an associated bitmap?
						int16_t xo = (int8_t)pgm_read_byte(&glyph->xOffset); // sic
						if (inst->isWrap && ((cursor_x + inst->textSize * (xo + w)) > inst->width)) {
							// Drawing character would go off right edge; wrap to new line
							cursor_x  = x;
							cursor_y += (int16_t)inst->textSize *
							(uint8_t)pgm_read_byte(&gfxFont->yAdvance);
						}
						lcd_drawChar(inst, cursor_x, cursor_y, c);
					}
					cursor_x += pgm_read_byte(&glyph->xAdvance) * (int16_t)inst->textSize;
				}
			}
			numChar++;
			c = *str++;
		}
	}

	return numChar;
}


uint32_t lcd_writeTextInCanvas(lcd_t *inst, int16_t x, int16_t y, int16_t w, int16_t h, lcd_layout_t layout, char* str, int32_t count) {
	uint16_t textWidth, textHeight;
	int16_t textX, textY;
	int16_t h1, h2, w1, w2;
	int16_t blankWidth;
	uint32_t numChar;

	if (w < 0) {
		w = inst->width - x;
	}

	if (h < 0) {
		h = inst->height - y;
	}

	/* Value returned by textX and textY are not used. They will be set later, based on
	 * the text layout */
	lcd_getTextBound(inst, x, y, str, count, &textX, &textY, &textWidth, &textHeight);

	/* Working out the region to be redrawn/cleared and X-coordinate of text based on
	 * horizontal layout */
	switch (layout & LCD_HOR_LAYOUT_MASK) {
	case LCD_LAYOUT_HOR_CENTER:
		w1 = (w - textWidth) >> 1;
		/* Calculate w2 from w1 instead of w2 = w1 to avoid integer truncation. */
		w2 = w - textWidth - w1;
		break;
	case LCD_LAYOUT_HOR_RIGHT:
		w1 = (w - textWidth);
		w2 = 0;
		break;
	default:
		/* default Left */
		w1 = 0;
		w2 = (w - textWidth);
		break;
	}
	textX = w1 > 0? x + w1 : x;

	/* Working out the region to be redrawn/cleared and Y-coordinate of text based on
	 * vertical layout */
	switch (layout & LCD_VER_LAYOUT_MASK) {
	case LCD_LAYOUT_VER_CENTER:
		h1 = (h - textHeight) >> 1;
		/* Calculate h2 from h1 instead of h2 = h1 to avoid integer truncation. */
		h2 = h - textHeight - h1;
		break;
	case LCD_LAYOUT_VER_BOTTOM:
		h1 = (h - textHeight);
		h2 = 0;
		break;
	default:
		/* default Top */
		h1 = 0;
		h2 = (h - textHeight);
		break;
	}
	textY = h1 > 0? y + h1 : y;

	if (inst->gfxFont) {
		/* For custom font */
		/* Because custom font renders differently from the default fixed size font,
		 * opaque background does not work out as expected. One way is to use:
		 * 1. getTextBound to get the rectangular area for the text
		 * 2. fillRect to fill the entire rectangular area
		 * 3. write text as normal. */
		if (inst->isBgOpaque && (inst->bgColor != inst->textColor)) {
			lcd_fillRect(inst, x, y, w > textWidth? w : textWidth, h > textHeight? h : textHeight, inst->bgColor);
		}
		/* draw text as usual */
		numChar = lcd_writeText(inst, textX, textY, str, count);
	} else {
		/* For default font */
		blankWidth = w > textWidth? w : textWidth;

		if (inst->isBgOpaque && (h1 > 0)) {
			lcd_fillRect(inst, x, y, blankWidth, h1, inst->bgColor);
		}

		if (inst->isBgOpaque && (w1 > 0)) {
			lcd_fillRect(inst, x, textY, w1, textHeight, inst->bgColor);
		}

		/* draw text as usual */
		numChar = lcd_writeText(inst, textX, textY, str, count);

		if (inst->isBgOpaque && (w2 > 0)) {
			/* Need to add w1 if it is positive */
			textWidth += w1 > 0? w1 : 0;
			lcd_fillRect(inst, x + textWidth, textY, w2, textHeight, inst->bgColor);
		}

		if (inst->isBgOpaque && (h2 > 0)) {
			/* Need to add w1 if it is positive */
			textHeight += h1 > 0? h1 : 0;
			lcd_fillRect(inst, x, y + textHeight, blankWidth, h2, inst->bgColor);
		}
	}

	return numChar;
}


uint32_t lcd_getTextBound(lcd_t *inst, uint16_t x, uint16_t y, char* str, int32_t count, int16_t *xOut, int16_t *yOut, uint16_t *w, uint16_t *h) {
	// Pass string and a cursor position, returns UL corner and W,H.
	uint8_t c; // Current character
	uint32_t numChar;
	const GFXfont *gfxFont;
	int16_t x1, y1;

	numChar = 0;
	x1 = x;
	y1 = y;
	*w  = *h = 0;

	gfxFont = inst->gfxFont;

	if (gfxFont) {
	    GFXglyph *glyph;
	    uint8_t   first = pgm_read_byte(&gfxFont->first),
	              last  = pgm_read_byte(&gfxFont->last),
	              gw, gh, xa;
	    int8_t    xo, yo;
	    int16_t   minx = inst->width, miny = inst->height, maxx = -1, maxy = -1,
	              gx1, gy1, gx2, gy2, ts = (int16_t)inst->textSize,
	              ya = ts * (uint8_t)pgm_read_byte(&gfxFont->yAdvance);

	    while ((c = *str++) && !(count > 0 && numChar >= count)) {
	    	if (c != '\n') { 	// Not a newline
	    		if (  (c != '\r')  	// Not a carriage return, is normal char
	    		   && (c >= first) && (c <= last)	// Char present in current font
				   ) {
					c    -= first;
					glyph = &(((GFXglyph *)(&gfxFont->glyph))[c]);
					gw    = pgm_read_byte(&glyph->width);
					gh    = pgm_read_byte(&glyph->height);
					xa    = pgm_read_byte(&glyph->xAdvance);
					xo    = pgm_read_byte(&glyph->xOffset);
					yo    = pgm_read_byte(&glyph->yOffset);
					if (inst->isWrap && ((x + (((int16_t)xo + gw) * ts)) >= inst->width)) {
						// Line wrap
						x  = 0;  // Reset x to 0
						y += ya; // Advance y by 1 line
					}
					gx1 = x   + xo * ts;
					gy1 = y   + yo * ts;
					gx2 = gx1 + gw * ts - 1;
					gy2 = gy1 + gh * ts - 1;
					if (gx1 < minx) minx = gx1;
					if (gy1 < miny) miny = gy1;
					if (gx2 > maxx) maxx = gx2;
					if (gy2 > maxy) maxy = gy2;
					x += xa * ts;
				}
	    	} else { // Newline
	    		x  = 0;  // Reset x
	    		y += ya; // Advance y by 1 line
	    	}

	    	numChar++;
	    }
	    // End of string
	    x1 = minx;
	    y1 = miny;
	    if(maxx >= minx) *w  = maxx - minx + 1;
	    if(maxy >= miny) *h  = maxy - miny + 1;

	} else { // Default font
	    uint16_t lineWidth = 0, maxWidth = 0; // Width of current, all lines

	    while ((c = *str++) && !(count > 0 && numChar >= count)) {
			if(c != '\n') { // Not a newline
				if(c != '\r') { // Not a carriage return, is normal char
					if(inst->isWrap && ((x + inst->textSize * 6) >= inst->width)) {
						x  = 0;            // Reset x to 0
						y += inst->textSize * 8; // Advance y by 1 line
						if (lineWidth > maxWidth) maxWidth = lineWidth; // Save widest line
						lineWidth  = inst->textSize * 6; // First char on new line
					} else { // No line wrap, just keep incrementing X
						lineWidth += inst->textSize * 6; // Includes interchar x gap
					}
				} // Carriage return = do nothing
			} else { // Newline
				x  = 0;            // Reset x to 0
				y += inst->textSize * 8; // Advance y by 1 line
				if(lineWidth > maxWidth) maxWidth = lineWidth; // Save widest line
				lineWidth = 0;     // Reset lineWidth for new line
			}

			numChar++;
	    }
	    // End of string
	    if (lineWidth) y += inst->textSize * 8; // Add height of last (or only) line
	    if (lineWidth > maxWidth) maxWidth = lineWidth;
	    *w = maxWidth;               // Still include last interchar x gap
	    *h = y - y1;
	} // End classic vs custom font

	if (xOut != NULL) *xOut = x1;
	if (yOut != NULL) *yOut = y1;

	return numChar;
}


void lcd_drawBitmap(lcd_t *inst, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint32_t* color) {
	st7735_setAddress(inst->st7735_inst, x, y, x+w-1, y+h-1);
	st7735_panel_pushColorArray(inst->st7735_inst, color, w * h);
}
