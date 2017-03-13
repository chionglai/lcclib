/*
 * signal.h
 *
 *  Created on: 19 Sep 2016
 *      Author: chiong
 *
 */

#ifndef INC_SIGNAL_H_
#define INC_SIGNAL_H_

#include <complex.h>

/* Definition for real signal type, both single and double precision. */
typedef float realf_t;
typedef double reald_t;

/* Definition of complex, single-precision signal type. */
typedef struct {
	float r;
	float i;
} complexf_t;

/* Definition of complex, double-precision signal type. */
typedef struct {
	double r;
	double i;
} complexd_t;

#endif /* INC_SIGNAL_H_ */
