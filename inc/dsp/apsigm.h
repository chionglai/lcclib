/*
 * apsigm.h
 *
 *  Created on: 20 Nov 2016
 *      Author: chiong
 *
 *  Noise suppression using apriori sigmoid function. Single precision
 *  (float) implementation only.
 *
 */

#ifndef INC_APSIGM_H_
#define INC_APSIGM_H_

#include <stdint.h>
#include "dsp/signal.h"

#include "3p-lib/fftw-3.3.5/inc/fftw3.h"

typedef struct {
    /* See Matlab code for the definition of these fields */
    /* Time constants */
    float ts;
    float nn1Tc, nn2Tc, nn3Tc;
    float xxTc, x1Tc, x2Tc, x3Tc;
    float n1Tc, n2Tc;

    float pre, post;
    float AAprior, apriori_floor, priorFact;
    float xiOpt;
    float alphaPsd;
    float p;
    float gain;

    /* Windows */
    float *fftWin, *ifftWin;
    fftwf_complex *win;

    /* Which channel to use as reference, channel start with index 0 */
    uint32_t refMic;
    /* 1 to use post filter, 0 to skip */
    uint8_t wpost;
    /* Duration, in number of frames, for initialisation (i.e. noise estimation) */
    uint32_t initDuration;

    uint32_t channel;
    uint32_t frameSize;
    uint32_t sampleRate;
} apsigmCfg_t;

typedef struct apsigm_s apsigm_t;

/* Default configuration that works. */
static apsigmCfg_t APSIGM_DEFAULT_CFG = {
        .pre = 0.056f, 	// pow(10.0f, -1.25f)
        .post = 0.33f,
        .AAprior = 0.98f,
        .apriori_floor = 0.003f, // pow(10.0f, -2.5f)
        .priorFact = 0.3f/0.7f,
        .xiOpt = 50.12f, 	// pow(10.0f, 1.7f)
        .alphaPsd = 0.8f,
        .p = 2.0f,
        .ts = 0.02f,
        .nn1Tc = 0.05f,
        .nn2Tc = 0.08f,
        .nn3Tc = 240.0f,
        .xxTc = 0.02f,
        .x1Tc = 0.05f,
        .x2Tc = 0.08f,
        .x3Tc = 240.0f,
        .n1Tc = 0.3f,
        .n2Tc = 2.0f,
		.gain = 1.0f,
        .fftWin = NULL,
        .ifftWin = NULL,
        .win = NULL,
        .refMic = 0,
		.wpost = 1,
		.initDuration = 20,

        /* Must be set explicitly. */
        .channel = 2,
        .frameSize = 128,
        .sampleRate = 8000
};

/**
 * @brief Create an apsigm_t instance.
 * @param[in/out] ppApsigm Address to store a newly created apsigm_t instance.
 * @param[in] cfg Configuration used to create an apsigm_t instance.
 * @return STATUS_OK if successful, STATUS_ERROR* otherwise.
 */
int32_t apsigm_create(apsigm_t **ppApsigm, const apsigmCfg_t *cfg);

/**
 * @brief To destroy an apsigm_t instance and release its resources.
 * @param[in/out] ppApsigm Address of an apsigm_t instance to be destroyed.
 * @return STATUS_OK if successful, STATUS_ERROR* otherwise.
 */
int32_t apsigm_destroy(apsigm_t **ppApsigm);

/**
 * @brief Process a single frame of input signal with apsigm_t instance.
 * @param[in/out] apsigm An apsigm_t instance.
 * @param[out] out Single channel output frame.
 * @param[in] in Multi-channel input frames, with [channelIdx][sampleIdx]
 * @param[in] nSample Number of sample of input frames.
 * @return STATUS_OK if successful, STATUS_ERROR* otherwise.
 */
int32_t apsigm_process(apsigm_t *apsigm, realf_t *out, realf_t **in, uint32_t nSample);

/**
 * @brief Set output gain for APSIGM algorithm.
 * @param[in/out] apsigm An apsigm_t instance.
 * @param[in] gain Gin in linear scale.
 * @return STATUS_OK if successful, STATUS_ERROR* otherwise.
 */
int32_t apsigm_setGain(apsigm_t *apsigm, float gain);

int32_t apsigm_getChannelCount(const apsigm_t *apsigm);

#endif /* INC_APSIGM_H_ */
