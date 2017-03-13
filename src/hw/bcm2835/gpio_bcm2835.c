/*
 * gpio_bcm2835.c
 *
 *  Created on: 6 Jul 2016
 *      Author: chiong
 *
 *  GPIO wrapper implementation for Raspberry Pi BCM2835.
 */

#include <stdlib.h>
#include "hw/gpio.h"
#include "3p-lib/bcm2835-1.50/inc/bcm2835.h"

void gpio_write(uint32_t pinId, uint8_t value) {
	bcm2835_gpio_write(pinId, value);
}

uint8_t gpio_read(uint32_t pinId) {
	return bcm2835_gpio_lev(pinId);
}

void gpio_toggle(uint32_t pinId) {
	gpio_write(pinId, ~gpio_read(pinId));
}
