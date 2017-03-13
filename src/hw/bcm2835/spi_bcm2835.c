/*
 * spi_bcm2835.c
 *
 *  Created on: 6 Jul 2016
 *      Author: chiong
 *
 *  SPI wrapper implementation for Raspberry Pi BCM2835.
 */

#include <stdlib.h>
#include "util/status.h"
#include "hw/spi.h"
#include "3p-lib/bcm2835-1.50/inc/bcm2835.h"

#if 0
#define BCM2835_SPI_COUNT	(1)

int32_t spi_open(void) {
	/* No need to allocate additional resources for instantiating SPI. */
	bcm2835_spi_begin();
	return BCM2835_SPI_COUNT;
}

int32_t spi_config(uint8_t spiIdx, const spiCfg_t *cfg) {
	uint32_t div;

	/* Rounding */
	div = 2 * cfg->baseClock / cfg->bitRate;
	div = (div >> 1) + (div & 0x1);

	bcm2835_spi_setClockDivider(div);
	bcm2835_spi_setChipSelectPolarity(BCM2835_SPI_CS0, LOW);
	bcm2835_spi_setDataMode(cfg->frameFormat);
	bcm2835_spi_setBitOrder(cfg->bitOrder);

	return STATUS_OK;
}

void spi_close(void) {
	bcm2835_spi_end();
}

#endif

int32_t spi_selectCs(uint8_t spiIdx, int8_t cs, uint8_t active) {
	bcm2835SPIChipSelect csPin;

	/* spiIdx is not used in this implementation. */
	(void) spiIdx;

	if (cs < 0) {
		csPin = BCM2835_SPI_CS_NONE;
	} else if (BCM2835_SPI_CS0 <= cs && cs <= BCM2835_SPI_CS2) {
		csPin = cs;
	} else {
		return STATUS_ERROR;
	}
	bcm2835_spi_chipSelect(csPin);
	bcm2835_spi_setChipSelectPolarity(csPin, active);

	return STATUS_OK;
}

int32_t spi_transfer(uint8_t spiIdx, uint8_t *rxBuffer, const uint8_t *txBuffer, uint32_t length) {
	if (NULL != rxBuffer) {
		bcm2835_spi_transfernb((char*) txBuffer, (char*) rxBuffer, length);
	} else {
		bcm2835_spi_writenb((char*) txBuffer, length);
	}
	return length;
}


