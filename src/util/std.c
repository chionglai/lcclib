/*
 * std.c
 *
 *  Created on: 13 Jul 2016
 *      Author: chiong
 */

#include <stdio.h>
#include <stdlib.h>
#include "util/std.h"
#include "util/status.h"

int32_t std_in_readLine(char *str, uint32_t len, char *msg) {
	uint32_t i = 0;
	char c = 0;

	printf("%s", msg);
	/* -1 is EOF */
	while ((c = (char) getchar()) != '\n' && c != -1 && i < len) {
		str[i] = c;
		i++;
	}
	str[i] = '\0';

	return i;
}

int32_t std_in_readInt(int *out, char *msg) {
	char str[STD_MAX_NUM_CHAR];
	int val;

	if (std_in_readLine(str, STD_MAX_NUM_CHAR, msg) > 0) {
		if (sscanf(str, "%d", &val) == 1) {
			*out = val;
			return 1;
		}
	}

	/* No valid value read */
	return STATUS_ERROR;
}

int32_t std_in_readFloat(float *out, char *msg) {
	char str[STD_MAX_NUM_CHAR];
	float val;

	if (std_in_readLine(str, STD_MAX_NUM_CHAR, msg) > 0) {
		if(sscanf(str, "%f", &val)) {
			*out = val;
			return 1;
		}
	}

	return STATUS_ERROR;
}
