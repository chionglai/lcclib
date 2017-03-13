/*
 * test_bit.h
 *
 *  Created on: 8 Jul 2016
 *      Author: chiong
 */

#ifndef TEST_TEST_BIT_H_
#define TEST_TEST_BIT_H_

/**
 * @brief All tests.
 */
void test_bitAll(void);

/**
 * @details Tests includes:
 * 		1. bitSet()
 * 		2. isBitSet()
 */
void test_bitSetSingle(void);

/**
 * @details Tests includes:
 * 		1. bitClr()
 * 		2. isBitClr()
 */
void test_bitClrSingle(void);

/**
 * @details Tests includes:
 * 		1. bitSetMask()
 * 		2. isAllBitSet()
 * 		3. isAnyBitClr()
 */
void test_bitSetMulti(void);

/**
 * @details Tests includes:
 * 		1. bitClrMask()
 * 		2. isAllBitClr()
 * 		3. isAnyBitSet()
 */
void test_bitClrMulti(void);


#endif /* TEST_TEST_BIT_H_ */
