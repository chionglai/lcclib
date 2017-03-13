/*
 * max7219.c
 *
 *  Created on: 6 Jul 2016
 *      Author: chiong
 */
#include <stdlib.h>
#include <string.h>
#include "util/buffer.h"
#include "hw/spi.h"
#include "module/max7219-dotMatrix/max7219.h"

struct max7219_s {
	/* SPI index containing SPI configuration. */
	uint8_t spiIdx;
	/* CS index to use. */
	uint8_t cs;
	/* Number of MAX7219 being cascaded. */
	uint32_t numCascade;
	/* Temporary buffer. Size of 2 * numCascade. */
	uint8_t *tempBuffer;
};

int32_t max7219_create(max7219_t **ppSelf, const max7219Cfg_t *pCfg) {
	max7219_t *pSelf;

	pSelf = (max7219_t*) malloc(sizeof(max7219_t));
	if (NULL == pSelf) {
		return STATUS_ERROR_MALLOC;
	}

	pSelf->numCascade = pCfg->numCascade;
	pSelf->spiIdx = pCfg->spiIdx;
	pSelf->cs = pCfg->cs;

	pSelf->tempBuffer = (uint8_t*) malloc(2 * pSelf->numCascade);
	if (NULL == pSelf->tempBuffer) {
		max7219_destroy(&pSelf);
		return STATUS_ERROR_MALLOC;
	}

	*ppSelf = pSelf;
	return STATUS_OK;
}

int32_t max7219_destroy(max7219_t **ppSelf) {
	max7219_t *pSelf = *ppSelf;

	if (NULL != pSelf) {
		if (NULL != pSelf->tempBuffer) {
			free(pSelf->tempBuffer);
		}
		free(pSelf);
		*ppSelf = NULL;
	}

	return STATUS_OK;
}

int32_t max7219_writeToAll(const max7219_t* pSelf, uint8_t regAddr, uint8_t value) {
	uint32_t i;
	uint32_t count = pSelf->numCascade << 1;
	for (i = 0; i < count; i += 2) {
		pSelf->tempBuffer[i] = regAddr;
		pSelf->tempBuffer[i+1] = value;
	}

	spi_selectCs(pSelf->spiIdx, pSelf->cs, 0); 	/* 0 for active LOW */
	spi_transfer(pSelf->spiIdx, NULL, pSelf->tempBuffer, count);

	return STATUS_OK;
}

int32_t max7219_write(const max7219_t* pSelf, const uint8_t* data, uint32_t len) {
	uint32_t i, j;

	spi_selectCs(pSelf->spiIdx, pSelf->cs, 0); 	/* 0 for active LOW */

	for (i = 0; i < MAX7219_DIGIT_LENGTH; i++) {
		/* Display data addresses are from 0x01 to 0x08.
		 * Because the data[0] is sent first, this result in the opposite
		 * LED matrix indexing as opposed to the way the module is cascaded.
		 * See the doc for dotmatrix.h for details.
		 */
		for (j = 0; (i + (j << 3)) < len; j++) {
			pSelf->tempBuffer[j << 1] = (i & 0x7) + 1;
			pSelf->tempBuffer[(j << 1) +1] = data[i + (j << 3)];
		}

		spi_transfer(pSelf->spiIdx, NULL, pSelf->tempBuffer, pSelf->numCascade << 1);
	}

	return STATUS_OK;
}

int32_t max7219_fill(const max7219_t* pSelf, const uint8_t data) {
	uint32_t i;

	for (i = 0; i < MAX7219_DIGIT_LENGTH; i++) {
		max7219_writeToAll(pSelf, i + 1, data);
	}

	return STATUS_OK;
}

uint32_t max7219_getNumCascade(const max7219_t* pSelf) {
	return pSelf->numCascade;
}
