/*
 * lcd.h
 *
 *  Created on: 15 Mar 2016
 *      Author: lcc
 *
 *  Reference:
 *  1. Source code for Adafruit_GFX.cpp
 */

#ifndef __LCD_H_INCLUDED
#define __LCD_H_INCLUDED

#include <stdint.h>
#include "module/st7735-lcd/st7735.h"
#include "module/st7735-lcd/gfxfont.h"

#define LCD_HOR_LAYOUT_MASK  0x0F
#define LCD_VER_LAYOUT_MASK  0xF0

typedef enum {
    LCD_LAYOUT_HOR_LEFT     = 0x00,
    LCD_LAYOUT_HOR_CENTER   = 0x01,
    LCD_LAYOUT_HOR_RIGHT    = 0x02,
    LCD_LAYOUT_VER_TOP      = 0x00,
    LCD_LAYOUT_VER_CENTER   = 0x10,
    LCD_LAYOUT_VER_BOTTOM   = 0x20
} lcd_layout_t;


#define LCD_COLOR_BLACK		(0x000000)
#define LCD_COLOR_WHITE		(0xFFFFFF)
#define LCD_COLOR_RED		(0xFF0000)
#define LCD_COLOR_GREEN		(0x00FF00)
#define LCD_COLOR_BLUE		(0x0000FF)
#define LCD_COLOR_CYAN		(0x00FFFF)
#define LCD_COLOR_PURPLE	(0xFF00FF)
#define LCD_COLOR_YELLOW	(0xFFFF00)


/* Portrait, with the cable/pin end as bottom. */
#define LCD_ORIENT_PORTRAIT_NORMAL      \
    ((1u << ST7735_PANEL_MIRROR_Y_pos) | (1u << ST7735_PANEL_MIRROR_X_pos))
/* Portrait, with cable/pin end as top. */
#define LCD_ORIENT_PORTRAIT_INVERTED    (0x00)

/* Lanscape, with the text on PCB as bottom. */
#define LCD_ORIENT_LANDSCAPE_NORMAL     \
    ((1u << ST7735_PANEL_EXCHANGE_XY_pos) | (1u << ST7735_PANEL_MIRROR_X_pos))
/* Lanscape, with the text on PCB as top. */
#define LCD_ORIENT_LANDSCAPE_INVERTED   \
    ((1u << ST7735_PANEL_EXCHANGE_XY_pos) | (1u << ST7735_PANEL_MIRROR_Y_pos))

/**
 * @brief Macro to write string to screen with default text color,
 * 		background color and text size.
 * @param[in] inst LCD instance.
 * @param[in] x Start x-coordinate.
 * @param[in] y Start y-coordinate.
 * @param[in] str String to be written, must be NULL terminated.
 */
#define LCD_write(inst, x, y, str)		lcd_writeText(inst, x, y, str, -1)

/**
 * @brief Macro for conveniently using the lcd_writeTextInCanvas().
 */
#define LCD_writeTextInCanvas(inst, x, y, w, h, layout, str)	\
	lcd_writeTextInCanvas(inst, x, y, w, h, layout, str, -1)

/**
 * @brief Macro to write a text in a canvas with width equals to LCD width.
 * @param[in] inst LCD instance.
 * @param[in] y Y offset of canvas.
 * @param[in] h Height of the canvas.
 * @param[in] layout Text layout in the canvas.
 * @param[in] str String to be drawn. Must be NULL terminated.
 */
#define LCD_writeTextInCanvasSpanX(inst, y, h, layout, str)		\
		lcd_writeTextInCanvas(inst, 0, y, -1, h, layout, str, -1)

/**
 * @brief Macro to previously written string from screen.
 * @details This will only work if:
 * 		1. x and y position are the same as previously written to screen.
 * 		2. Background color matches the current background color on screen.
 * 		3. Same font and text size as previously written to screen.
 * @param[in] inst LCD instance.
 * @param[in] x Start x-coordinate.
 * @param[in] y Start y-coordinate.
 * @param[in] str String to be written, must be NULL terminated.
 */
