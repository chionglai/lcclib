

#ifndef __FIMATH_H
#define __FIMATH_H

#include "stdint.h"

#define FIMATH_INF				0x7FFFFFFF
#define FIMATH_NINF				0x80000000

#define FIMATH_MAX32			0x7FFFFFFF
#define FIMATH_MIN32			0x80000000

#define FIMATH_MAX16			0x00007FFF
#define FIMATH_MIN16			0xFFFF8000


#define FIMATH_LUT_MAX_INDEX_BIT    7
#define FIMATH_LUT_INDEX_MASK       0x7F
#define FIMATH_LUT_MAX_BIT          16


/// This is the normalised lookup table used for computing power of 2 in fixed point. The idea is to normalise the input 
/// into the form, u = i + f, where u is the input, i is the integer part, and 0 <= f < 1 is the fractional part.
/// Then, 2^u = 2^(i + f) = 2^i * 2^f, where 2^f is obtained from this lookup table. Once in this form, 2^u == (2^f << i).
/// This lookup table is meant for 1q7 fixed point format (i.e. 8-bit total word length and 7-bit fractional length), thus
/// the 7BIT in the name. For the remaining fractional length which is non-zero, interpolation on the lookup table will be 
/// performed. Note that the values of the table is 16-bit (can also be considered as 16-bit fractional length). 
/// So, if a 1q15 fixed point format is required, the value of the table need to be shifted right by one.
#define FIMATH_EXP2_LUT_7BIT		\
		 {    0,   356,   714,  1073,  1435,  1799,  2164,  2532,  2902,  3273,  3647,  4022,	4400,  4780,  5162,  5546,  	\
			 5932,  6320,  6710,  7102,  7496,  7893,  8292,  8693,  9096,  9501,  9908, 10318, 10730, 11144, 11560, 11979, 	\
		  12400, 12823, 13249, 13676, 14106, 14539, 14974, 15411, 15850, 16292, 16737, 17183, 17633, 18084, 18538, 18995,		\
			19454, 19915, 20379, 20846, 21315, 21786, 22260, 22737, 23216, 23698, 24183, 24670, 25160, 25652, 26148, 26645, 	\
			27146, 27649, 28155, 28664, 29175, 29690, 30207, 30727, 31249, 31775, 32303, 32834, 33369, 33906, 34446, 34988, 	\
			35534, 36083, 36635, 37190, 37747, 38308, 38872, 39439, 40009, 40582, 41158, 41738, 42320, 42906, 43495, 44087,		\
			44682, 45280, 45882, 46487, 47095, 47707, 48322, 48940, 49562, 50187, 50815, 51447, 52082, 52721, 53363, 54008, 	\
			54658, 55310, 55966, 56626, 57289, 57956, 58627, 59301, 59979, 60661, 61346, 62035, 62727, 63424, 64124, 64828, 	\
		  65535}	// last entry is required due to index + 1 in the interpolation
/// Note that the table above is offset by 2^16 in order to reduce the byte size of its entries. E.g. for index 0, the value
/// should be 65536, since 2^0 = 1, and this maps to 65536 = 2^16. So in order to compensate for this, this offset is required.
#define FIMATH_EXP2_LUT_OFFSET		(0x1 << 16)

