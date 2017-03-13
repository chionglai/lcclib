/*
 * max7219.h
 *
 *  Created on: 6 Jul 2016
 *      Author: chiong
 *
 *  This implementation only sends the encoded/arranged display data to cascaded MAX7219
 *  hardware. It does not perform any encoding/arranging of display data, e.g. conversion
 *  from text to display data. This is done in dotmatrix.c.
 *
 *  Note:
 *  1. Since some bits value are treated as don't care by the hardware, there is no need
 *     for value masking. Masking can still be done just to be safe.
 */
#ifndef SRC_MAX7219_H_
#define SRC_MAX7219_H_

#include <stdio.h>
#include <stdint.h>
#include "util/status.h"
#include "module/max7219-dotMatrix/max7219.h"

/* MAX7219 register address */
#define MAX7219_REG_NO_OP			0x00
#define MAX7219_REG_DIGIT_0			0x01
#define MAX7219_REG_DIGIT_1			0x02
#define MAX7219_REG_DIGIT_2			0x03
#define MAX7219_REG_DIGIT_3			0x04
#define MAX7219_REG_DIGIT_4			0x05
#define MAX7219_REG_DIGIT_5			0x06
#define MAX7219_REG_DIGIT_6			0x07
#define MAX7219_REG_DIGIT_7			0x08
#define MAX7219_REG_DECODE_MODE		0x09
#define MAX7219_REG_INTENSITY		0x0A
#define MAX7219_REG_SCAN_LIMIT		0x0B
#define MAX7219_REG_SHUTDOWN		0x0C
#define MAX7219_REG_DISPLAY_TEST	0x0F

#define MAX7219_VALUE_OFF			0x00
#define MAX7219_VALUE_ON			0x01

#define MAX7219_MASK_INTENSITY		0x0F
#define MAX7219_MASK_SCAN_LIMIT		0x07

#define MAX7219_DIGIT_LENGTH		8

/**
 * @brief Macro to set the decode mode.
 * @param[in] pSelf Pointer to an instance.
 * @param[in] value
 */
#define MAX7219_setDecodeMode(pSelf, value)	\
	max7219_writeToAll(pSelf, MAX7219_REG_DECODE_MODE, value)

/**
 * @brief Macro to set the intensity.
 * @param[in] pSelf Pointer to an instance.
 * @param[in] value A value between [0, 15], where 0 is min intensity, 15 is max.
 */
#define MAX7219_setIntensity(pSelf, value)	\
	max7219_writeToAll(pSelf, MAX7219_REG_INTENSITY, value & MAX7219_MASK_INTENSITY)

/**
 * @brief Macro to set the scan limit.
 * @param[in] pSelf Pointer to an instance.
 * @param[in] value A value between [0, 7].
 */
#define MAX7219_setScanLimit(pSelf, value)	\
	max7219_writeToAll(pSelf, MAX7219_REG_SCAN_LIMIT, value & MAX7219_MASK_SCAN_LIMIT)

/**
 * @brief Macro to enable/disable MAX7219.
 * @param[in] pSelf Pointer to an instance.
 * @param[in] onOrOff Either MAX7219_VALUE_ON or MAX7219_VALUE_OFF.
 */
#define MAX7219_setDisplayEnabled(pSelf, onOrOff)	\
	max7219_writeToAll(pSelf, MAX7219_REG_SHUTDOWN, onOrOff)

/**
 * @brief Macro to enable or disable display test.
 * @param[in] pSelf Pointer to an instance.
 * @param[in] onOrOff Either MAX7219_VALUE_ON or MAX7219_VALUE_OFF.
 */
#define MAX7219_testDisplay(pSelf, onOrOff)	\
	max7219_writeToAll(pSelf, MAX7219_REG_DISPLAY_TEST, onOrOff)

/**
 * @brief Macro to fill all cascaded MAX7219 with the given data.
 * @param[in] pSelf Pointer to an instance.
 */
#define MAX7219_clear(pSelf)	\
	max7219_fill(pSelf, 0)

typedef struct max7219_s max7219_t;

typedef struct {
	/* Number of cascaded MAX7219 hardware. */
	uint32_t numCascade;
	/* SPI index to be used for communicating with the max7219. */
	uint8_t spiIdx;
	/* CS index to be used for SPI communication. */
	uint8_t cs;
} max7219Cfg_t;

/**
 * @brief Initialise max7219 instance.
 * @param[out] ppSelf Pointer to store the created and initialised max7219 instance.
 * @param[in] cfg Configuration struct for creating a max7219_t instance.
 * @return Status code.
 */
int32_t max7219_create(max7219_t **ppSelf, const max7219Cfg_t *pCfg);

/**
 * @brief Close max7219 instance. Releasing all its memory.
 * @param[in/out] ppSelf Max7219 instance to be closed. Once successfully closed,
 * 		*ppSelf will be NULL.
 * @return Status code.
 */
int32_t max7219_destroy(max7219_t **ppSelf);

/**
 * @brief Write a single reg-value to all cascaded MAX7219 hardware.
 * @param[in] pSelf Pointer to a MAX7219 instance.
 * @param[in] regAddr Register address to write to.
 * @param[in] value Value to write.
 * @return Status code.
 */
int32_t max7219_writeToAll(const max7219_t* pSelf, uint8_t regAddr, uint8_t value);

/**
 * @brief Write data to cascaded MAX7219.
 * @details This function packs the data with their register address and write to MAX7219.
 * 		The data itself must have been decoded/arranged in the layout as how the cascaded
 * 		MAX7219 hardware are connected.
 * @param[in] pSelf Pointer to a MAX7219 instance.
 * @param[in] data Dot matrix display data to be written.
 * @param[in] len Length of data to be written.
 * @return Status code.
 */
int32_t max7219_write(const max7219_t* pSelf, const uint8_t* data, uint32_t len);

/**
 * @brief Fill display with the given data.
 * @param[in] pSelf Pointer to a MAX7219 instance.
 * @param[in] data Data to be filled.
 * @return Status code.
 */
int32_t max7219_fill(const max7219_t* pSelf, const uint8_t data);

/**
 * @brief Get the number of cascaded MAX7219 hardware.
 * @param[in] pSelf Pointer to a MAX7219 instance.
 * @return Number of cascaded MAX7219.
 */
uint32_t max7219_getNumCascade(const max7219_t* pSelf);


#endif /* SRC_MAX7219_H_ */
