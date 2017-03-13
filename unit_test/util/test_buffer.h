/*
 * test_buffer.h
 *
 *  Created on: 8 Jul 2016
 *      Author: chiong
 */

#ifndef TEST_TEST_BUFFER_H_
#define TEST_TEST_BUFFER_H_

/**
 * @brief Run all test.
 */
void test_bufferAll(void);

/**
 * @details Test includes:
 * 		1. create()
 * 		2. destroy()
 * 		3. All getters.
 */
void test_buffer2dCreateDestroy(void);

/**
 * @details Test includes:
 * 		1. putDataSingle()
 * 		2. getDataSingle()
 */
void test_buffer2dPutGetSingle(void);

/**
 * @details Test includes:
 * 		1. putDataRow()
 * 		2. getDataRow()
 */
void test_buffer2dPutGetRow(void);

/**
 * @details Test includes:
 * 		1. putDataCol()
 * 		2. getDataCol()
 */
void test_buffer2dPutGetCol(void);

/**
 * @details Test includes:
 * 		1. fill()
 */
void test_buffer2dFill(void);

#endif /* TEST_TEST_BUFFER_H_ */