/// This is the normalised lookup table used for computing logarithmic to the base 2 in fixed point. The idea is to
/// normalise the input into the form, u = x * 2^n, where u is the input, n is integer, and 1 <= x < 2.
/// Then, log2(u) = log2(x * 2^n) = n + log2(x), where log2(x) is obtained from this lookup table.
/// This lookup table is meant for 1q7 fixed point format (i.e. 8-bit total word length and 7-bit fractional length), thus
/// the 7BIT in the name. For the remaining fractional length which is non-zero, interpolation on the lookup table will be 
/// performed. Note that the values of the table is 16-bit (can also be considered as 16-bit fractional length). 
/// So, if a 1q15 fixed point format is required, the value of the table need to be shifted right by one.
#define FIMATH_LOG2_LUT_7BIT		\
{       0,    736,   1466,   2190,   2909,   3623,   4331,   5034,   5732,   6425,   7112,   7795,   8473,   9146,   9814,  10477,  	\
	11136,  11791,  12440,  13086,  13727,  14363,  14996,  15624,  16248,  16868,  17484,  18096,  18704,  19308,  19909,  20505,  	\
	21098,  21687,  22272,  22854,  23433,  24007,  24579,  25146,  25711,  26272,  26830,  27384,  27936,  28484,  29029,  29571,		\
    30109,  30645,  31178,  31707,  32234,  32758,  33279,  33797,  34312,  34825,  35334,  35841,  36346,  36847,  37346,  37842,  	\
	38336,  38827,  39316,  39802,  40286,  40767,  41246,  41722,  42196,  42667,  43137,  43603,  44068,  44530,  44990,  45448,  	\
	45904,  46357,  46809,  47258,  47705,  48150,  48593,  49034,  49472,  49909,  50344,  50776,  51207,  51636,  52063,  52488,		\
    52911,  53332,  53751,  54169,  54584,  54998,  55410,  55820,  56229,  56635,  57040,  57443,  57845,  58245,  58643,  59039,  	\
	59434,  59827,  60219,  60609,  60997,  61384,  61769,  62152,  62534,  62915,  63294,  63671,  64047,  64421,  64794,  65166,		\
    65535}	// last entry is required due to index + 1 in the interpolation

	
#define FIMATH_SIN_LUT_7BIT			\
{     0,    804,   1608,   2412,   3216,   4019,   4821,   5623,   6424,   7224,   8022,   8820,   9616,  10411,  11204,  11996,	\
  12785,  13573,  14359,  15143,  15924,  16703,  17479,  18253,  19024,  19792,  20557,  21320,  22078,  22834,  23586,  24335,  	\
  25080,  25821,  26558,  27291,  28020,  28745,  29466,  30182,  30893,  31600,  32303,  33000,  33692,  34380,  35062,  35738,	\
  36410,  37076,  37736,  38391,  39040,  39683,  40320,  40951,  41576,  42194,  42806,  43412,  44011,  44604,  45190,  45769,  	\
  46341,  46906,  47464,  48015,  48559,  49095,  49624,  50146,  50660,  51166,  51665,  52156,  52639,  53114,  53581,  54040,  	\
  54491,  54934,  55368,  55794,  56212,  56621,  57022,  57414,  57798,  58172,  58538,  58896,  59244,  59583,  59914,  60235,	\
  60547,  60851,  61145,  61429,  61705,  61971,  62228,  62476,  62714,  62943,  63162,  63372,  63572,  63763,  63944,  64115,  	\
  64277,  64429,  64571,  64704,  64827,  64940,  65043,  65137,  65220,  65294,  65358,  65413,  65457,  65492,  65516,  65531, 	\
  65535}

/**
 * This is the normalised lookup table used for computing sigmoid function y = 1/(1 + exp(-a*(x - c))) in fixed point. 
 * The idea is to normalise the curve to fit into a 128-entry table with 16-bit value. In order to capture the curve in limited
 * entries, this lookup table is for sigmoid function with a = 5 and c = 0. To use this table for any value of a and c, the 
 * input xi must first be transformed to x = (xi - c)/a. This lookup table is meant for 1q7 fixed point format, thus
 * the 7BIT in the name. For the remaining fractional length which is non-zero, interpolation on the lookup table will be 
 * performed. Note that the values of the table is unsigned 16-bit (can also be considered as 16-bit fractional length). 
 * So, if a 1q15 fixed point format is required, the value of the table need to be shifted right by one.
 */
#define FIMATH_SIGMOID_LUT_7BIT		\
{     439,    474,    512,    553,    598,    646,    698,    754,    815,    880,    951,   1027,   1109,   1197,   1292,   1395,   \
     1506,   1625,   1754,   1892,   2041,   2201,   2374,   2559,   2758,   2972,   3202,   3448,   3713,   3996,   4299,   4624,   \
     4971,   5342,   5739,   6161,   6611,   7090,   7600,   8140,   8714,   9321,   9964,  10642,  11357,  12109,  12899,  13728,   \
    14595,  15500,  16444,  17425,  18442,  19495,  20582,  21700,  22849,  24025,  25226,  26448,  27689,  28945,  30213,  31489,   \
    32768,  34047,  35323,  36591,  37847,  39088,  40310,  41511,  42687,  43836,  44954,  46041,  47094,  48111,  49092,  50036,   \
    50941,  51808,  52637,  53427,  54179,  54894,  55572,  56215,  56822,  57396,  57936,  58446,  58925,  59375,  59797,  60194,   \
    60565,  60912,  61237,  61540,  61823,  62088,  62334,  62564,  62778,  62977,  63162,  63335,  63495,  63644,  63782,  63911,   \
    64030,  64141,  64244,  64339,  64427,  64509,  64585,  64656,  64721,  64782,  64838,  64890,  64938,  64983,  65024,  65062,   \
    65535}

