/*
 * apsigm.c
 *
 *  Created on: 20 Nov 2016
 *      Author: chiong
 */

#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "util/status.h"
#include "util/stack.h"
#include "dsp/apsigm.h"

#include "3p-lib/fftw-3.3.5/inc/fftw3.h"
/* cblas requires the following lib:
 * libclas.a, libblas.a libgfortran.a */
#include "3p-lib/lapack-3.6.1/inc/cblas.h"

/* Constant for LAPACKE and CBLAS operations */
/* For inverse matrix */
#define INV_MAT_LAYOUT		(CblasColMajor)
#define INV_MAT_UPLO		(CblasUpper)
#define INV_MAT_TRAN		(CblasNoTrans)
#define INV_MAT_DIAG		(CblasNonUnit)
#define INV_MAT_ALPHA		(1.0f)
#define INV_MAT_INCX		(1)

/* Alginment base on sizeof(float complex) */
#define APSIGM_STACK_ALIGNMENT  (sizeof(float complex))

typedef enum {
	APSIGM_INIT   = 0,	/**< Initial estimation state */
	APSIGM_NORMAL = 1	/**< Normal suppression state, after count exceed init duration */
} apsigmState_t;

struct apsigm_s {
	/* Number of input channel. */
	uint32_t channel;
	/* Frame or cloack size, in number of samples. */
	uint32_t frameSize;
	/* Channel index to be used as reference mic. */
	uint32_t refmic;
	/* 1 to use post filter, 0 to skip. */
	uint8_t wpost;
	/* Count to determine if the algorithm is in init state or normal state. */
	uint32_t count;
	/* Initialisation duration, in number of frames */
	uint32_t initDuration;

	/* Current algorithm state */
	apsigmState_t state;

	/* real signal input buffer, multi-channel */
	float **inbuf;
	/* real signal output buffer, single-channel */
	float *outbuf;
	/* buffer for overlap and add */
	float *ONSbuf;
	/* FFT window. Same for all channels. */
	float *fftwin;
	/* IFFT window. */
	float *ifftwin;
	/* FFT buffer for input, multi-channel */
	fftwf_complex **infftbuf;
	/* FFT buffer for output, single channel */
	fftwf_complex *outfftbuf;
	/* FFT plan. One per input channel. */
	fftwf_plan *fft_plan;
	/* IFFT plan. Single output only. */
	fftwf_plan ifft_plan;
	/** complex window, if NULL, then no windowing == rectangular window */
	fftwf_complex *win;

	float complex **Rsd;
	float complex **Rss;

	/**
	 * Variables for processing. Refer Matlab code sig_apriori_multichannels4.m
	 * for the definition of these variables
	 */
	float *pn, *ps;	// vector, for each freq. point
	float *Gv, *Gf, *Gvp;
	float *a12p, *a22p;
	float pre;			// pre-filter noise floor
	float post;			// post-filter noise floor
	float ts;
	float as;
	float AAprior;
	float apriori_floor;
	float priorFact;
	float xiOpt;
	float siga, sigc;

	float alphaPSD;
	float gain;
	float p;

	float eta_nn1;
	float eta_nn2;
	float eta_nn3;
	float eta_xx;
	float eta_x1;
	float eta_x2;
	float eta_x3;
	float eta_n1;
	float eta_n2;

	/* Stack for temporary variable */
	vStack_t *stack;
};

