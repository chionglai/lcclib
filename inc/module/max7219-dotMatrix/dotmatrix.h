/*
 * dotmatrix.h
 *
 *  Created on: 7 Jul 2016
 *      Author: chiong
 *
 *  This implementation is responsible for:
 *  1. Encoding/arranging/converting text to display data.
 *  2. Display any picture, if implemented.
 *
 *  It assumes that the dot matrix is cascaded to form a single line ONLY.
 *  For the dot matrix module that I have, the way to cascade it such that drawing text
 *  (using the existing font from my LCD module) is the easiest will be to arrange the
 *  cascaded MAX7219 dot matrix module as follows:
 *  1. DIN pin at the bottom, and DOUT pin at the top. Do NOT cascade the modules such that
 *     DIN and DOUT pin are next to each other. This may simplify hardware connection, but
 *     will complicate software.
 *  2. An illustration on connection below (VCC and GND not shown)
 *
 *  	            Module 2      |      Module 1     |      Module 0
 *             ===================|===================|===================
 *  	                          |                   |
 *  	         To Module 3 DIN  |  To Module 2 DIN  |  To Module 1 DIN
 *  	           ^    ^   ^     |    ^    ^   ^     |    ^    ^   ^
 *  	           |    |   |     |    |    |   |     |    |    |   |
 *       Top:     CLK, CS, DOUT   |   CLK, CS, DOUT   |   CLK, CS, DOUT
 *  	                          |                   |
 *       Bottom:  CLK, CS, DIN    |   CLK, CS, DIN    |   CLK, CS, DIN
 *                 ^    ^   ^     |    ^    ^   ^     |    ^    ^   ^
 *                 |    |   |     |    |    |   |     |    |    |   |
 *                From Module 1   |  From Module 0    |     From MCU
 *
 *  3. For this arrangement, the x, y coordinate of the LED is as follows.
 *     Although the modules are cascaded from right to left, the LED matrix
 *     is increasing from left to right. This is because the line[0] data
 *     is sent first in max7219_write() as opposed to line[N].
 *
 *       Top-left
 *        (0, 0) Line[0], Line[1], ..., Line[N]
 *     Bit[0]   +------------------------------>
 *     Bit[1]   |
 *     ...      |
 *              |
 *     Bit[7]   |
 *              v
 *
 */

#ifndef SRC_DOTMATRIX_H_
#define SRC_DOTMATRIX_H_

#include "util/status.h"

/* Constant for dot matrix font */
#define DOTMATRIX_FONT_WIDTH		(5)
#define DOTMATRIX_FONT_CHAR_MARGIN	(1)
#define DOTMATRIX_FONT_CHAR_WIDTH	(DOTMATRIX_FONT_WIDTH + DOTMATRIX_FONT_CHAR_MARGIN)

/* Data struct for dot matrix display. */
typedef struct dotMatrix_s dotMatrix_t;

/**
 * @brief Create a dot matrix instance.
 * @param[out] Pointer to contain the address of dot matrix instance created.
 * @param[in] numLine Number of cascaded dot matrix lines. E.g. for two cascaded
 * 		MAX7219, numLine = 2 * MAX7219_DIGIT_LENGTH. Refer to header doc on how
 * 		to connect the hardware.
 * @return Status code.
 */
int32_t dotMatrix_create(dotMatrix_t** ppSelf, uint32_t numLine);

/**
 * @brief Destroy a dot matrix instance and free its memory.
 * @param[in/out] ppSelf Dot matrix instance to be destroyed. Upon successful destruction,
 * 		*ppSelf will be NULL.
 * @return Status code.
 */
int32_t dotMatrix_destroy(dotMatrix_t** ppSelf);

/**
 * @brief Draw the given data to dot matrix buffer.
 * @param[in/out] pSelf Dot matrix instance.
 * @param[in] xOffset X offset of buffer to which data is to be drawn.
 * @param[in] data Data to be drawn to buffer.
 * @param[in] len Length of data.
 */
void dotMatrix_draw(dotMatrix_t* pSelf, int32_t xOffset, const uint8_t *data, uint32_t len);

/**
 * @brief Draw a char starting at x offset.
 * @details The x offset are based on the LED and start with 0 from the leftmost.
 * @param[in/out] pSelf Dot matrix instance.
 * @param[in] xOffset X-offset of the display buffer, not the actual dot matrix hardware.
 * @param[in] c Character to be drawn.
 */
void dotMatrix_drawChar(dotMatrix_t* pSelf, int32_t xOffset, char c);

/**
 * @brief Draw a text starting at x offset.
 * @details The x offset is based on the LED and start with 0 from the leftmost.
 * @param[in/out] pSelf Dot matrix instance.
 * @param[in] xOffset X-offset of the display buffer, not the actual dot matrix hardware.
 * @param[in] text String to be drawn. Must be NULL terminated.
 */
void dotMatrix_drawText(dotMatrix_t* pSelf, int32_t xOffset, const char* text);

/**
 * @brief Get part of dot matrix display data.
 * @details The output data can be sent to cascaded MAX7219 hardware dot matrix
 * 		for display
 * @param[in] pSelf Dot matrix instance.
 * @param[in] xOffset X offset of the dot matrix display data.
 * @param[in] len Length to be fetched.
 * @param[in] isWrap 1 for wrap around and 0 for blank insertion if end of line is reached.
 * @param[out] data Output buffer. Must have length of len.
 */
void dotMatrix_getDisplay(dotMatrix_t* pSelf, int32_t xOffset, uint32_t len, uint8_t isWrap, uint8_t *data);

/**
 * @brief Set the end line of dot matrix display buffer.
 * @details This numLine is used to decide for wrapping of blank insertion.
 * @param[in/out] pSelf Dot matrix instance.
 * @param[in] numLine New endLine. Will be clipped to the number of lines in
 * 		the buffer in pSelf.
 */
void dotMatrix_setNumLineToDisplay(dotMatrix_t* pSelf, uint32_t numLine);

/**
 * @brief Clear the display data.
 * @param[in/out] pSelf Dot matrix instance.
 */
void dotMatrix_clear(dotMatrix_t* pSelf);

/**
 * @brief Get the number of lines available for display.
 * @param[in] pSelf Dot matrix instance.
 * @return Number of lines available for display.
 */
uint32_t dotMatrix_getNumLineToDisplay(dotMatrix_t* pSelf);

/**
 * @brief Get the max number of lines supported by this dot matrix instance.
 * @param[in] pSelf Dot matrix instance.
 * @return Max number of lines supported.
 */
uint32_t dotMatrix_getMaxLine(dotMatrix_t* pSelf);

#endif /* SRC_DOTMATRIX_H_ */