#define FIMATH_SIGMOID_GRADIENT         5
#define FIMATH_SIGMOID_ORIGIN_SHIFT     6
#define FIMATH_SIGMOID_ORIGIN_INDEX     (1ul << FIMATH_SIGMOID_ORIGIN_SHIFT)

	
#define fimath_clip(value, min, max)    ((value) > (max)? (max) : ((value) < (min)? (min) : (value)))

	
/// This constant is used to calculate e^x using fimath_exp2(), i.e.
/// e^x == 2^(x*log2(e)), where log2(e) can be pre-calculated as below.
/// The value of 3098164009 is for unsigned i1q31 (maximum precision).
/// To convert to signed i2q30, right shift by 1
#define FIMATH_LOG2E                    (3098164009UL >> 1)
#define FIMATH_LOG2E_FL                 30
#define fimath_exp(in, fl)              _fimath_expN(FIMATH_LOG2E, FIMATH_LOG2E_FL, in, fl)


/// This constant is used to calculate 10^x using fimath_exp2(), i.e.
/// 10^x == 2^(x*log2(10)), where log2(10) can be pre-calculated as below.
/// The value of 3566893132 is for unsigned i2q30 (maximum precision).
/// To convert to signed i3q29, right shift by 1
#define FIMATH_LOG2_10                  (3566893132UL >> 1)
#define FIMATH_LOG2_10_FL               29
#define fimath_exp10(in, fl)            _fimath_expN(FIMATH_LOG2_10, FIMATH_LOG2_10_FL, in, fl)
		

/// This constant is used to calculate natural log(x) using fimath_log2(), i.e.
/// log(x) == log2(x) * (1/log2(e)), where 1/log2(e) can be pre-calculated as below.
/// The value of 2977044472 is for unsigned i0q32 (maximum precision).
/// To convert to signed i1q31, right shift by 1
#define FIMATH_LOG2_ER                  (2977044472UL >> 1)
#define FIMATH_LOG2_ER_FL               31
#define fimath_log(in, fl)              _fimath_logN(FIMATH_LOG2_ER, FIMATH_LOG2_ER_FL, in, fl)
	

/// This constant is used to calculate log10(x) using fimath_log2(), i.e.
/// log10(x) == log2(x) * (1/log2(10)), where 1/log2(10) can be pre-calculated as below.
/// The value of 1292913986 is for unsigned i0q32 (maximum precision).
/// To convert to signed i1q31, right shift by 1
#define FIMATH_LOG2_10R                 (1292913986UL >> 1)
#define FIMATH_LOG2_10R_FL              31
#define fimath_log10(in, fl)            _fimath_logN(FIMATH_LOG2_10R, FIMATH_LOG2_10R_FL, in, fl)

#define fimath_log2ToLog10(in, fl)      ((((int64_t) in) * ((int64_t) FIMATH_LOG2_10R)) >> FIMATH_LOG2_10R_FL)
#define fimath_log10ToLog2(in, fl)      ((((int64_t) in) * ((int64_t) FIMATH_LOG2_10)) >> FIMATH_LOG2_10_FL)


/// min and max without branching
#define fimath_min(x, y)            (y ^ ((x ^ y) & -(x < y))) // min(x, y)
#define fimath_max(x, y)            (x ^ ((x ^ y) & -(x < y))) // max(x, y)