int32_t apsigm_create(apsigm_t **ppApsigm, const apsigmCfg_t *cfg) {
	apsigm_t *apsigm;
	int32_t status;
	vStackCfg_t stackCfg;
	float temp;
	uint32_t i;
	uint32_t packedSize;

	apsigm = (apsigm_t*) malloc(sizeof(apsigm_t));
	if (NULL == apsigm) {
		return STATUS_ERROR_MALLOC;
	}
	*ppApsigm = apsigm;

	/**
	 * Size for:
	 * 1. X1 = number of channels
	 * 2. Ws = number of channels
	 * 3. XX = (channel * (channel + 1))/2
	 */
	stackCfg.size = APSIGM_STACK_ALIGNMENT *
	        (cfg->channel +
	         cfg->channel +
	         (cfg->channel * (cfg->channel + 1))/2 +
			 8	/* Random value as margin */
	        );
	stackCfg.alignment = APSIGM_STACK_ALIGNMENT;
	status = stack_create(&apsigm->stack, &stackCfg);
	if (STATUS_OK != status) {
	    apsigm_destroy(&apsigm);
	    return status;
	}

	/* Initialise component parameters */
	apsigm->channel = cfg->channel;
	apsigm->frameSize = cfg->frameSize;
	apsigm->refmic = cfg->refMic;
	apsigm->wpost = cfg->wpost;
	apsigm->initDuration = cfg->initDuration;
	apsigm->pre = cfg->pre;
	apsigm->post = cfg->post;
	apsigm->AAprior = cfg->AAprior;
	apsigm->apriori_floor = cfg->apriori_floor;
	apsigm->priorFact = cfg->priorFact;
	apsigm->xiOpt = cfg->xiOpt;
	apsigm->alphaPSD = cfg->alphaPsd;
	apsigm->p = cfg->p;
	apsigm->gain = cfg->gain;
	apsigm->fftwin = cfg->fftWin;
	apsigm->ifftwin = cfg->ifftWin;
	apsigm->win = cfg->win;

	apsigm->siga = apsigm->xiOpt / (1.0f + apsigm->xiOpt);
	apsigm->sigc = log(apsigm->priorFact * (1.0f + apsigm->xiOpt)) / apsigm->siga;

	temp = cfg->sampleRate / cfg->frameSize;
	apsigm->ts = cfg->ts;
	apsigm->as = exp(-2.2 / (temp * apsigm->ts));
	apsigm->eta_nn1 = exp(-2.2 / (temp * cfg->nn1Tc));
	apsigm->eta_nn2 = exp(-2.2 / (temp * cfg->nn2Tc));
	apsigm->eta_nn3 = exp(-2.2 / (temp * cfg->nn3Tc));
	apsigm->eta_xx = exp(-2.2 / (temp * cfg->xxTc));
	apsigm->eta_x1 = exp(-2.2 / (temp * cfg->x1Tc));
	apsigm->eta_x2 = exp(-2.2 / (temp * cfg->x2Tc));
	apsigm->eta_x3 = exp(-2.2 / (temp * cfg->x3Tc));
	apsigm->eta_n1 = exp(-2.2 / (temp * cfg->n1Tc));
	apsigm->eta_n2 = exp(-2.2 / (temp * cfg->n2Tc));

	/* Allocate component variables */
	apsigm->pn = (float*) calloc(apsigm->frameSize + 1, sizeof(float));
	apsigm->ps = (float*) calloc(apsigm->frameSize + 1, sizeof(float));
	apsigm->Gv = (float*) calloc(apsigm->frameSize + 1, sizeof(float));
	apsigm->Gf = (float*) calloc(apsigm->frameSize + 1, sizeof(float));
	apsigm->Gvp = (float*) calloc(apsigm->frameSize + 1, sizeof(float));
	apsigm->a12p = (float*) calloc(apsigm->frameSize + 1, sizeof(float));
	apsigm->a22p = (float*) calloc(apsigm->frameSize + 1, sizeof(float));

	apsigm->inbuf = (float**) calloc(apsigm->channel, sizeof(float*));
	apsigm->outbuf = (float*) calloc(2*apsigm->frameSize, sizeof(float));
	apsigm->ONSbuf = (float*) calloc(2*apsigm->frameSize, sizeof(float));
	apsigm->infftbuf = (fftwf_complex**) fftwf_malloc(apsigm->channel * sizeof(fftwf_complex*));
	apsigm->outfftbuf = (fftwf_complex*) fftwf_malloc((apsigm->frameSize + 1) * sizeof(fftwf_complex));

	apsigm->fft_plan = (fftwf_plan*) malloc(sizeof(fftwf_plan) * apsigm->channel);

	if (NULL == apsigm->pn ||
		NULL == apsigm->ps ||
		NULL == apsigm->Gv ||
		NULL == apsigm->Gf ||
		NULL == apsigm->Gvp ||
		NULL == apsigm->a12p ||
		NULL == apsigm->a22p ||
		NULL == apsigm->inbuf ||
		NULL == apsigm->outbuf ||
		NULL == apsigm->ONSbuf ||
		NULL == apsigm->infftbuf ||
		NULL == apsigm->outfftbuf ||
		NULL == apsigm->fft_plan) {
		apsigm_destroy(&apsigm);
		return STATUS_ERROR_MALLOC;
	}

	for (i = 0; i < apsigm->channel; i++) {
		apsigm->inbuf[i] = (float*) calloc(2*apsigm->frameSize, sizeof(float));
		apsigm->infftbuf[i] = (fftwf_complex*) fftwf_malloc((apsigm->frameSize + 1) * sizeof(fftwf_complex));
		apsigm->fft_plan[i] = fftwf_plan_dft_r2c_1d(2*apsigm->frameSize, apsigm->inbuf[i], apsigm->infftbuf[i], FFTW_MEASURE);
	}

	apsigm->ifft_plan = fftwf_plan_dft_c2r_1d(2*apsigm->frameSize, apsigm->outfftbuf, apsigm->outbuf, FFTW_MEASURE);

	apsigm->Rsd = (float complex**) malloc(sizeof(float complex*) * (apsigm->frameSize + 1));
	apsigm->Rss = (float complex**) malloc(sizeof(float complex*) * (apsigm->frameSize + 1));

	packedSize = apsigm->channel * (apsigm->channel + 1) / 2;
	for (i = 0; i < apsigm->frameSize + 1; i++) {
		apsigm->Rsd[i] = (float complex*) calloc(sizeof(float complex), apsigm->channel);
		apsigm->Rss[i] = (float complex*) calloc(sizeof(float complex), packedSize);
	}

	apsigm->count = 0;
	apsigm->state = APSIGM_INIT;

	return STATUS_OK;
}

