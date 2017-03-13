/*
 * gpio.h
 *
 *  Created on: 19 Sep 2016
 *      Author: chiong
 *
 *  GPIO wrapper for target dependent GPIO middleware. Hardware initialisation and configuration
 *  are done external to this wrapper (in application layer). This interface is only to wrap
 *  the functionalities of different target dependent middleware hardware peripheral.
 *
 *  Feature NOT supported:
 *  1. Write to multiple GPIO pin at once, even though they are on the same register.
 *  2. Read from multiple GPIO pin at once, even though they are on the same register.
 */

#ifndef INC_GPIO_H_
#define INC_GPIO_H_

#include <stdint.h>

#if 0
typedef enum {
	GPIO_DIR_INPUT = 0,
	GPIO_DIR_OUTPUT = 1
} gpioDir_t;

typedef enum {
	GPIO_PULL_UP     = 0,
	GPIO_PULL_DOWN   = 1,
	GPIO_PULL_UPDOWN = 2
} gpioPud_t;

typedef struct {
	gpioDir_t dir;
	gpioPud_t pud;
} gpioCfg_t;


/**
 * @brief Initialise the GPIO wrapper.
 * @details Must be call before using any GPIO function in this wrapper.
 * @returns Returns STATUS_OK if succeed, STATUS_ERROR otherwise.
 */
int32_t gpio_open(void);

/**
 * @brief Configure the given GPIO pin ID with the given pin mode.
 * @param[in] pinId GPIO pin ID to be configured.
 * @param[in] pinMode GPIO pin mode.
 * @returns STATUS_OK if successful, STATUS_ERROR otherwise.
 */
int32_t gpio_config(uint32_t pinId, uint32_t pinMode);

/**
 * @brief Clean up the GPIO wrapper.
 * @details Must be called to release all resources on hardware GPIO. Once this
 *      function has been called, no function in this wrapper can be called anymore.
 */
void gpio_close(void);

#endif

/**
 * @brief Write to the given GPIO.
 * @param[in] pinId GPIO pin ID to be written.
 * @param[in] value HIGH to write logic 1, LOW for logic 0.
 */
void gpio_write(uint32_t pinId, uint8_t value);

/**
 * @brief Read the logic for the given GPIO.
 * @param[in] pinId GPIO pin ID to be read.
 * @returns HIGH if logic 1, LOW for logic 0.
 */
uint8_t gpio_read(uint32_t pinId);

/**
 * @brief Toggle the logic for the givn GPIO.
 * @param[in] pinID GPIO pin ID to be toggled.
 */
void gpio_toggle(uint32_t pinId);

#endif /* INC_GPIO_H_ */
