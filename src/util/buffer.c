/*
 * buffer.c
 *
 *  Created on: 8 Jul 2016
 *      Author: chiong
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "util/buffer.h"

/**
 * @brief Data struct for 2D buffer stored as 1d array.
 */
struct buffer2d_s {
	/** Contiguous memory that contains the actual data. */
	void* data;
	/** Size, in bytes of each element. */
	uint32_t elemSize;
	/** Number of rows. */
	uint32_t nRow;
	/** Number of columns. */
	uint32_t nCol;
	/** 2D buffer layout, either row or column wise. */
	uint32_t layout;
};

int32_t buffer2d_create(buffer2d_t **ppSelf, uint32_t nRow, uint32_t nCol, uint32_t elemSize, uint8_t layout) {
	buffer2d_t *pSelf;

	pSelf = (buffer2d_t*) malloc(sizeof(buffer2d_t));
	if (NULL == pSelf) {
		return STATUS_ERROR_MALLOC;
	}
	pSelf->elemSize = elemSize;
	pSelf->nRow = nRow;
	pSelf->nCol = nCol;
	pSelf->data = (void*) calloc(pSelf->nRow * pSelf->nCol, pSelf->elemSize);
	if (NULL == pSelf->data) {
		buffer2d_destroy(&pSelf);
		return STATUS_ERROR_MALLOC;
	}
	pSelf->layout = layout;
	*ppSelf = pSelf;

	return STATUS_OK;
}

int32_t buffer2d_destroy(buffer2d_t **ppSelf) {
	buffer2d_t *pSelf = *ppSelf;
	if (NULL != pSelf) {
		if (NULL != pSelf->data) {
			free(pSelf->data);
		}

		free(pSelf);
		*ppSelf = NULL;
	}

	return STATUS_OK;
}

void buffer2d_putDataSingle(buffer2d_t *pSelf, uint32_t iRow, uint32_t jCol, const void *data) {
	switch(pSelf->layout) {
	case BUFFER2D_LAYOUT_ROW_WISE:
		memcpy(pSelf->data + (iRow * pSelf->nCol + jCol) * pSelf->elemSize, data, pSelf->elemSize);
		break;

	case BUFFER2D_LAYOUT_COLUMN_WISE:
		memcpy(pSelf->data + (jCol * pSelf->nRow + iRow) * pSelf->elemSize, data, pSelf->elemSize);
		break;

	default:
		/* Do nothing */
		break;
	}
}

void buffer2d_putDataRow(buffer2d_t *pSelf, uint32_t iRow, void *data) {
	uint32_t i;
	uint32_t jump;
	void *pData;

	switch(pSelf->layout) {
	case BUFFER2D_LAYOUT_ROW_WISE:
		memcpy(pSelf->data + (iRow * pSelf->nCol) * pSelf->elemSize, data, pSelf->elemSize * pSelf->nCol);
		break;

	case BUFFER2D_LAYOUT_COLUMN_WISE:
		jump = pSelf->nRow * pSelf->elemSize;
		pData = pSelf->data + iRow * pSelf->elemSize;
		for (i = 0; i < pSelf->nCol; i++, data += pSelf->elemSize, pData += jump) {
			memcpy(pData, data, pSelf->elemSize);
		}
		break;

	default:
		/* Do nothing */
		break;
	}
}

void buffer2d_putDataCol(buffer2d_t *pSelf, uint32_t jCol, void *data) {
	uint32_t i;
	uint32_t jump;
	void *pData;

	switch(pSelf->layout) {
	case BUFFER2D_LAYOUT_ROW_WISE:
		jump = pSelf->nCol * pSelf->elemSize;
		pData = pSelf->data + jCol * pSelf->elemSize;
		for (i = 0; i < pSelf->nRow; i++, data += pSelf->elemSize, pData += jump) {
			memcpy(pData, data, pSelf->elemSize);
		}
		break;

	case BUFFER2D_LAYOUT_COLUMN_WISE:
		memcpy(pSelf->data + (jCol * pSelf->nRow) * pSelf->elemSize, data, pSelf->elemSize * pSelf->nRow);
		break;

	default:
		/* Do nothing */
		break;
	}
}

