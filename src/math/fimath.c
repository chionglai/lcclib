

#include "math/fimath.h"

static uint16_t FIMATH_EXP2_LUT[] = FIMATH_EXP2_LUT_7BIT;
static uint16_t FIMATH_LOG2_LUT[] = FIMATH_LOG2_LUT_7BIT;
static uint16_t FIMATH_SIN_LUT[]  = FIMATH_SIN_LUT_7BIT;
static uint16_t FIMATH_SIGMOID_LUT[]  = FIMATH_SIGMOID_LUT_7BIT;	
	
int32_t fimath_sigmoid(int32_t in, int32_t gradient, int32_t mid, uint8_t numFracBit) {
	uint32_t bits;
	int32_t x, index;
	int32_t yi, rem;
	int64_t temp;
	
	/// convert the input in into 8q24 positive fixed point value for table index
	// This is to scale the input to the correct gradient and mid point to be mapped to LUT
	temp = ((int64_t)(in - mid) * gradient / FIMATH_SIGMOID_GRADIENT) >> numFracBit;
	x = (int32_t) fimath_clip(temp, FIMATH_MIN32, FIMATH_MAX32);
	//x = __SSAT(((int64_t)(in - mid) * gradient / FIMATH_SIGMOID_GRADIENT) >> numFracBit, 32);
	
	// This is because the index 0 for LUT is not mapped to 0 value, but -1 of 1q6.
	x += (1ul << numFracBit);
	
	// index need to be 7-bit integer + 2^6 since the LUT is a 128-element LUT with 
	// index 0 as -1 for 1q6
	index = (x >> (numFracBit - FIMATH_SIGMOID_ORIGIN_SHIFT));
	
	if (index > FIMATH_LUT_INDEX_MASK) {	// over on the positive end
		return (int32_t) (1ul << numFracBit);
	} else if (index < 0) {		// over on the negative end
		return 0;
	} else {
		
		bits = numFracBit - FIMATH_LUT_MAX_BIT;
		if (bits > 0) {
			yi = (FIMATH_SIGMOID_LUT[index] << bits);
		} else {
			yi = (FIMATH_SIGMOID_LUT[index] >> (-bits));
		}
		
		// since x is in 8q24 format, and the index takes the upper 6 fixed point bit of 8q(6 ... 18)
		// thus, the remainder takes the lower 18 fixed point bit, hence the mask 0x3FFFF
		// x     --> 8q(6 ... 18) 
		// index --> 1q(6
		// rem   -->      ... 18)
		// rem is still in 8q24 format, but only have the value from the lower 18 fixed point bits for intepolation
		rem = (x & 0x3FFFF) >> 3;

		// adjusting rem to match the output fixed point format, in order to lose minimal precision
		// rem is i1q15
		// LUT is i0q16
		// rem*LUT is i1q31
		rem *= (FIMATH_SIGMOID_LUT[index+1] - FIMATH_SIGMOID_LUT[index]);
		rem = rem >> (31 - numFracBit);
		
		return (rem + yi);
	}
}


uint8_t fimath_removeLZ(uint32_t* in) {
	uint8_t i, j;
	
	i = 0;
	while ((i < sizeof(uint32_t)) && ((*in & 0xFF000000) == 0)) {
		*in = *in << 8;
		i++;
	}
	
	i = (uint8_t) (i << 3);
	j = 0;
	if (i < 32) {
		while ((j < 8) && ((*in & 0x80000000) == 0)) {
			*in = *in << 1;
			j++;
		}
	}	
	return (i + j);
}
	

int32_t fimath_log2(int32_t in, uint8_t numFracBit) {
	uint32_t x;
	int32_t n;
	uint32_t frac, rem;
	uint8_t index;
	int8_t shift;
	
	if (in <= 0) {
		return (int32_t) FIMATH_NINF;
	}
	
	x = (uint32_t) in;
	n = fimath_removeLZ(&x);
	n = 31 - numFracBit - n;	// 31 is for 32-bit data
	
	//adjusting n to match output fixed point
	n = n << numFracBit;
	
	// 31 here is the number of bit for in after its leading zeros are removed
	index = (x >> (31 - FIMATH_LUT_MAX_INDEX_BIT)) & FIMATH_LUT_INDEX_MASK;
	
	// get the fractional part from LUT
	// frac is i16q16
	frac = FIMATH_LOG2_LUT[index];
	
	// adjusting frac to match the output fixed point format, in order to lose minimal precision
	if (numFracBit > FIMATH_LUT_MAX_BIT) {
		frac = (frac << (numFracBit - FIMATH_LUT_MAX_BIT));
	} else {
		frac = (frac >> (FIMATH_LUT_MAX_BIT - numFracBit));
	}

	// use the remainder fractional bits for interpolation. Only the remaining 16-bit is used and it is 
	// always positive. Remember that the remaining fractional bits no longer contains a sign bit, so 
	// remove it (if any) by ANDing with 0x7FFF. Here, if-else branching is used. For non-branching,
	// refer to fimath_exp2().
	shift = FIMATH_LUT_MAX_BIT - FIMATH_LUT_MAX_INDEX_BIT;
	if (shift >= 0) {
		rem = (x >> shift) & 0x7FFF;
	} else {
		rem = (x << (-shift)) & 0x7FFF;
	}
	
	// adjusting rem to match the output fixed point format, in order to lose minimal precision
	// rem is i1q15
	// LUT is i0q16
	// rem*LUT is i1q31
	rem *= (FIMATH_LOG2_LUT[index+1] - FIMATH_LOG2_LUT[index]);
	rem = rem >> (31 - numFracBit);
	
	return (n + (int32_t) frac + (int32_t) rem);
}


