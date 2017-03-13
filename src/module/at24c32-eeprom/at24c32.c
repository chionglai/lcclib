/*
 * at24c32.c
 *
 *  Created on: 19/06/2013
 *      Author: lcc
 */

#include <stdlib.h>
#include <string.h>
#include "util/status.h"
#include "hw/i2c.h"
#include "module/at24c32-eeprom/at24c32.h"

struct at24c32_s {
	i2c_t i2c;
};

int32_t at24c32_create(at24c32_t **ppAt24c32, const at24c32Cfg_t *cfg) {
	at24c32_t *at24c32;

	at24c32 = (at24c32_t*) malloc(sizeof(at24c32_t));
	if (NULL == at24c32) {
		return STATUS_ERROR_MALLOC;
	}
	*ppAt24c32 = at24c32;
	at24c32->i2c.i2cIdx = cfg->i2cIdx;
	at24c32->i2c.slaveAddr = AT24C32_I2C_ADDR;
	
	return STATUS_OK;
}

int32_t at24c32_destroy(at24c32_t **ppAt24c32) {
	if (NULL != *ppAt24c32) {
		free(*ppAt24c32);
		*ppAt24c32 = NULL;
	}

	return STATUS_OK;
}

int32_t at24c32_updateReg (at24c32_t *at24c32, uint16_t regAddr, uint8_t mask, uint8_t data) {
	uint8_t buffer[3];
	int32_t status;

	buffer[0] = (uint8_t)((regAddr >> 8) & 0x00FF);
	buffer[1] = (uint8_t)(regAddr & 0x00FF);

	if ((status = i2c_transfer(&at24c32->i2c, buffer, 1, buffer, 2)) != STATUS_OK)
		return status;

	buffer[0] = (uint8_t)((regAddr >> 8) & 0x00FF);
	buffer[1] = (uint8_t)(regAddr & 0x00FF);
	buffer[2] = (buffer[0] & ~mask) | (mask & data);

	return i2c_transfer(&at24c32->i2c, NULL, 0, buffer, 3);
}

int32_t at24c32_readRegN (at24c32_t *at24c32, uint16_t regStartAddr, uint8_t* const buffer, uint32_t len) {
	uint8_t addr[2];

	addr[0] = (uint8_t)((regStartAddr >> 8) & 0x00FF);
	addr[1] = (uint8_t)(regStartAddr & 0x00FF);
	return i2c_transfer(&at24c32->i2c, buffer, len, addr, 2);
}

int32_t at24c32_writeRegN(at24c32_t *at24c32, uint16_t regStartAddr, const uint8_t* const data, uint32_t len) {
	uint8_t *buffer;

	buffer = (uint8_t*) malloc(len + 2);
	if (NULL == buffer) {
		return STATUS_ERROR;
	}

	buffer[0] = (uint8_t)((regStartAddr >> 8) & 0x00FF);
	buffer[1] = (uint8_t)(regStartAddr & 0x00FF);
	memcpy(&buffer[2], data, len);

	return i2c_transfer(&at24c32->i2c, NULL, 0, buffer, len+2);
}

