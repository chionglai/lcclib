/*
 * anim.c
 *
 *  Created on: 12 Jul 2016
 *      Author: chiong
 */

#include <stdlib.h>
#include <string.h>
#include "module/max7219-dotMatrix/anim.h"
#include "module/max7219-dotMatrix/dotmatrix.h"
#include "module/max7219-dotMatrix/max7219.h"


/**
 * @brief Bitwise invert all element in an array.
 * @details Support in-place operation.
 * @param[out] out Output array. Can be same as input array for in-place operation.
 * @param[in] in Input array.
 * @param[in] len Length of array to be inverted.
 */
void anim_invertArray(void* out, void* in, uint32_t len);

void anim_hScrollText(anim_scroll_t *pAnim, dotMatrix_t *pDotMatrix, max7219_t *pMax7219, const char* text) {
	int32_t offset;
	int32_t numDisplayLine;
	int32_t numTextLine;
	uint8_t *tempBuf;

	if (NULL != text) {
		dotMatrix_drawText(pDotMatrix, 0, text);
	}

	numDisplayLine = max7219_getNumCascade(pMax7219) * MAX7219_DIGIT_LENGTH;
	tempBuf = (uint8_t*) malloc(numDisplayLine);
	numTextLine = dotMatrix_getNumLineToDisplay(pDotMatrix);

	if (pAnim->stepSize > 0) {
		/* Positive/left scrolling */
		for (offset = -numDisplayLine; offset <= numTextLine; offset+=pAnim->stepSize) {
			dotMatrix_getDisplay(pDotMatrix, offset, numDisplayLine, 0, tempBuf);
			max7219_write(pMax7219, tempBuf, numDisplayLine);

			if (pAnim->delayFxn)
				pAnim->delayFxn(pAnim->waitTimeMs);
		}
	} else {
		/* Negative/right scrolling */
		for (offset = numTextLine; offset >= -numDisplayLine; offset+=pAnim->stepSize) {
			dotMatrix_getDisplay(pDotMatrix, offset, numDisplayLine, 0, tempBuf);
			max7219_write(pMax7219, tempBuf, numDisplayLine);

			if (pAnim->delayFxn)
				pAnim->delayFxn(pAnim->waitTimeMs);
		}
	}
	/* Clear the display hen done */
	MAX7219_clear(pMax7219);

	free(tempBuf);
}

void anim_blinkText(anim_blink_t *pAnim, dotMatrix_t *pDotMatrix, max7219_t *pMax7219, const char* text) {
	uint32_t i;
	uint32_t numChar;
	int32_t offset;
	int32_t numDisplayLine;
	int32_t numTextLine;
	uint32_t len;
	uint8_t *tempBuf;

	numDisplayLine = max7219_getNumCascade(pMax7219) * MAX7219_DIGIT_LENGTH;
	tempBuf = (uint8_t*) malloc(numDisplayLine);

	if (NULL != text) {
		len = strlen(text);

		numChar = numDisplayLine / DOTMATRIX_FONT_CHAR_WIDTH;
		numChar = numChar < len? numChar : len;

		/* Assume tempBuf size > text size */
		memcpy(tempBuf, text, numChar);
		tempBuf[numChar] = '\0';

		dotMatrix_drawText(pDotMatrix, 0, (char*)tempBuf);
	}

	numTextLine = dotMatrix_getNumLineToDisplay(pDotMatrix);

	/* numTextLine will always be <= numDisplayLine */
	switch(pAnim->layout) {
	case TEXT_LAYOUT_LEFT:
		offset = 0;
		break;
	case TEXT_LAYOUT_RIGHT:
		offset = numTextLine - numDisplayLine;
		break;
	default:
		/* TEXT_LAYOUT_CENTER is default */
		offset = (numTextLine - numDisplayLine) / 2;
		break;
	}
	dotMatrix_getDisplay(pDotMatrix, offset, numDisplayLine, 0, tempBuf);
	max7219_write(pMax7219, tempBuf, numDisplayLine);

	for (i = 0; i < pAnim->len; i++) {
		/* Use MAX7219 shutdown function for blinking rather than actually
		 * clear the display
		 */
		MAX7219_setDisplayEnabled(pMax7219, MAX7219_VALUE_ON);
		if (pAnim->delayFxn)
			pAnim->delayFxn(pAnim->onTimeMs[i]);
		MAX7219_setDisplayEnabled(pMax7219, MAX7219_VALUE_OFF);
		if (pAnim->delayFxn)
			pAnim->delayFxn(pAnim->offTimeMs[i]);
	}

	/* Actually clear the display and re-enable the display so other process can use it */
	MAX7219_clear(pMax7219);
	/* Since clearing the display results in sending a lot of data to the dot matrix, having
	 * the delay between clearing and enable the dot matrix helps to avoid flickering.
	 */
	if (pAnim->delayFxn)
		pAnim->delayFxn(pAnim->waitTimeMs);
	MAX7219_setDisplayEnabled(pMax7219, MAX7219_VALUE_ON);

	free(tempBuf);
}