int32_t apsigm_destroy(apsigm_t **ppApsigm) {
	apsigm_t *apsigm;
	uint32_t i;

	apsigm = *ppApsigm;
	if (NULL != apsigm) {
	    stack_destroy(&apsigm->stack);

	    fftwf_destroy_plan(apsigm->ifft_plan);

	    /* Free per channel memories */
	    for (i = 0; i < apsigm->channel; i++) {
	    	if (NULL != apsigm->inbuf[i])
	    		free(apsigm->inbuf[i]);
	    	if (NULL != apsigm->infftbuf[i])
				fftwf_free(apsigm->infftbuf[i]);
    		fftwf_destroy_plan(apsigm->fft_plan[i]);
	    }
	    if (NULL != apsigm->inbuf)
	    	free(apsigm->inbuf);
	    if (NULL != apsigm->infftbuf)
			fftwf_free(apsigm->infftbuf);
	    if (NULL != apsigm->fft_plan)
			free(apsigm->fft_plan);

	    /* Free per sample memories */
	    for (i = 0; i < apsigm->frameSize+1; i++) {
			if (NULL != apsigm->Rsd[i])
				free(apsigm->Rsd[i]);
			if (NULL != apsigm->Rss[i])
				free(apsigm->Rss[i]);
		}
		if (NULL != apsigm->Rsd)
			free(apsigm->Rsd);
		if (NULL != apsigm->Rss)
			free(apsigm->Rss);

		/* Free memories */
		if (NULL != apsigm->pn)
			free(apsigm->pn);
		if (NULL != apsigm->ps)
			free(apsigm->ps);
		if (NULL != apsigm->Gv)
			free(apsigm->Gv);
		if (NULL != apsigm->Gf)
			free(apsigm->Gf);
		if (NULL != apsigm->Gvp)
			free(apsigm->Gvp);
		if (NULL != apsigm->a12p)
			free(apsigm->a12p);
		if (NULL != apsigm->a22p)
			free(apsigm->a22p);
		if (NULL != apsigm->outbuf)
			free(apsigm->outbuf);
		if (NULL != apsigm->ONSbuf)
			free(apsigm->ONSbuf);
		if (NULL != apsigm->outfftbuf)
			fftwf_free(apsigm->outfftbuf);

		free(apsigm);
		*ppApsigm = NULL;
	}

	return STATUS_OK;
}