#define LCD_clear(inst, x, y, str)		lcd_writeText(inst, x, y, str, -1)

/**
 * @brief Macro to fill up the entire screen with a solid color.
 * @param[in] inst LCD instance.
 * @param[in] color 24-bit RGB color to be filled.
 */
#define LCD_fillScreen(inst, color)		lcd_fillRect(inst, 0, 0, (inst)->width, (inst)->height, color)

/**
 * @brief Macro to get the rectangular text bound without (x, y) coordinate.
 * @param[in] inst LCD instance.
 * @param[in] str String to be written, must be NULL terminated.
 * @param[out] w Width of the text.
 * @param[out] h Height of the text.
 */
#define LCD_getTextBound(inst, str, w, h)	\
	lcd_getTextBound(inst, 0, 0, str, -1, NULL, NULL, w, h)

typedef struct {
	/** Instance containing hardware pin to use */
	st7735_t *st7735_inst;
	/** Width, in number of pixels, of screen */
    uint32_t width;
    /** Height, in number of pixels, of screen */
    uint32_t height;
    /** Screen orientation */
    uint8_t orientation;

    /** Custom font for drawing text */
    const GFXfont *gfxFont;
    /** Text color */
    uint32_t textColor;
    /** Background color for drawing text. Something like text highlight. */
    uint32_t bgColor;
    /** Flag to determine if background colour needs to be drawn when drawing chars. */
    uint8_t isBgOpaque;
    /** Text size */
    uint8_t textSize;
    /** Flag to wrap char to new line if reach end of screen */
    uint8_t isWrap;
} lcd_t;


#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialise LCD screen with filled color.
 * @param[out] inst Pointer to LCD instance handler.
 * @param[in] st7735_inst A ST7735 instance to be associated to the LCD instance.
 * 		The LCD instance will point to this st7735 instance and will not make
 * 		a copy for itself.
 * @returns Status code.
 */
int32_t lcd_create(lcd_t **inst, st7735_t *st7735_inst);


/**
 * @brief Close and free LCD instance.
 * @param[in] inst LCD instance to be closed. Once closed, inst == NULL and can
 * 		no longer be used.
 */
void lcd_destroy(lcd_t **inst);


/**
 * @brief Set the orientation of screen.
 * @param[in] inst LCD instance.
 * @param[in] newOrient New orientation.
 * @returns status code.
 */
int32_t lcd_setOrientation(lcd_t *inst, uint8_t newOrient);


/**
 * @brief Set text wrap.
 * @param[in] inst LCD instance.
 * @param[in] isWrap Non-zero for text wrap, zero otherwise.
 */
void lcd_setTextWrap(lcd_t *inst, uint8_t isWrap);


/**
 * @brief Set custom font for text rendering.
 * @param[in] inst LCD instance.
 * @param[in] newFont Pointer to custom font to use.
 */
void lcd_setFont(lcd_t *inst, const GFXfont *newFont);


/**
 * @brief Set the default text color, background color and text size.
 * @param[in] color 24-bit RGB text color.
 * @param[in] bgColor 24-bit RGB background color.
 * @param[in] isBgOpaque Flag to decide to draw background color (opaque) or not (transparent).
 * @param[in] size Text size.
 */
void lcd_setTextStroke(lcd_t *inst, uint32_t color, uint32_t bgColor, uint8_t isBgOpaque, uint8_t size);


/**
 * @brief Draw a single pixel on screen.
 * @param[in] inst LCD instance.
 * @param[in] x Address x
 * @param[in] y Address y
 * @param[in] color 24-bit RGB color.
 */
void lcd_drawPixel(lcd_t *inst, uint16_t x, uint16_t y, uint32_t color);