#ifdef __cplusplus__
extern "C" {
#endif

/**
 * @brief Function to calculate sigmoid function.
 * @param[in] in Sigmoid function argument.
 * @param[in] gradient Gradient of sigmoid function.
 * @param[in] mid The mid point for the sigmoid function.
 * @param[in] numFracBit Number of fractional bit for all the inputs as well as the output.
 */
int32_t fimath_sigmoid(int32_t in, int32_t gradient, int32_t mid, uint8_t numFracBit);

/**
 * @brief Function to remove all the leading zeros from a 32-bit integer.
 * @details This function perform a 2-stage removal. First, it checks byte by byte.
 *      Then, it checks bit by bit within the byte that contains at least one 1's.
 *      But still, O(n) = n???
 * @param[inout] in Pointer to 32-bit whose leading zeros are to be removed. Once the
 *      leading zeros are removed, the resulted value is reflected in this variable.
 * @return The number of leading zeros removed.
 */
uint8_t fimath_removeLZ(uint32_t* in);


/// Function to calculate the logarithmic to the base 2 using fixed point and lookup table.
/// Specifically, this function calculate y = log2(u). The following steps are used:
/// 1. Normalise u into u = x * 2^n, where 1 <= x < 2
/// 2. Then log2(u) == log2(x * 2^n) == n + log2(x).
/// 3. Removing all leading zeros from u. After all leading zeros are removed from u, the 
/// 	 resulted value will be x.
/// 4. Calculate n = 32 - numFracBit - numberOfLeadingZerosRemoved - 1;
/// 5. Calculate log2(x) from lookup table.
/// 6. Interpolate the remaining fractional bits using the lookup table.
/// 7. Adjust the fixed point format for n, fractional and interpolated value.
/// 8. The result is given by n + fractionalFromLUT + interpolateValueFromLUT.
/// \param[in] in The input value x in 32-bit fixed point. Must be strictly positive. If in <= 0,
/// 		this function will return FIMATH_NINF.
/// \param[in] numFracBit The number of fractional bits for in. E.g. if in is a i8q24 (i.e. 8 integer bit + 
/// 		24 fractional bit), numFracBit = 24. numFracBit must be specified correctly.
/// \return The result of log2(in) in 32-bit fixed point. The format is the same as in, e.g. if in is i8q24, then,
/// 		the returned value is also i8q24. Returns FIMATH_NINF if in <= 0.
int32_t fimath_log2(int32_t in, uint8_t numFracBit);

/// Function to calculate logarithmic to any base using fixed point fimath_log2(). Logarithmic
/// to any base can be calculated using log2() using the formula below:
/// 	logN(x) == log2(x) * (1/log2(N))
/// Note that this function should not be used directly unless you know what you are doing.
/// Refer to fimath_log() and fimath_log10() macros for more details on how the argument 
/// scale and scaleFL should be pre-calculated.
/// \param[in] scale The value of 1/log2(N) in fixed point for conversion from log2() to logN(). 
/// \param[in] scaleFL The fractional length/bits for scale.
/// \param[in] in Value of x in fixed point.
/// \param[in] numFracBit The fractional length/bits of in.
/// \return The result of logN(x) in fixed point, having same fractional length as in.
int32_t _fimath_logN(int32_t scale, uint8_t scaleFL, int32_t in, uint8_t numFracBit);

/// Function to calculate the power of 2 for fixed point. Specifically, this function 
/// calculates y = 2^x using fixed point and lookup table. The following steps are used:
/// 1. Normalise x into x = i + f, where i is the integer part and f is the fractional part.
/// 2. Then, 2^x == 2^i * 2^f. In fixed point term, 2^x == (2^f << i).
/// 3. Calculate 2^f from 7-bit lookup table, i.e. 7 MSB bit of frac is used to index the lookup table.
/// 4. Interpolate the remaining fractional bits using the lookup table.
/// 5. Adjust the fixed point format for both fractional and interpolated value, i.e. frac = 2^f + interpolated value.
/// 6. Calculate the integer part i (ref. Step 2).
/// 7. Shift the frac by i amount (shift left for positive, right for negative).
/// \param[in] in The input value x in 32-bit fixed point. If in is too large such that 2^in cannot be represented 
/// 		by 32-bit with specified fixed point format, FIMATH_INF will be returned.
/// \param[in] numFracBit The number of fractional bits for in. E.g. if in is a i8q24 (i.e. 8 integer bit + 
/// 		24 fractional bit), numFracBit = 24. numFracBit must be specified correctly.
/// \return The result of 2^in in 32-bit fixed point. The format is the same as in, e.g. if in is i8q24, then,
/// 		the returned value is also i8q24.
int32_t fimath_exp2(int32_t in, uint8_t numFracBit);

/// Function to calculate any exponential using fixed point fimath_exp2(). Any exponential 
/// can be calculated using exp2() using the formula below:
/// 	a^x = 2^(x * log2(a))
/// Note that this function should not be used directly unless you know what you are doing.
/// Refer to fimath_exp() and fimath_exp10() macros for more details on how the argument 
/// scale and scaleFL should be pre-calculated.
/// \param[in] scale The value of log2(a) in fixed point for conversion from exp2() to expN(). 
/// \param[in] scaleFL The fractional length/bits for scale.
/// \param[in] in Value of x in fixed point.
/// \param[in] numFracBit The fractional length/bits of in.
/// \return The result of a^x in fixed point, having same fractional length as in.
int32_t _fimath_expN(int32_t scale, uint8_t scaleFL, int32_t in, uint8_t numFracBit);

/// Function to calculate sine for first quadrant only using fixed point, lookup table and linear interpolation.
/// This function is where the main calculation on sine value is performed. The function
/// fimath_sin() basically calls this function and then use the relationship between 
/// Quadrant 1, 2, 3 and 4 to calculate the sine value for the whole circle. 
/// \param[in] in Input value in the format of i0q32, in the range of [0, 1), which is mapped to [0, pi/2).
/// \return The sine value for Quadrant 1 only, in i1q31 format.
int32_t fimath_sinQ1(uint32_t in);

/// Function to calculate sine using fixed point and lookup table. This function
/// basically calls fimath_sinQ1() and then use the relationship between 
/// Quadrant 1, 2, 3 and 4 to calculate the sine value for the whole circle.
/// \param[in] in Input value in the format of i1q31, in the range of [-1, 1), which is mapped to [-pi, pi).
/// \return The sine value, in i1q31 format. 
int32_t fimath_sin(int32_t in);

/// Function to calculate cosine for first quadrant only using fixed point, lookup table 
/// and linear interpolation. This function is where the main calculation on cosine value 
/// is performed. Although to be more exact, it uses the relationship of cos(x) = sin(pi/2 - x) 
/// and call fimath_sinQ1() to actually compute the cosine value. The function
/// fimath_cos() basically calls this function and then use the relationship between 
/// Quadrant 1, 2, 3 and 4 to calculate the cossine value for the whole circle. 
/// \param[in] in Input value in the format of i0q32, in the range of [0, 1), which is mapped to [0, pi/2).
/// \return The cosine value for Quadrant 1 only, in i1q31 format.
int32_t fimath_cosQ1(uint32_t in);

/// Function to calculate cosine using fixed point and lookup table. This function
/// basically calls fimath_cosQ1() and then use the relationship between 
/// Quadrant 1, 2, 3 and 4 to calculate the cosine value for the whole circle.
/// \param[in] in Input value in the format of i1q31, in the range of [-1, 1), which is mapped to [-2*pi, 2*pi).
/// \return The cosine value, in i1q31 format. 
int32_t fimath_cos(int32_t in);

// TODO: comment, no overflow guard on addition
/**
 * \brief Function to calculate exponential average in fixed point format.
 * \details Although this function is used to calculate the exponential average, it can be use to 
 * 			calculate any equation that has the form of z = a*x + b*y. Note that this function does not 
 *			perform signed saturation and does not guard against overflow from the addition.
 * \param[in] prevEst Previous value of average estimate, or the variable a in the above equation.
 * \param[in] beta Averaging constant, or the variable x
 * \param[in] currEst Current value to be averaged, or the variable b
 * \param[in] mBeta One minus the averaging constant, or the variable y
 * \param[in] numFracBit Number of fractional bit for all the parameters prevEst, beta, currEst and mBeta.
 * \return The new average value, or the variable z.
 */
int32_t fimath_expAvg(int32_t prevEst, int32_t beta, int32_t currEst, int32_t mBeta, uint8_t numFracBit);

/**
 * \brief Calculate the absolute value of a signed integer.
 * \param[in] x A signed integer whose absolute value is to be calculated.
 * \return The absolute value of x. If x is 0x80000000, 0x7FFFFFFF is returned.
 */
int32_t fimath_abs(int32_t x);


#ifdef __cplusplus__
}
#endif

#endif
