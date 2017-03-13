/*
 * bitmap.h
 *
 *  Created on: 3 Jun 2016
 *      Author: chiong
 */

#ifndef INC_BITMAP_H_
#define INC_BITMAP_H_

#include <stdint.h>
#include "status.h"

/* Header information size in bytes */
#define BITMAP_FILE_HEADER_SIZE		(14)
#define BITMAP_IMAGE_HEADER_SIZE	(40)
#define BITMAP_HEADER_SIZE			(BITMAP_FILE_HEADER_SIZE + BITMAP_IMAGE_HEADER_SIZE)


#define BITMAP_COLORMAP_24_BIT		(24)
#define BITMAP_COLORMAP_32_BIT		(32)


typedef struct {
	int16_t type;
	int32_t size;
	int32_t reserved;
	int32_t offBits;
} bitmap_file_t;

typedef struct {
	int32_t size;
	int32_t width;
	int32_t height;
	int16_t planes;
	int16_t bitCount;		/**< Number of bits per pixel */
	int32_t compression;
	int32_t imageSize;
	int32_t xPixelPerMeter;
	int32_t yPixelPerMeter;
	int32_t colorUsed;
	int32_t colorImportant;
} bitmap_image_t;


typedef struct {
	bitmap_file_t fileHeader;
	bitmap_image_t imageHeader;
	uint32_t *image;
} bitmap_t;


/**
 * @brief Read bitmap file.
 * @details Read bitmap file into bitmap struct. Only 32-bit BMP format is
 * 		currently supported.
 * @param[out] Bitmap struct to be populated in this function. Once done,
 * 		this struct must be freed by calling bitmap_destroy().
 * @param[in] filename Filename of bitmap file to read.
 */
int32_t bitmap_readFile(bitmap_t **bitmap, char* filename);


/**
 * @brief To desroy and free up bitmap struct.
 * @details Once destroyed, bitmap will be NULL and must not be used.
 * @param[inout] Bitmap struct to be destroyed and freed.
 */
void bitmap_destroy(bitmap_t **bitmap);

#endif /* INC_BITMAP_H_ */