int32_t apsigm_process(apsigm_t *apsigm, realf_t *out, realf_t **in, uint32_t nSample) {
	int32_t cf;		// frame counter
	int32_t cc;		// channel counter

	int32_t Nc;
	float absXp;
	float snrPost1;
	float sigmf;
	float complex Dref;
	float Gamma;
	float sN, sNss, sNs3;
	float estimate, POST, AP, xi;
	float Gsp;
	float complex Zf;

	float complex *X1;
	float complex *Ws;
	float complex *XX;

	uint32_t packedSize;

	/* Allocate temporary variables */
	stack_openFrame(apsigm->stack);
	X1 = (float complex*) stack_alignedAlloc(apsigm->stack, sizeof(float complex), apsigm->channel);
	Ws = (float complex*) stack_alignedAlloc(apsigm->stack, sizeof(float complex), apsigm->channel);
	packedSize = apsigm->channel * (apsigm->channel + 1) / 2;
	XX = (float complex*) stack_alignedAlloc(apsigm->stack, sizeof(float complex), packedSize);

	// for each channel perform 1D FFT
	for (cc = 0; cc < apsigm->channel; cc++) {
		if (apsigm->fftwin != NULL) {
			/* apply fft window */
			// old data
			for (cf = 0; cf < nSample; cf++)
				apsigm->inbuf[cc][cf] *= apsigm->fftwin[cf];

			// new data
			for (cf = nSample; cf < 2*nSample; cf++)
				apsigm->inbuf[cc][cf] = in[cc][cf-nSample] * apsigm->fftwin[cf];
		} else {
			/* Do nothing: unity rectangular window */
			// new data
			for (cf = nSample; cf < 2*nSample; cf++)
				apsigm->inbuf[cc][cf] = in[cc][cf-nSample];
		}

		fftwf_execute (apsigm->fft_plan[cc]);	// perform FFT

		// slide in old data (new data now becomes old). Note that this does not
		// matter if fft_plan destroy the input data or not, since previously old
		// data is all shifted out, due to 50% overlap. If not, FFT plan will need
		// to be set to preserve input data.
		memcpy(apsigm->inbuf[cc], in[cc], nSample * sizeof(float));
	}

	// for each frequency point, perform task
	Nc = nSample + 1;

	switch(apsigm->state) {
	case APSIGM_INIT:
		if (++apsigm->count > apsigm->initDuration)
			apsigm->state = APSIGM_NORMAL;
		break;
	default:
		/* Do nothing for default */
	    break;
	}

	for (cf = 0; cf < Nc; cf++) {
		// SINGLE CHANNEL ENHANCEMENT
		if (apsigm->win != NULL) {		// applying window
			apsigm->infftbuf[apsigm->refmic][cf] *= apsigm->win[cf];
		}

		absXp = pow(cabs(apsigm->infftbuf[apsigm->refmic][cf]), apsigm->p);
		snrPost1 = absXp/apsigm->pn[cf];

		// sNss is updated before Gv update
		if (apsigm->Gv[cf] <= 0.2)
			sNss = apsigm->eta_xx;
		else if (apsigm->Gv[cf] <= 0.5)
			sNss = apsigm->eta_xx;
		else 	// Gv[cf] > 0.5
			sNss= apsigm->eta_xx;

		// sNs3 is updated before Gv update
		if (apsigm->Gv[cf] <= 0.3)
			sNs3 = apsigm->eta_x1;
		else if (apsigm->Gv[cf] <= 0.6)
			sNs3 = apsigm->eta_x2;
		else  	// Gv[cf] > 0.6
			sNs3 = apsigm->Gv[cf] < apsigm->eta_x3? apsigm->Gv[cf] : apsigm->eta_x3;

		apsigm->Gv[cf] = 1.0/(1.0+exp(-apsigm->siga*(snrPost1 - apsigm->sigc)));

		// sN is updated after Gv update
		if (apsigm->Gv[cf] <= 0.3)
			sN = apsigm->eta_nn1;
		else if (apsigm->Gv[cf] <= 0.6)
			sN = apsigm->eta_nn2;
		else 	// Gv[cf] > 0.6
			sN = apsigm->Gv[cf] < apsigm->eta_nn3? apsigm->Gv[cf] : apsigm->eta_nn3;

		switch(apsigm->state) {
		case APSIGM_INIT:
			apsigm->pn[cf] = (apsigm->pn[cf]*(apsigm->count - 1.0f) + absXp)/apsigm->count;	// sliding window average
			break;
		case APSIGM_NORMAL:
			estimate = sN*apsigm->pn[cf] + (1.0f - sN)*absXp;
			apsigm->pn[cf] = apsigm->alphaPSD * apsigm->pn[cf] + (1.0f - apsigm->alphaPSD)*estimate;
			break;
			/* Do nothing for default */
		}

		apsigm->ps[cf] = apsigm->as * apsigm->ps[cf] + (1.0 - apsigm->as)*absXp;

		POST = apsigm->ps[cf]/apsigm->pn[cf] - 1.0;
		if (POST < 0)
			POST = 0.0;

		absXp = pow(cabs(apsigm->Gf[cf] * apsigm->infftbuf[apsigm->refmic][cf]), apsigm->p);
		AP = apsigm->AAprior*absXp / apsigm->pn[cf] + (1.0f - apsigm->AAprior)*POST;
		xi = AP > apsigm->apriori_floor? AP : apsigm->apriori_floor;

		switch(apsigm->state) {
		case APSIGM_INIT:
			apsigm->Gf[cf] = xi/(1.0f + xi);
			break;
		case APSIGM_NORMAL:
			apsigm->Gf[cf] = (1.0f - exp(-3.0f * xi))/(1.0f + exp(-3.0f * xi))/(1.0f + exp(-xi + 0.7f));
			break;
			/* Do nothing for default */
		}

		if (apsigm->Gf[cf] < apsigm->pre)	// cap at pre
			apsigm->Gf[cf] = apsigm->pre;

		Dref = conj(apsigm->Gf[cf]*apsigm->infftbuf[apsigm->refmic][cf]);

		// ALTERNATIVE MULTICHANNEL WIENER FILTER
		for (cc = 0; cc < apsigm->channel; cc++) {
			if(apsigm->win != NULL && cc != apsigm->refmic)
				apsigm->infftbuf[cc][cf] *= apsigm->win[cf];	// applying window

			X1[cc] = apsigm->infftbuf[cc][cf];
			apsigm->Rsd[cf][cc] = (1.0-sNss)*apsigm->Rsd[cf][cc] + sNss*X1[cc]*Dref;
		}

		memcpy(Ws, apsigm->Rsd[cf], apsigm->channel*sizeof(float complex));

		if (APSIGM_NORMAL == apsigm->state) {
			// matrix operation, XX = X1*X1'
			memset(XX, 0, sizeof(float complex)*packedSize); 	// clear matrix XX before operation

			cblas_chpr(INV_MAT_LAYOUT, INV_MAT_UPLO, apsigm->channel, INV_MAT_ALPHA, X1, INV_MAT_INCX, XX);

			// zhpr_ calculates XX = alpha*X1*X1' + XX, so need to clear XX every time before calc

			for (cc = 0; cc < packedSize; cc++) {
				apsigm->Rss[cf][cc] = (1.0 - apsigm->eta_xx*sNs3)*apsigm->Rss[cf][cc]
					+ apsigm->eta_xx*sNs3*XX[cc];
			}

			// hermitian A^(-1)*X
			cblas_ctpsv(INV_MAT_LAYOUT, INV_MAT_UPLO, INV_MAT_TRAN, INV_MAT_DIAG, apsigm->channel, apsigm->Rss[cf], Ws, INV_MAT_INCX);
		}

		cblas_cdotc_sub(apsigm->channel, Ws, INV_MAT_INCX, X1, INV_MAT_INCX, &Zf);

		if (!apsigm->wpost) {	// if post filter disabled
			apsigm->outfftbuf[cf] = Zf;
			continue;
		}

		absXp = pow(cabs(Zf), apsigm->p);
		apsigm->a12p[cf] = apsigm->as*apsigm->a12p[cf] + (1.0 - apsigm->as)*absXp;

		if (apsigm->Gvp[cf] <= 0.3)
			sN = apsigm->eta_x1;
		else if (apsigm->Gvp[cf] <= 0.6)
			sN = apsigm->eta_x2;
		else // Gvp[cf] > 0.6
			sN = apsigm->Gvp[cf] < apsigm->eta_x3? apsigm->Gvp[cf] : apsigm->eta_x3;

		switch(apsigm->state) {
		case APSIGM_INIT:
			apsigm->a22p[cf] = apsigm->a12p[cf];
			break;
		case APSIGM_NORMAL:
			apsigm->a22p[cf] = sN * apsigm->a22p[cf] + (1.0f - sN) * absXp;
			break;
			/* Do nothing for default */
		}

		Gamma = apsigm->a12p[cf]/apsigm->a22p[cf];
		sigmf = 1.0/(1.0+exp(-5.0*(Gamma + 1e-18 - 1.4)));
		apsigm->Gvp[cf] = (sigmf > 0.05)? sigmf : 0.05;
		sigmf = 1.0/(1.0+exp(-3.0*(Gamma - 2.5)));
		Gsp = (sigmf > apsigm->post)? sigmf : apsigm->post;

		apsigm->outfftbuf[cf] = Gsp*Zf;
	}

	// IFFT output
	fftwf_execute(apsigm->ifft_plan);

	// put data to output
	if (apsigm->ifftwin != NULL) {
		for (cf = 0; cf < nSample; cf++) {
			out[cf] = (float)((apsigm->ONSbuf[cf] + apsigm->outbuf[cf] * apsigm->ifftwin[cf]) * apsigm->gain);
		}

		// buffer for next overlap and add frame
		for (cf = nSample; cf < 2*nSample; cf++) {
			apsigm->ONSbuf[cf-nSample] = apsigm->outbuf[cf] * apsigm->ifftwin[cf];
		}
	} else {
		for (cf = 0; cf < nSample; cf++) {
			out[cf] = (float)((apsigm->ONSbuf[cf] + apsigm->outbuf[cf]) * apsigm->gain);
		}

		// buffer for next overlap and add frame
		memcpy(apsigm->ONSbuf, apsigm->outbuf + nSample, nSample * sizeof(apsigm->outbuf[0]));
	}

	stack_closeFrame(apsigm->stack);

	return STATUS_OK;
}

int32_t apsigm_getChannelCount(const apsigm_t *apsigm) {
	return apsigm->channel;
}
