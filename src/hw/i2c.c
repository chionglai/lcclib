/*
 * i2c.c
 *
 *  Created on: 6 Jul 2016
 *      Author: chiong
 */

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "util/status.h"
#include "hw/i2c.h"

int32_t i2c_updateReg(i2c_t *i2c, uint8_t regAddr, uint8_t mask, uint8_t data) {
	uint8_t buffer[2];
	int32_t status;

	if ((status = i2c_readRegN(i2c, regAddr, buffer, 1)) != STATUS_OK)
		return status;

	buffer[1] = (buffer[0] & ~mask) | (mask & data);
	buffer[0] = regAddr;

	return i2c_transfer(i2c, NULL, 0, buffer, 2);
}

int32_t i2c_readRegN (i2c_t *i2c, uint8_t regStartAddr, uint8_t* const buffer, uint32_t len) {
	return i2c_transfer(i2c, buffer, len, &regStartAddr, 1);
}

int32_t i2c_writeRegN(i2c_t *i2c, uint8_t regStartAddr, const uint8_t* const data, uint32_t len) {
	uint8_t *buffer;
	int32_t status;

	buffer = (uint8_t*) malloc(len + 1);
	if (NULL == buffer) {
		return STATUS_ERROR;
	}

	buffer[0] = regStartAddr;
	memcpy(&buffer[1], data, len);

	status = i2c_transfer(i2c, NULL, 0, buffer, len+1);
	free(buffer);
	return status;
}