void buffer2d_getDataSingle(const buffer2d_t *pSelf, uint32_t iRow, uint32_t jCol, void *data) {
	switch(pSelf->layout) {
	case BUFFER2D_LAYOUT_ROW_WISE:
		memcpy(data, pSelf->data + (iRow * pSelf->nCol + jCol) * pSelf->elemSize, pSelf->elemSize);
		break;

	case BUFFER2D_LAYOUT_COLUMN_WISE:
		memcpy(data, pSelf->data + (jCol * pSelf->nRow + iRow) * pSelf->elemSize, pSelf->elemSize);
		break;

	default:
		/* Do nothing */
		break;
	}
}

void buffer2d_getDataRow(buffer2d_t *pSelf, uint32_t iRow, void *data) {
	uint32_t i;
	uint32_t jump;
	void *pData;

	switch(pSelf->layout) {
	case BUFFER2D_LAYOUT_ROW_WISE:
		memcpy(data, pSelf->data + (iRow * pSelf->nCol) * pSelf->elemSize, pSelf->elemSize * pSelf->nCol);
		break;

	case BUFFER2D_LAYOUT_COLUMN_WISE:
		jump = pSelf->nRow * pSelf->elemSize;
		pData = pSelf->data + iRow * pSelf->elemSize;
		for (i = 0; i < pSelf->nCol; i++, data += pSelf->elemSize, pData += jump) {
			memcpy(data, pData, pSelf->elemSize);
		}
		break;

	default:
		/* Do nothing */
		break;
	}
}

void buffer2d_getDataCol(buffer2d_t *pSelf, uint32_t jCol, void *data) {
	uint32_t i;
	uint32_t jump;
	void *pData;

	switch(pSelf->layout) {
	case BUFFER2D_LAYOUT_ROW_WISE:
		jump = pSelf->nCol * pSelf->elemSize;
		pData = pSelf->data + jCol * pSelf->elemSize;
		for (i = 0; i < pSelf->nRow; i++, data += pSelf->elemSize, pData += jump) {
			memcpy(data, pData, pSelf->elemSize);
		}
		break;

	case BUFFER2D_LAYOUT_COLUMN_WISE:
		memcpy(data, pSelf->data + (jCol * pSelf->nRow) * pSelf->elemSize, pSelf->elemSize * pSelf->nRow);
		break;

	default:
		/* Do nothing */
		break;
	}
}

void buffer2d_fill(buffer2d_t *pSelf, void *elem) {
	uint32_t i, count;
	void *pData;

	pData = (void*) buffer2d_getBuffer(pSelf);
	count = pSelf->nRow * pSelf->nCol;
	for (i = 0; i < count; i++) {
		memcpy(pData, elem, pSelf->elemSize);
		pData += pSelf->elemSize;
	}
}

uint32_t buffer2d_getElemSize(const buffer2d_t *pSelf) {
	return pSelf->elemSize;
}

uint32_t buffer2d_getNumRow(buffer2d_t *pSelf) {
	return pSelf->nRow;
}

uint32_t buffer2d_getNumCol(buffer2d_t *pSelf) {
	return pSelf->nCol;
}

const void* buffer2d_getBuffer(buffer2d_t *pSelf) {
	return pSelf->data;
}

void buffer2d_print(buffer2d_t *pSelf) {
	uint32_t i, j;
	uint8_t val8;
	uint16_t val16;
	uint32_t val32;

	switch(pSelf->elemSize) {
	case 1:
		for (i = 0; i < pSelf->nRow; i++) {
			for (j = 0; j < pSelf->nCol; j++) {
				buffer2d_getDataSingle(pSelf, i, j, &val8);
				fprintf(stdout, "%d, ", val8);
			}
			fprintf(stdout, "\n");
		}
		break;

	case 2:
		for (i = 0; i < pSelf->nRow; i++) {
			for (j = 0; j < pSelf->nCol; j++) {
				buffer2d_getDataSingle(pSelf, i, j, &val16);
				fprintf(stdout, "%d, ", val16);
			}
			fprintf(stdout, "\n");
		}
		break;

	case 4:
		for (i = 0; i < pSelf->nRow; i++) {
			for (j = 0; j < pSelf->nCol; j++) {
				buffer2d_getDataSingle(pSelf, i, j, &val32);
				fprintf(stdout, "%d, ", val32);
			}
			fprintf(stdout, "\n");
		}
		break;
	}
}
