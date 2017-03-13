/*
 * pwm.h
 *
 *  Created on: 19 Sep 2016
 *      Author: chiong
 *
 *  PWM wrapper for target dependent PWM middleware. Hardware initialisation and configuration
 *  are done external to this wrapper (in application layer). This interface is only to wrap
 *  the functionalities of different target dependent middleware hardware peripheral.
 *
 */

#ifndef INC_PWM_H_
#define INC_PWM_H_

#include <stdint.h>

#if 0
typedef enum {
    PWM_POL_ACTIVE_LOW = 0,
    PWM_POL_ACTIVE_HIGH = 1
} pwmPolarity_t;

typedef struct {
    /* PWM period is microseconds. */
    uint32_t period;
    /* PWM polarity. */
    pwmPolarity_t polarity;
} pwmCfg_t;

/**
 * @brief Initialise the PWM wrapper.
 * @details Must be call before using any PWM function in this wrapper.
 * @returns The number of available hardware PWM peripheral. Returns STATUS_ERROR if
 *      fails.
 */
int32_t pwm_open(void);

/**
 * @brief Configure the given PWM.
 * @param[in] pwmIdx Index for the hardware PWM to be configured. First
 *      index is 0, and must be less than the number returned by pwm_open().
 * @param[in] pwmCfg PWM configuration.
 * @returns STATUS_OK if succeed, STATUS_ERROR otherwise.
 */
int32_t pwm_config(uint8_t pwmIdx, const pwmCfg_t *cfg);

/**
 * @brief Clean up the PWM wrapper.
 * @details Must be called to release all resources on hardware PWM. Once this
 *      function has been called, no function in this wrapper can be called anymore.
 */
void pwm_close(void);

#endif

/**
 * @brief Start the PWM.
 * @param[in] pwmIdx Index for the hardware PWM to be configured. First
 *      index is 0, and must be less than the number returned by pwm_open().
 * @returns STATUS_OK if succeed, STATUS_ERROR otherwise.
 */
int32_t pwm_start(uint8_t pwmIdx);

/**
 * @brief Stop the PWM.
 * @param[in] pwmIdx Index for the hardware PWM to be configured. First
 *      index is 0, and must be less than the number returned by pwm_open().
 * @returns STATUS_OK if succeed, STATUS_ERROR otherwise.
 */
int32_t pwm_stop(uint8_t pwmIdx);

/**
 * @brief Set the duty cycle of the PWM.
 * @details The new duty cycle will take effect immediately if the PWM has
 *      been started.
 * @param[in] pwmIdx Index for the hardware PWM to be configured. First
 *      index is 0, and must be less than the number returned by pwm_open().
 * @param[in] dutyCycle New duty cycle, in 0q16 format, scaled to [0, 1).
 *      E.g. 65535 == 99.x% and 32768 == 50%.
 * @returns STATUS_OK if succeed, STATUS_ERROR otherwise.
 */
int32_t pwm_setDutyCycle(uint8_t pwmIdx, uint16_t dutyCycle);

#endif  /* INC_PWM_H_ */
