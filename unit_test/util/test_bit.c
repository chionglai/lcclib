/*
 * test_bit.c
 *
 *  Created on: 8 Jul 2016
 *      Author: chiong
 */

#include <stdint.h>
#include "util/bit.h"
#include "debug/assert.h"
#include "test_bit.h"

void test_bitSetSingle(void) {
	uint32_t val = 0;
	uint32_t bitPos[] = {0, 31, 5};
	uint32_t count = sizeof(bitPos) / sizeof(bitPos[0]);

	while (count > 0) {
		count--;
		ASSERT(BIT_isBitClr(val, bitPos[count]), "Bit not cleared initially.");
		val = BIT_bitSet(val, bitPos[count]);
		ASSERT(BIT_isBitSet(val, bitPos[count]), "Bit not set correctly.");
	}
}

void test_bitClrSingle(void) {
	uint32_t val = -1;
	uint32_t bitPos[] = {0, 31, 5};
	uint32_t count = sizeof(bitPos) / sizeof(bitPos[0]);

	while (count > 0) {
		count--;
		ASSERT(BIT_isBitSet(val, bitPos[count]), "Bit not set initially.");
		val = BIT_bitClr(val, bitPos[count]);
		ASSERT(BIT_isBitClr(val, bitPos[count]), "Bit not cleared correctly.");
	}
}

void test_bitSetMulti(void) {
	uint32_t val = 0;
	uint32_t mask = 0;
	uint32_t bitPos[] = {0, 31, 5};
	uint32_t count = sizeof(bitPos) / sizeof(bitPos[0]);

	ASSERT(mask == 0, "mask not initialised to 0.");
	while (count > 0) {
		count--;
		mask = BIT_bitSet(mask, bitPos[count]);
	}
	ASSERT(val == 0, "value not initialised to 0.")
	val = BIT_bitSetMask(val, mask);
	ASSERT(BIT_isAllBitSet(val, mask), "Bits not set correctly.")

	val = BIT_bitClr(val, bitPos[0]);
	ASSERT(!BIT_isAllBitSet(val, mask), "One bit cleared, but isAllBitSet still true.")
	ASSERT(BIT_isAnyBitClr(val, mask), "isAnyBitClr failed.")
}

void test_bitClrMulti(void) {
	uint32_t val = -1;
	uint32_t mask = 0;
	uint32_t bitPos[] = {0, 31, 5};
	uint32_t count = sizeof(bitPos) / sizeof(bitPos[0]);

	ASSERT(mask == 0, "mask not initialised to 0.");
	while (count > 0) {
		count--;
		mask = BIT_bitSet(mask, bitPos[count]);
	}
	ASSERT(val == -1, "value not initialised to all bit set.")
	val = BIT_bitClrMask(val, mask);
	ASSERT(BIT_isAllBitClr(val, mask), "Bits not cleared correctly.")

	val = BIT_bitSet(val, bitPos[0]);
	ASSERT(!BIT_isAllBitClr(val, mask), "One bit set, but isAllBitClr still true.")
	ASSERT(BIT_isAnyBitSet(val, mask), "isAnyBitSet failed.")
}

void test_bitAll(void) {
	test_bitSetSingle();
	test_bitClrSingle();
	test_bitSetMulti();
	test_bitClrMulti();
}
