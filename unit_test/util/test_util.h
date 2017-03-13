/*
 * test_util.h
 *
 *  Created on: 8 Jul 2016
 *      Author: chiong
 */

#ifndef TEST_TEST_UTIL_H_
#define TEST_TEST_UTIL_H_

/**
 * @brief Test util all.
 */
void test_utilAll(void);

/**
 * @details test swap() for various native types.
 */
void test_utilSwap(void);

/**
 * @details test clip() for various native types.
 */
void test_utilClip(void);

/**
 * @details Test includes:
 * 		1. read8()
 * 		2. write8()
 */
void test_utilRw8(void);

/**
 * @details Test includes:
 * 		1. read16()
 * 		2. write16()
 */
void test_utilRw16(void);

/**
 * @details Test includes:
 * 		1. read32()
 * 		2. write32()
 */
void test_utilRw32(void);


#endif /* TEST_TEST_UTIL_H_ */
