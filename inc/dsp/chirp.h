/*
 * chirp.h
 *
 *  Created on: 19 Sep 2016
 *      Author: chiong
 *
 *  Implementation of chirp generator in 32-bit fixed point.
 */

#ifndef INC_CHIRP_H_
#define INC_CHIRP_H_

#include <stdint.h>

typedef struct {
    /* Initial frequency, in Hz. */
    float freq0;
    /* Linear frequency step, in Hz. */
    float freqStep;
    /* Samping frequency, in Hz. */
    float fs;
} chirp_cfg_t;

typedef struct chirp_s chirp_t;

/**
 * @brief Create a chirp generator object with the given configuration.
 * @param[out] pp_chirp Address to store the newly created chirp generator object.
 *      NULL if object failed to be created.
 * @param[in] p_chirp_cfg Configuration to create chirp generator object.
 */
void chirp_create(chirp_t **pp_chirp, const chirp_cfg_t *p_chirp_cfg);

/**
 * @brief Destroy a chirp generator object and free all its memory.
 * @param[in/out] pp_chirp Address containing the chirp generator object to be
 *      destroyed. Once successfully destroyed, *pp_chirp will be NULL.
 */
void chirp_destroy(chirp_t **pp_chirp);

/**
 * @brief Reset the chirp generator object such that the next call to getFrame()
 *      will return the first frame as if the object is first created.
 * @param[in/out] p_chirp Chirp generator object to be reset.
 */
void chirp_reset(chirp_t *p_chirp);

/**
 * @brief Get a frame of chirp signal.
 * @details Keep calling this function to get the successive frame to generate a chirp
 *      signal.
 * @param[in/out] p_chirp Chirp generator object.
 * @param[out] buffer Buffer to contain the current frame.
 * @param[in] size Size, in number of element, of buffer.
 * @return Number of sample actually generated and put into buffer.
 */
uint32_t chirp_getFrame(chirp_t *p_chirp, int32_t *buffer, uint32_t size);


#endif /* INC_CHIRP_H_ */
