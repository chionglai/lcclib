/*
 * i2c_bcm2835.c
 *
 *  Created on: 6 Jul 2016
 *      Author: chiong
 */

#include <stdint.h>
#include <stdlib.h>
#include "util/status.h"
#include "hw/i2c.h"

#include "3p-lib/bcm2835-1.50/inc/bcm2835.h"

int32_t i2c_transfer(i2c_t *i2c, uint8_t *rxBuffer, uint32_t rLen, const uint8_t *txBuffer, uint32_t wLen) {
	uint8_t status;

	bcm2835_i2c_setSlaveAddress((uint8_t)(i2c->slaveAddr));

	if (NULL != txBuffer && wLen > 0) {
		if ((status = bcm2835_i2c_write((const char*)txBuffer, wLen)) != BCM2835_I2C_REASON_OK)
			return STATUS_ERROR;
	}

	if (NULL != rxBuffer && rLen > 0) {
		if ((status = bcm2835_i2c_read((char*)rxBuffer, rLen)) != BCM2835_I2C_REASON_OK)
			return STATUS_ERROR;
	}

	return STATUS_OK;
}
