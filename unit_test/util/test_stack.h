/*
 * test_buffer.h
 *
 *  Created on: 8 Jul 2016
 *      Author: chiong
 */

#ifndef TEST_TEST_STACK_H_
#define TEST_TEST_STACK_H_

/**
 * @details Test all
 */
void test_stackAll(void);

/**
 * @details Test stack created with correct size and alignment.
 */
void test_stackCreate(void);

/**
 * @details Test single and nested matched open and close frame
 *      not result in memory leak.
 */
void test_stackOpenCloseFrameMatch(void);

/**
 * @details Test nested non-matched open and close frame result in memory leak.
 *      Test that more close than open frame will not crash, only return to initial
 *      state as when created.
 */
void test_stackOpenCloseFrameNonMatch(void);

/**
 * @details Test stack memory alloc with correct size and alignment.
 * @assert stack_create() is correct.
 */
void test_stackAlloc(void);


#endif /* TEST_TEST_STACK_H_ */