int32_t _fimath_logN(int32_t scale, uint8_t scaleFL, int32_t in, uint8_t numFracBit) {
	int32_t res2;
	
	res2 = fimath_log2(in, numFracBit);
	
	return (int32_t) ( (((int64_t) res2) * ((int64_t) scale)) >> scaleFL );
}


int32_t fimath_exp2(int32_t in, uint8_t numFracBit) {
	uint32_t result;
	uint32_t frac;
	uint8_t index;
	uint32_t rem;
	int16_t intShift, shift;
	
	
	intShift = (int16_t) (in >> numFracBit);
	if (intShift >= (31 - numFracBit)) {
		return FIMATH_INF;
	}
	
	// normalise input into 2^in = 2^i * 2^f. Let, frac = 2^f, obtained from LUT with interpolation
	shift = numFracBit - FIMATH_LUT_MAX_INDEX_BIT;
	frac = FIMATH_EXP2_LUT_OFFSET;
	if (shift >= 0) {		
		// there is more than 8 fractional bits, so split into 8 MSB fractional bits 
		// for lookup table index, and the remainder is used to interpolate from lookup table
		index = (in >> shift) & FIMATH_LUT_INDEX_MASK;
		frac += FIMATH_EXP2_LUT[index];
		
		// use the remainder fractional bits for interpolation. Only the remaining 16-bit is used and it is 
		// always positive. Convert the remaining fractional bits to i1q31, then back to i1q15. This is to 
		// avoid branching in case shift > 16. Remember that the remaining fractional bits no longer 
		// contains a sign bit, so remove it (if any) by ANDing with 0x7FFF.
		rem = (uint16_t) (((in << (31 - shift)) >> 16) & 0x7FFF);
		
		// adjusting rem to match the output fixed point format, in order to lose minimal precision
		// frac is i0q16
		// rem is i1q15
		// LUT is i0q16
		// rem*LUT == i1q31
		rem *= (FIMATH_EXP2_LUT[index+1] - FIMATH_EXP2_LUT[index]);
		rem = rem >> (31 - numFracBit);
	} else {		
		// there is less than 8 fractional bit, so use whatever we have
		index = (uint8_t) ((in << (-shift)) & FIMATH_LUT_INDEX_MASK);
		frac = FIMATH_EXP2_LUT[index];
		rem = 0;
	}
	
	// adjusting frac to match the output fixed point format, in order to lose minimal precision
	if (numFracBit > FIMATH_LUT_MAX_BIT) {
		frac = (frac << (numFracBit - FIMATH_LUT_MAX_BIT));
	} else {
		frac = (frac >> (FIMATH_LUT_MAX_BIT - numFracBit));
	}
	
	// add in interpolated data and LUT offset
	frac += rem;
	
	// perform the final shift
	if (intShift >= 0) {
		result = frac << intShift;
	} else {
		result = frac >> (-intShift);
	}
	return (int32_t) result;
}


int32_t _fimath_expN(int32_t scale, uint8_t scaleFL, int32_t in, uint8_t numFracBit) {
	int32_t x;
	
	x = (int32_t) ( (((int64_t) in) * ((int64_t) scale)) >> scaleFL );
	
	return fimath_exp2(x, numFracBit);
}


int32_t fimath_sinQ1(uint32_t in) {
	uint8_t index;
	uint32_t rem;
	int32_t result;
	
	// extract MSB 7-bit to serve as lookup index
	index = (in >> 25) & FIMATH_LUT_INDEX_MASK;
	
	// LUT is i0q16, convert to i1q31
	result = (FIMATH_SIN_LUT[index] << 15);
	
	// perform linear interpolation using remaining bits
	// rem is i7q25, convert to i16q16
	// LUT is i0q16
	// rem*LUT is i0q32
	rem = ((in >> 9) & 0xFFFF);
	rem *= (FIMATH_SIN_LUT[index+1] - FIMATH_SIN_LUT[index]);
	
	result += (rem >> 1);
	
	return result;
}