/**
 * @brief Draw a vertical line on screen.
 * @param[in] inst LCD instance.
 * @param[in] x Start address x
 * @param[in] y Start address y
 * @param[in] len Length of line in number of pixels.
 * @param[in] color 24-bit RGB color.
 */
void lcd_drawFastVLine(lcd_t *inst, uint16_t x, uint16_t y, uint16_t len, uint32_t color);


/**
 * @brief Draw a horizontal line on screen.
 * @param[in] inst LCD instance.
 * @param[in] x Start address x
 * @param[in] y Start address y
 * @param[in] len Length of line in number of pixels.
 * @param[in] color 24-bit RGB color.
 */
void lcd_drawFastHLine(lcd_t *inst, uint16_t x, uint16_t y, uint16_t len, uint32_t color);


/**
 * @brief Draw a line from (x0, y0) to (x1, y1).
 * @param[in] inst LCD instance.
 * @param[in] x0 Start x address
 * @param[in] y0 Start y address
 * @param[in] x1 End x address
 * @param[in] y1 End y address
 * @param[in] color 24-bit RGB color.
 */
void lcd_drawLine(lcd_t *inst, uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint32_t color);


/**
 * @brief Draw a generic polygon with given vertices (x0, y0), (x1, y1), ..., (xn, yn).
 * @param[in] inst LCD instance.
 * @param[in] x Array containing the x-coordinates for all vertices of a polygon.
 * @param[in] y Array containing the y-coordinates for all vertices of a polygon.
 * @param[in] count Number of vertices, i.e. length of array x and y.
 * @param[in] color 24-bit RGB color.
 */
void lcd_drawPolygon(lcd_t *inst, const uint16_t* x, const uint16_t* y, uint32_t count, uint32_t color);


/**
 * @brief Draw a circle with given radius.
 * @param[in] inst LCD instance.
 * @param[in] x0 X-coordinate of the center of the circle.
 * @param[in] y0 Y-coordinate of the center of the circle.
 * @param[in] color 24-bit RGB color.
 */
void lcd_drawCircle(lcd_t *inst, uint16_t x0, uint16_t y0, uint16_t r, uint32_t color);


/**
 * @brief Draw a rectangle.
 * @param[in] inst LCD instance.
 * @param[in] x X-coordinate of the 'start' vertex of the rectangle.
 * @param[in] y Y-coordinate of the 'start' vertex of the rectangle.
 * @param[in] width Width of rectangle, in number of pixels.
 * @param[in] height Height of rectangle, in number of pixels.
 * @param[in] color 24-bit RGB color.
 */
void lcd_drawRect(lcd_t *inst, uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color);


/**
 * @brief Fill a rectangular area on screen.
 * @param[in] inst LCD instance.
 * @param[in] x Start address x
 * @param[in] y Start address y
 * @param[in] width Width of rectangular area, in number of pixels
 * @param[in] height Height of rectangular area, in number of pixels
 * @param[in] color 24-bit RGB color.
 */
void lcd_fillRect(lcd_t *inst, uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color);


/**
 * @brief Draw a single char on screen.
 * @param[in] inst LCD instance.
 * @param[in] x Starting x-coordinate.
 * @param[in] y Starting y-coordinate.
 * @param[in] c Character to draw on screen.
 */
void lcd_drawChar(lcd_t *inst, uint16_t x, uint16_t y, char c);


/**
 * @brief Write a string to screen.
 * @param[in] inst LCD instance.
 * @param[in] x Starting x-coordinate.
 * @param[in] y Starting y-coordinate.
 * @param[in] str String to write to screen. Must be NULL terminated.
 * @param[in] count Number of char to write to screen. Ignored if
 * 		str is shorter than count. E.g. str = "hello\0", count == 10,
 * 		will only write "hello" to screen. Specify negative count to
 * 		have the whole string printed to screen, i.e. printing is
 * 		terminated by NULL delimiter.
 * @returns Number of char written to screen, including newlines, carriage return,
 * 		but excluding NULL delimiter.
 */
