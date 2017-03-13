/*
 * test_buffer.c
 *
 *  Created on: 8 Jul 2016
 *      Author: chiong
 */

#include <stdio.h>
#include "util/stack.h"
#include "debug/assert.h"
#include "test_stack.h"


void test_stackCreate(void) {
    vStack_t *stack;
    vStackCfg_t cfg;
    void *ptr;
    uintptr_t mask;

    cfg.alignment = 4;
    cfg.size = 20;
    stack_create(&stack, &cfg);

    ASSERT(stack_getSize(stack) == cfg.size, "Stack created with incorrect size.");
    ptr = stack_alloc(stack, cfg.size);
    mask = (uintptr_t)(cfg.alignment - 1);
    ASSERT((((uintptr_t) ptr) & mask) == 0, "Stack created with incorrect alignment.");

    stack_destroy(&stack);
}

void test_stackOpenCloseFrameMatch(void) {
    vStack_t *stack;
    vStackCfg_t cfg;
    void *ptr0;
    void *ptr1;
    void *ptr2;
    void *dummy;

    cfg.alignment = 1;
    cfg.size = 1024;
    stack_create(&stack, &cfg);

    ptr0 = stack_alloc(stack, 1);
    stack_openFrame(stack);
    dummy = stack_alloc(stack, 17);
    dummy = stack_alloc(stack, 25);

    ptr1 = stack_alloc(stack, 1);
    stack_openFrame(stack);
    dummy = stack_alloc(stack, 17);
    dummy = stack_alignedAlloc(stack, 4, 25);
    stack_closeFrame(stack);

    ptr2 = stack_alloc(stack, 1);
    ASSERT((ptr2 - ptr1) == 1, "Incorrect matched inner open and close frame.");

    stack_closeFrame(stack);
    ptr2 = stack_alloc(stack, 1);
    ASSERT((ptr2 - ptr0) == 1, "Incorrect matched outer open and close frame.");

    (void) dummy;
    stack_destroy(&stack);
}

void test_stackOpenCloseFrameNonMatch(void) {
    vStack_t *stack;
    vStackCfg_t cfg;
    void *ptr0;
    void *ptr1;
    void *ptr2;
    void *dummy;

    cfg.alignment = 4;
    cfg.size = 20;
    stack_create(&stack, &cfg);

    ptr0 = stack_alloc(stack, 1);
    stack_openFrame(stack);
    dummy = stack_alloc(stack, 17);
    dummy = stack_alloc(stack, 25);

    (void) dummy;
    stack_closeFrame(stack);
    ptr1 = stack_alloc(stack, 1);
    ASSERT(ptr1 - ptr0 == 1, "Matched open and close frame. ptr1 should be next to ptr0.");

    /* Extra close frame */
    stack_closeFrame(stack);
    ptr2 = stack_alloc(stack, 1);
    ASSERT(ptr2 - ptr1 == 1, "Extra close should not have any effect. ptr2 should be next to ptr1.");

    stack_destroy(&stack);
}

#define TEST_UINT8_SIZE     (7)
#define TEST_UINT16_SIZE    (12)
#define TEST_UINT32_SIZE    (20)

void test_stackAlloc(void) {
    vStack_t *stack;
    vStackCfg_t cfg;
    void *ptr;
    uintptr_t mask;
    uint32_t expectedRemSize;

    cfg.alignment = 1;
    cfg.size = 1024;
    stack_create(&stack, &cfg);

    /* alloc uint8 aligned memory */
    ptr = stack_alloc(stack, TEST_UINT8_SIZE);
    ASSERT(NULL != ptr, "Unexpected NULL.");
    expectedRemSize = cfg.size - TEST_UINT8_SIZE;
    ASSERT(stack_getAvailSize(stack) == expectedRemSize, "Incorrect remaining size after allocate uint8.");

    /* alloc uint16 aligned memory */
    ptr = stack_alignedAlloc(stack, 2, TEST_UINT16_SIZE);
    ASSERT(NULL != ptr, "Unexpected NULL.");
    mask = (uintptr_t) (2 - 1);
    ASSERT((((uintptr_t) ptr) & mask) == 0, "Incorrect alignment for uint16.");
    expectedRemSize = cfg.size - TEST_UINT8_SIZE - (TEST_UINT8_SIZE & mask)
                - TEST_UINT16_SIZE * 2;
    ASSERT(stack_getAvailSize(stack) == expectedRemSize, "Incorrect remaining size after allocate uint16.");

    /* alloc uint32 aligned memory */
    ptr = stack_alignedAlloc(stack, 4, TEST_UINT32_SIZE);\
    ASSERT(NULL != ptr, "Unexpected NULL.");
    mask = (uintptr_t) (4 - 1);
    ASSERT((((uintptr_t) ptr) & mask) == 0, "Incorrect alignment for uint32.");
    expectedRemSize = cfg.size - TEST_UINT8_SIZE - (TEST_UINT8_SIZE & 1)
            - TEST_UINT16_SIZE * 2
            - TEST_UINT32_SIZE * 4;
    if ((TEST_UINT16_SIZE * 2) & mask) {
        expectedRemSize -= 4 - mask;
    }
    ASSERT(stack_getAvailSize(stack) == expectedRemSize, "Incorrect remaining size after allocate uint32.");

    /* attempt to alloc size bigger than available */
    ptr = stack_alloc(stack, stack_getAvailSize(stack) + 1);
    ASSERT(NULL == ptr, "Expected NULL, but did not get it.");

    /* attempt to alloc just enough to make the stack full */
    ptr = stack_alloc(stack, stack_getAvailSize(stack));
    ASSERT(NULL != ptr, "Not expecting NULL.");
    ASSERT(stack_getAvailSize(stack) == 0, "Incorrect available size.");

    stack_destroy(&stack);
}

void test_stackAll(void) {
    test_stackCreate();
    test_stackOpenCloseFrameMatch();
    test_stackOpenCloseFrameNonMatch();
    test_stackAlloc();
}