int32_t fimath_sin(int32_t in) {
	uint32_t uin;
	int32_t result;
	uint8_t quadrant;
	
	// Convert input from i1q31 to ui0q32
	uin = (uint32_t)(in << 1);
	
	// check for quadrant. Quadrant is given by MSB 2-bit
	quadrant = (uint8_t) (uin >> 30);
	switch (quadrant) {
		case 0:		// Quadrant 1
			result = fimath_sinQ1(uin << 2);
			break;
		case 1:		// Quadrant 2
			result = fimath_sinQ1((0x7FFFFFFF - uin) << 2);		// (0.5 - uin)
			break;
		case 2:		// Quadrant 3
			result = -fimath_sinQ1((uin - 0x7FFFFFFF) << 2);		// (uin - 0.5)
			break;
		case 3:		// Quadrant 4
			result = -fimath_sinQ1((0xFFFFFFFF - uin) << 2);
			break;
	}
	
	if (in < 0) {
		result = -result;
	}
	
	return result;
}


int32_t fimath_cosQ1(uint32_t in) {
	return fimath_sinQ1(0xFFFFFFFF - in);
}


int32_t fimath_cos(int32_t in) {
	uint32_t uin;
	int32_t result;
	uint8_t quadrant;
	
	// Convert input from i1q31 to ui0q32
	uin = (uint32_t)(in << 1);
	
	// check for quadrant. Quadrant is given by MSB 2-bit
	quadrant = (uint8_t) (uin >> 30);
	switch (quadrant) {
		case 0:		// Quadrant 1
			result = fimath_cosQ1(uin << 2);
			break;
		case 1:		// Quadrant 2
			result = -fimath_cosQ1((0x7FFFFFFF - uin) << 2);		// (0.5 - uin)
			break;
		case 2:		// Quadrant 3
			result = -fimath_cosQ1((uin - 0x7FFFFFFF) << 2);		// (uin - 0.5)
			break;
		case 3:		// Quadrant 4
			result = fimath_cosQ1((0xFFFFFFFF - uin) << 2);
			break;
	}
	
	// Note that cos(x) == cos(-x), so there is no need to concern if x is negative or not
	return result;
}

// NOT verified
int32_t fimath_expAvg(int32_t prevEst, int32_t beta, int32_t currEst, int32_t mBeta, uint8_t numFracBit) {
	int64_t result64;

	// both operand 0 and 1 have 32-bit word length and numFracBit fractional bits,
	// and the result will have 64-bit word length and 2*numFracBit fractional bits
	result64 = ((int64_t) prevEst) * ((int64_t) beta);
	result64 = (result64 >> 1) + ((((int64_t) currEst) * ((int64_t) mBeta)) >> 1);    // right shift 1 to guard against addition overflow

	//prevEst = fimath_clip((int32_t) (result64 >> numFracBit), (int32_t) FIMATH_MIN32, FIMATH_MAX32);
	
	prevEst = (int32_t) (result64 >> numFracBit);
	prevEst = fimath_shiftAndSat(prevEst, 1);
	return prevEst;
}

// NOT verified
int32_t fimath_expAvg32(int32_t prevEst, int32_t beta, int32_t currEst, int32_t mBeta, uint8_t numFracBit) {
    int32_t result32;
    uint8_t nShift = numFracBit / 2;

    // both operand 0 and 1 have 32-bit word length and numFracBit fractional bits,
    // and the result will have 64-bit word length and 2*numFracBit fractional bits
    result32 = (prevEst >> nShift) * (beta >> nShift);
    result32 = (result32 >> 1) + (((currEst >> nShift) * (mBeta >> nShift)) >> 1);    // right shift 1 to guard against addition overflow

    //prevEst = fimath_clip((int32_t) (result64 >> numFracBit), (int32_t) FIMATH_MIN32, FIMATH_MAX32);
    prevEst = fimath_shiftAndSat(result32, nShift);
    return prevEst;
}

int32_t fimath_abs(int32_t x) {
	int32_t const mask = x >> (sizeof(int32_t) * 8 - 1);
	
	// (x + mask) ^ mask is to get the absolute value for all values except the
	// most negative (e.g. 0x80000000).
	// (x == FIMATH_MIN32) is to takes care of 0x80000000 (the most negative value),
	// since we want it to be 0x7FFFFFFF
	return ((x + mask) ^ mask) - (((int32_t) FIMATH_MIN32) == x);
}


int32_t fimath_shiftAndSat(int32_t x, int32_t shift) {
    uint32_t mask;
    int32_t temp;

    if (shift == 0) {
        return x;
    } else {
        mask = -(FIMATH_MIN32 >> shift);
        temp = x & mask;
        if (temp == mask) {
            return FIMATH_MIN32;
        } else if (temp == (mask ^ FIMATH_MIN32)) {
            return FIMATH_MAX32;
        } else {
            return (x << shift);
        }
    }
}
