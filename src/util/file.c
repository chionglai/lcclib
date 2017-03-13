/*
 * file.c
 *
 *  Created on: 13 Jul 2016
 *      Author: chiong
 */

#include <stdio.h>
#include "util/file.h"
#include "util/status.h"

int32_t file_readBinN(void* buffer, uint32_t size, uint32_t count, const char* filename) {
	FILE *f;
	uint32_t nRead;

	f = fopen(filename, "rb");
	if (NULL == f) {
		return STATUS_ERROR_FILE_OPEN;
	}
	/* Find length of file to read till EOF */
	if (count == 0) {
		fseek(f, 0, SEEK_END);
		count = ftell(f)/size;
		rewind(f);
	}

	nRead = fread(buffer, size, count, f);
	fclose(f);

	return nRead;
}