void anim_invertText(anim_invert_t *pAnim, dotMatrix_t *pDotMatrix, max7219_t *pMax7219, const char* text) {
	uint32_t i;
	uint32_t numChar;
	int32_t offset;
	int32_t numDisplayLine;
	int32_t numTextLine;
	uint32_t len;
	uint8_t *tempBuf;
	uint8_t *invBuf;

	numDisplayLine = max7219_getNumCascade(pMax7219) * MAX7219_DIGIT_LENGTH;
	tempBuf = (uint8_t*) malloc(numDisplayLine);
	invBuf = (uint8_t*) malloc(numDisplayLine);

	if (NULL != text) {
		len = strlen(text);

		numChar = numDisplayLine / DOTMATRIX_FONT_CHAR_WIDTH;
		numChar = numChar < len? numChar : len;

		/* Assume tempBuf size > text size */
		memcpy(tempBuf, text, numChar);
		tempBuf[numChar] = '\0';

		dotMatrix_drawText(pDotMatrix, 0, (char*)tempBuf);
	}

	numTextLine = dotMatrix_getNumLineToDisplay(pDotMatrix);

	/* numTextLine will always be <= numDisplayLine */
	switch(pAnim->layout) {
	case TEXT_LAYOUT_LEFT:
		offset = 0;
		break;
	case TEXT_LAYOUT_RIGHT:
		offset = numTextLine - numDisplayLine;
		break;
	default:
		/* TEXT_LAYOUT_CENTER is default */
		offset = (numTextLine - numDisplayLine) / 2;
		break;
	}
	dotMatrix_getDisplay(pDotMatrix, offset, numDisplayLine, 0, tempBuf);
	anim_invertArray(invBuf, tempBuf, numDisplayLine);

	for (i = 0; i < pAnim->len; i++) {
		max7219_write(pMax7219, tempBuf, numDisplayLine);
		if (pAnim->delayFxn)
			pAnim->delayFxn(pAnim->onTimeMs[i]);
		max7219_write(pMax7219, invBuf, numDisplayLine);
		if (pAnim->delayFxn)
			pAnim->delayFxn(pAnim->offTimeMs[i]);
	}
	/* Clear the display hen done */
	MAX7219_clear(pMax7219);
	if (pAnim->delayFxn)
		pAnim->delayFxn(pAnim->waitTimeMs);

	free(tempBuf);
	free(invBuf);
}

void anim_changeText(anim_change_t *pAnim, dotMatrix_t *pDotMatrix, max7219_t *pMax7219, const char *const *textArr, uint32_t len) {
	uint32_t i, j;
	uint32_t numChar;
	uint32_t numDisplayChar;
	int32_t offset;
	int32_t numDisplayLine;
	int32_t numTextLine;
	uint32_t textLen;
	uint8_t *tempBuf;
	const char *text;

	numDisplayLine = max7219_getNumCascade(pMax7219) * MAX7219_DIGIT_LENGTH;
	numDisplayChar = numDisplayLine / DOTMATRIX_FONT_CHAR_WIDTH;
	tempBuf = (uint8_t*) malloc(numDisplayLine);

	for (i = 0; i < len; i++) {
		text = textArr[i];
		textLen = strlen(text);
		numChar = numDisplayChar < textLen? numDisplayChar : textLen;

		/* Assume tempBuf size > text size */
		memcpy(tempBuf, text, numChar);
		tempBuf[numChar] = '\0';

		dotMatrix_drawText(pDotMatrix, 0, (char*)tempBuf);
		numTextLine = dotMatrix_getNumLineToDisplay(pDotMatrix);

		/* numTextLine will always be <= numDisplayLine */
		switch(pAnim->layout) {
		case TEXT_LAYOUT_LEFT:
			offset = 0;
			break;
		case TEXT_LAYOUT_RIGHT:
			offset = numTextLine - numDisplayLine;
			break;
		default:
			/* TEXT_LAYOUT_CENTER is default */
			offset = (numTextLine - numDisplayLine) / 2;
			break;
		}
		dotMatrix_getDisplay(pDotMatrix, offset, numDisplayLine, 0, tempBuf);

		/* Wrap changing information in case pAnim->len < len */
		j = i % pAnim->len;
		if (NULL != pAnim->isInvert && pAnim->isInvert[j]) {
			anim_invertArray(tempBuf, tempBuf, numDisplayLine);
		}
		max7219_write(pMax7219, tempBuf, numDisplayLine);
		if (pAnim->delayFxn)
			pAnim->delayFxn(pAnim->timeMs[j]);
	}

	/* Clear the display hen done */
	MAX7219_clear(pMax7219);

	free(tempBuf);
}


void anim_invertArray(void* out, void* in, uint32_t len) {
	uint32_t i;
	uint8_t *pOut = (uint8_t*) out;
	uint8_t *pIn = (uint8_t*) in;

	for (i = 0; i < len; i++) {
		pOut[i] = ~pIn[i];
	}
}
