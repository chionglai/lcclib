/*
 * anim.h
 *
 *  Created on: 12 Jul 2016
 *      Author: chiong
 */

#ifndef SRC_ANIM_H_
#define SRC_ANIM_H_

#include <stdint.h>
#include "portable/fxn.h"
#include "module/max7219-dotMatrix/dotmatrix.h"
#include "module/max7219-dotMatrix/max7219.h"

typedef enum {
	TEXT_LAYOUT_LEFT = 0,
	TEXT_LAYOUT_RIGHT,
	TEXT_LAYOUT_CENTER
} text_layout_t;

typedef struct {
	/* Delay function, in ms. */
	fxnDelay_t delayFxn;
	/* Wait time, in ms before each scroll step. */
	uint32_t waitTimeMs;
	/* Number of LED jumps per scroll step. Positive for left scrolling,
	 * negative for right scrolling.
	 */
	int32_t stepSize;
} anim_scroll_t;

typedef struct {
	/* Delay function, in ms. */
	fxnDelay_t delayFxn;
	/* Both onTimeMs and offTimeMs must have same length. All onTimeMs, offTimeMs and waitMs
	 * can be used to generate different kind of blink patterns. E.g.
	 * 		onTimeMs : 200, 200,
	 * 		offTimeMs: 500, 500,
	 * 		waitMs   : 1000
	 * will generate a heart-beat like blink pattern.
	 */
	/* Array of text on times in ms. */
	uint32_t *onTimeMs;
	/* Array of text off times in ms. */
	uint32_t *offTimeMs;
	/* Wait time before start of next cycle. */
	uint32_t waitTimeMs;
	/* Length of both onTimeMs and offTimeMs. */
	uint32_t len;
	/* Layout for the text to be displayed. */
	text_layout_t layout;
} anim_blink_t;

/**
 * @brief Data struct for inverting text cycle.
 * @details It has the same struct as anim_blink_t. offTimeMs is used as the time for the display
 * 		in inverted mode.
 */
typedef anim_blink_t anim_invert_t;

typedef struct {
	/* Delay function, in ms. */
	fxnDelay_t delayFxn;
	/* Time in ms to display each sub-text. */
	uint32_t *timeMs;
	/* Array to represent a particular sub-text to be inverted or not. E.g.
	 * 		isInvert: 0, 0, 1, 0
	 * means invert sub-text 2 and display normal sub-text 0, 1 and 3. If
	 * NULL, all sub-text is displayed as normal.
	 */
	uint8_t *isInvert;
	/* Length of both timeMs and isInvert (if not NULL). */
	uint32_t len;
	/* Layout for the text to be displayed. */
	text_layout_t layout;
} anim_change_t;


/**
 * @brief Animate horizontal scrolling of text.
 * @details This function will only return once the whole text has been fully scrolled. The scrolling
 * 		starts with the first character entering from the display edge (either from left or right edge,
 * 		depending on the scrolling direction) and ends when the last character has fully exited
 * 		the display edge.
 * @param[in] pAnim Scroll animation instance containing the scrolling information.
 * @param[in] pDotMatrix Dot matrix instance containing buffer for encoded text for display.
 * @param[in] pMax7219 Cascaded hardware MAX7219 instance.
 * @param[in] text Text to be scrolled. NULL to display whatever is in pDotMatrix. Can be used for
 * 		continuous animation.
 */
void anim_hScrollText(anim_scroll_t *pAnim, dotMatrix_t *pDotMatrix, max7219_t *pMax7219, const char *text);

/**
 * @brief Animate blinking of text.
 * @details This function will only return once the whole blinking cycle has completed.
 * @param[in] pAnim Blink animation instance containing the blinking pattern.
 * @param[in] pDotMatrix Dot matrix instance containing buffer for encoded text for display.
 * @param[in] pMax7219 Cascaded hardware MAX7219 instance.
 * @param[in] text The text to be blinked. The text length is determined by the number of lines
 * 		available for display. Long text will truncated to fit into the display window. NULL to
 * 		display whatever is in pDotMatrix. Can be used for continuous animation.
 */
void anim_blinkText(anim_blink_t *pAnim, dotMatrix_t *pDotMatrix, max7219_t *pMax7219, const char *text);

/**
 * @brief Animate inverting of text.
 * @details This function will only return once the whole inverting cycle has completed.
 * @param[in] pAnim Inverting animation instance containing the blinking pattern.
 * @param[in] pDotMatrix Dot matrix instance containing buffer for encoded text for display.
 * @param[in] pMax7219 Cascaded hardware MAX7219 instance.
 * @param[in] text The text to be blinked. The text length is determined by the number of lines
 * 		available for display. Long text will truncated to fit into the display window. NULL to
 * 		display whatever is in pDotMatrix. Can be used for continuous animation.
 */
void anim_invertText(anim_invert_t *pAnim, dotMatrix_t *pDotMatrix, max7219_t *pMax7219, const char *text);

/**
 * @brief Animate changing of text.
 * @details This function will only return once the whole changing cycle has completed. One whole
 * 		changing cycle starts at the first text (textArr[0]) and ends at the last text (textArrlen-1]).
 * 		If len and pAnim.len do not match, pAnim.len will cycle itself if pAnim.len < len. Otherwise,
 * 		pAnim.len will just be consumed as much as len if pAnim.len > len.
 * @param[in] pAnim Changing animation instance containing the change information.
 * @param[in] pDotMatrix Dot matrix instance containing buffer for encoded text for display.
 * @param[in] pMax7219 Cascaded hardware MAX7219 instance.
 * @param[in] textArr Array of text to be changed. Each text length is determined by the number of lines
 * 		available for display. Long text will truncated to fit into the display window. Cannot be NULL.
 * @param[in] len Length of textArr.
 */
void anim_changeText(anim_change_t *pAnim, dotMatrix_t *pDotMatrix, max7219_t *pMax7219, const char *const *textArr, uint32_t len);

#endif /* SRC_ANIM_H_ */
