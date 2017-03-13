/*
 * test_util.c
 *
 *  Created on: 8 Jul 2016
 *      Author: chiong
 */

#include <stdint.h>
#include "util/util.h"
#include "debug/assert.h"
#include "test_util.h"


/**
 * @details test swap() for various native types.
 */
#define INT_NUM_0	(123456)
#define INT_NUM_1	(-34455)
#define FLOAT_NUM_0	(12.098f)
#define FLOAT_NUM_1	(-344.76f)
#define UCHAR_NUM_0	(12)
#define UCHAR_NUM_1	(249)

void test_utilSwap(void) {
	int ia, ib, it;
	float fa, fb, ft;
	unsigned char ca, cb, ct;

	/* Test for int */
	ia = INT_NUM_0;
	ib = INT_NUM_1;
	ASSERT(INT_NUM_1 != ia, "Incorrect initial value of ia.");
	ASSERT(INT_NUM_0 != ib, "Incorrect initial value of ib.");
	UTIL_swap(ib, ia, it);
	ASSERT(INT_NUM_1 == ia, "Incorrect swap on ia.");
	ASSERT(INT_NUM_0 == ib, "Incorrect swap on ib.");

	/* Test for float */
	fa = FLOAT_NUM_0;
	fb = FLOAT_NUM_1;
	ASSERT(FLOAT_NUM_1 != fa, "Incorrect initial value of fa.");
	ASSERT(FLOAT_NUM_0 != fb, "Incorrect initial value of fb.");
	UTIL_swap(fb, fa, ft);
	ASSERT(FLOAT_NUM_1 == fa, "Incorrect swap on fa.");
	ASSERT(FLOAT_NUM_0 == fb, "Incorrect swap on fb.");

	/* Test for char */
	ca = UCHAR_NUM_0;
	cb = UCHAR_NUM_1;
	ASSERT(UCHAR_NUM_1 != ca, "Incorrect initial value of ca.");
	ASSERT(UCHAR_NUM_0 != cb, "Incorrect initial value of cb.");
	UTIL_swap(cb, ca, ct);
	ASSERT(UCHAR_NUM_1 == ca, "Incorrect swap on ca.");
	ASSERT(UCHAR_NUM_0 == cb, "Incorrect swap on cb.");
}

#define FLOAT_LOWER		(-12.3f)
#define FLOAT_UPPER		(24.6f)
#define INT_LOWER		(-12345)
#define INT_UPPER		(124)
void test_utilClip(void) {
	int ia;
	float fa;

	ia = INT_LOWER - 10;
	ASSERT(ia < INT_LOWER, "ia not initialised correctly.");
	UTIL_clip(ia, INT_LOWER, INT_UPPER);
	ASSERT(INT_LOWER <= ia && ia <= INT_UPPER, "ia not clipped correctly.");

	ia = INT_UPPER + 10;
	ASSERT(ia > INT_UPPER, "ia not initialised correctly.");
	UTIL_clip(ia, INT_LOWER, INT_UPPER);
	ASSERT(INT_LOWER <= ia && ia <= INT_UPPER, "ia not clipped correctly.");

	ia = (INT_UPPER + INT_LOWER) / 2;
	ASSERT(INT_LOWER <= ia && ia <= INT_UPPER, "ia not initialised correctly.");
	UTIL_clip(ia, INT_LOWER, INT_UPPER);
	ASSERT(INT_LOWER <= ia && ia <= INT_UPPER && ia == (INT_UPPER + INT_LOWER) / 2,
		   "Unexpected change in ia.");

	fa = FLOAT_LOWER - 10;
	ASSERT(fa < FLOAT_LOWER, "fa not initialised correctly.");
	UTIL_clip(fa, FLOAT_LOWER, FLOAT_UPPER);
	ASSERT(FLOAT_LOWER <= fa && fa <= FLOAT_UPPER, "fa not clipped correctly.");

	fa = FLOAT_UPPER + 10;
	ASSERT(fa > FLOAT_UPPER, "fa not initialised correctly.");
	UTIL_clip(fa, FLOAT_LOWER, FLOAT_UPPER);
	ASSERT(FLOAT_LOWER <= fa && fa <= FLOAT_UPPER, "fa not clipped correctly.");

	fa = (FLOAT_UPPER + FLOAT_LOWER) / 2.0f;
	ASSERT(FLOAT_LOWER <= fa && fa <= FLOAT_UPPER, "fa not initialised correctly.");
	UTIL_clip(fa, FLOAT_LOWER, FLOAT_UPPER);
	ASSERT(FLOAT_LOWER <= fa && fa <= FLOAT_UPPER && fa == (FLOAT_UPPER + FLOAT_LOWER) / 2.0f,
			   "Unexpected change in fa.");
}

/**
 * @details Test includes:
 * 		1. read8()
 * 		2. write8()
 */
#define RW8_TEST_VALUE	((uint8_t) 0xBE)
void test_utilRw8(void) {
	uint8_t a;
	uint8_t arr[10] = {0};

	arr[3] = RW8_TEST_VALUE;
	a = UTIL_read8(&arr[3]);
	ASSERT(a == arr[3], "read8() failed.");

	UTIL_write8(&arr[9], RW8_TEST_VALUE);
	ASSERT(RW8_TEST_VALUE == arr[9], "write8 failed.")
}

/**
 * @details Test includes:
 * 		1. read16()
 * 		2. write16()
 */
#define RW16_TEST_VALUE	((uint16_t) 0xBEEF)
void test_utilRw16(void) {
	uint16_t a;
	uint16_t arr[10] = {0};

	arr[3] = RW16_TEST_VALUE;
	a = UTIL_read16(&arr[3]);
	ASSERT(a == arr[3], "read16() failed.");

	UTIL_write16(&arr[9], RW16_TEST_VALUE);
	ASSERT(RW16_TEST_VALUE == arr[9], "write16() failed.");
}

/**
 * @details Test includes:
 * 		1. read32()
 * 		2. write32()
 */
#define RW32_TEST_VALUE	((uint32_t) 0xFEEDB00B)
void test_utilRw32(void) {
	uint32_t a;
	uint32_t arr[10] = {0};

	arr[3] = RW32_TEST_VALUE;
	a = UTIL_read32(&arr[3]);
	ASSERT(a == arr[3], "read32() failed.");

	UTIL_write32(&arr[9], RW32_TEST_VALUE);
	ASSERT(RW32_TEST_VALUE == arr[9], "write32() failed.");
}


void test_utilAll(void) {
	test_utilSwap();
	test_utilClip();
	test_utilRw8();
	test_utilRw16();
	test_utilRw32();
}
