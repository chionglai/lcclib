/*
 * bitmap.c
 *
 *  Created on: 3 Jun 2016
 *      Author: chiong
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "util/bitmap.h"
#include "util/util.h"


int32_t bitmap_readFile(bitmap_t **bitmap, char* filename) {
	bitmap_t *pBitmap;
	uint8_t *image;
	FILE *pFile;
	int32_t size;

	pBitmap = (bitmap_t*) malloc(sizeof(bitmap_t));
	if (NULL == pBitmap) {
		return STATUS_ERROR_MALLOC;
	}

	pFile = fopen(filename, "r");
	if (NULL == pFile) {
		bitmap_destroy(&pBitmap);
		return STATUS_ERROR_FILE_OPEN;
	}

	/* Need to do this because of struct auto memory alignment for packing of
	 * different sized elements. So need to skip the auto padding.
	 */
	fread(&pBitmap->fileHeader.type, 1, 2, pFile);
	fread(&pBitmap->fileHeader.size, 1, BITMAP_HEADER_SIZE - 2, pFile);

	size = pBitmap->fileHeader.size - BITMAP_HEADER_SIZE;
	image = (uint8_t*) malloc((size_t) size);
	if (NULL == image) {
		fclose(pFile);
		bitmap_destroy(&pBitmap);
		return STATUS_ERROR_MALLOC;
	}

	if (BITMAP_COLORMAP_32_BIT == pBitmap->imageHeader.bitCount) {
		fread(image, 1, (size_t) size, pFile);
	} else {
		fprintf(stderr, "Only support 32-bit BMP format!\n");
	}
	fclose(pFile);

	pBitmap->imageHeader.width = pBitmap->imageHeader.width < 0? -pBitmap->imageHeader.width : pBitmap->imageHeader.width;
	pBitmap->imageHeader.height = pBitmap->imageHeader.height < 0? -pBitmap->imageHeader.height : pBitmap->imageHeader.height;
	pBitmap->image = (uint32_t*) image;
	*bitmap = pBitmap;

	return STATUS_OK;
}


void bitmap_destroy(bitmap_t **bitmap) {
	bitmap_t *pBitmap = *bitmap;

	if (NULL != pBitmap) {
		if (NULL != pBitmap->image) {
			free(pBitmap->image);
		}
		free(pBitmap);
		*bitmap = NULL;
	}
}
