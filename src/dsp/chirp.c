/*
 * chirp.c
 *
 *  Created on: 19 Sep 2016
 *      Author: chiong
 */

#include <stdlib.h>
#include <stddef.h>

#include "math/fimath.h"
#include "dsp/chirp.h"

struct chirp_s {
    /* Initial frequency in 1q31, normalised to sampling frequency. */
    uint32_t freq0;
    /* Linear frequency step in 1q31, normalised to sampling frequency. */
    uint32_t freqStep;
    /* First stage integrator output (i.e. instantaneous frequency, normalised to
     * sampling rate), in 1q31 */
    uint32_t freq;
    /* Second stage integrator output (i.e. instantaneous phase, normalised to
     * [-pi, pi)), in 1q31 */
    uint32_t phase;
};

void chirp_create(chirp_t **pp_chirp, const chirp_cfg_t *p_chirp_cfg) {
    chirp_t *p_chirp;
    float ts;

    p_chirp = (chirp_t*) malloc(sizeof(chirp_t));
    *pp_chirp = p_chirp;
    if (p_chirp != NULL) {
        /* 1. Since the fimath sine argument is from [-1, 1), which is mapped to
         *    [-pi, pi), the continuous time argument of 2*pi*f/fs is only f/fs.
         * 2. Both freq0 and freqStep are internally stored as 1q31.
         * 3. For linear frequency sweep,
         *        f(t) = freqStep * t + freq0,
         *        f[n] = (freqStep*ts) * n + freq0
         *      phi(t) = freqStep/2 * t^2 + freq0*t,
         *      phi[n] = (freqStep/2 * ts^2) * n^2 + (freq0*ts) * n
         *           t = n * ts
         *    Sampling period, ts has been kept as float since for ts = 1/48000, a 1q15
         *    fixed point barely have any precision to represent the value. Unless, it
         *    is stored as 1q31, which results in 2q62 for ts^2. */
        ts = 1.0f / p_chirp_cfg->fs;
        p_chirp->freq0 = (uint32_t) (p_chirp_cfg->freq0 * ts * FIMATH_MAX32);
        p_chirp->freqStep = (uint32_t) (p_chirp_cfg->freqStep * ts * ts * FIMATH_MAX32);

        chirp_reset(p_chirp);
    }
}

void chirp_destroy(chirp_t **pp_chirp) {
    chirp_t *p_chirp = *pp_chirp;
    if (p_chirp != NULL) {
        free(p_chirp);
        *pp_chirp = NULL;
    }
}

void chirp_reset(chirp_t *p_chirp) {
    if (p_chirp != NULL) {
        p_chirp->freq = p_chirp->freq0;
        p_chirp->phase = 0;
    }
}

uint32_t chirp_getFrame(chirp_t *p_chirp, int32_t *buffer, uint32_t count) {
    uint32_t i;

    for (i = 0; i < count; i++) {
        buffer[i] = fimath_sin((int32_t) p_chirp->phase);

        p_chirp->freq += p_chirp->freqStep;
        p_chirp->phase += p_chirp->freq;
        p_chirp->phase &= FIMATH_MAX32;
    }

    return i;
}