uint32_t lcd_writeText(
		lcd_t *inst,
		uint16_t x,
		uint16_t y,
		char* str,
		int32_t count);

/**
 * @brief Write a string in a rectangular canvas.
 * @details Currently only support a single line text. Multi-line text
 * 		will not be drawn correctly. The supported layout for a given canvas
 * 		with sufficient size is as follows.
 *
 * 		+---------------+------------------+---------------+
 * 		| Top, Left     | Top, HCenter     | Top, Right    |
 * 		+---------------+------------------+---------------+
 * 		| VCenter, Left | VCenter, HCenter | VCenter,Right |
 * 		+---------------+------------------+---------------+
 * 		| Bottom, Left  | Bottom, HCenter  | Bottom, Right |
 * 		+---------------+------------------+---------------+
 *
 * @param[in] inst LCD instance.
 * @param[in] x Starting x-coordinate of the canvas.
 * @param[in] y Starting y-coordinate of the canvas.
 * @param[in] w Width of the canvas. -1 for width equals to LCD instance's width.
 * 		If the width is less than the width required to draw the text, the width
 * 		is ignored and the width required to draw the text is used instead.
 * @param[in] h Height of the canvas. -1 for width equals to LCD instance's height.
 * 		If the height is less than the height required to draw the text, the height
 * 		is ignored and the height required to draw the text is used instead.
 * @param[in] layout Text layout in the canvas.
 * @param[in] str String to write to screen. Must be NULL terminated.
 * @param[in] count Number of char to write to screen. Ignored if
 * 		str is shorter than count. E.g. str = "hello\0", count == 10,
 * 		will only write "hello" to screen. Specify negative count to
 * 		have the whole string printed to screen, i.e. printing is
 * 		terminated by NULL delimiter.
 * @returns Number of char written to screen, including newlines, carriage return,
 * 		but excluding NULL delimiter.
 */
uint32_t lcd_writeTextInCanvas(
		lcd_t *inst,
		int16_t x,
		int16_t y,
		int16_t w,
		int16_t h,
		lcd_layout_t layout,
		char* str,
		int32_t count);

/**
 * @brief Get the rectangular bound for a given text.
 * @details This function will return the upper left corner (xo, yo) and
 * 		the width and height. These are affected by the input argument
 * 		(x, y) and isWrap parameter.
 * @param[in] inst LCD instance.
 * @param[in] x X coordinate where the text is intended to be drawn.
 * @param[in] y Y coordinate where the text is intended to be drawn.
 * @param[in] str Text to be calculated. Must be NULL terminated.
 * @param[in] count Number of char to write to screen. Ignored if
 * 		str is shorter than count. E.g. str = "hello\0", count == 10,
 * 		will only calculate for "hello". Specify negative count to
 * 		have the whole string calculated till NULL delimiter.
 * @param[out] xo Output of upper left corner x coordinate.
 * @param[out] yo Output of upper left corner y coordinate.
 * @param[out] w Total width required.
 * @param[out] h Total height required.
 *
 */
uint32_t lcd_getTextBound(lcd_t *inst, uint16_t x, uint16_t y, char* str, int32_t count, int16_t *xOut, int16_t *yOut, uint16_t *w, uint16_t *h);


/**
 * @brief Draw an array of 24-bit RGB color to screen.
 * @param[in] inst LCD instance.
 * @param[in] x Start x-coordinate.
 * @param[in] y Start y-coordinate.
 * @param[in] w Width of screen real estate to draw.
 * @param[in] h Height of screen real estate to draw.
 * @param[in] color Array of 24-bit RGB color. Must have length == w * h.
 */
void lcd_drawBitmap(lcd_t *inst, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint32_t* color);


#ifdef __cplusplus
}
#endif

#endif /* __LCD_H_INCLUDED */

