/*
 * stack.c
 *
 *  Created on: 8 Jul 2016
 *      Author: chiong
 *
 *
 */

#include <stdlib.h>
#include <string.h>
#include "util/status.h"
#include "util/stack.h"

struct vStack_s {
    /* Read-only. This base address is the raw addr as returned by malloc()
     * without any alignment. It is used for free-ing the memory only. */
    void *baseAddr;
	/* Start address of this stack memory instance. Inclusive. */
	void *startAddr;
	/* Read-only. End address of this stack memory instance. Exclusive.
	 * Used for checking during allocation. */
	void *endAddr;
	/* Current start address of remaining memory. */
	void *currAddr;
	/* Current start of frame address. For nested use. */
	void *currFrameAddr;
	/* Stack frame depth. */
	uint8_t frameDepth;
};

int32_t stack_create(vStack_t **ppScratch, const vStackCfg_t *cfg) {
	vStack_t *stack;
	uintptr_t mask;
	uintptr_t ptr;

	stack = (vStack_t*) malloc(sizeof(vStack_t));
	if (NULL == stack) {
		return STATUS_ERROR_MALLOC;
	}

	stack->baseAddr = malloc(cfg->size + cfg->alignment - 1);
	if (NULL == stack->baseAddr) {
		stack_destroy(&stack);
		return STATUS_ERROR_MALLOC;
	}

	/* Perform manual alignment */
	ptr = (uintptr_t) stack->baseAddr;
	mask = (uintptr_t) (cfg->alignment - 1);
	if (mask) {
	    ptr = (ptr & ~mask) + (uintptr_t) cfg->alignment;
	}

	stack->startAddr = (void*) ptr;
	stack->endAddr = stack->startAddr + cfg->size;
	stack_reset(stack);

	*ppScratch = stack;
	return STATUS_OK;
}

int32_t stack_destroy(vStack_t **ppScratch) {
	vStack_t *stack;

	stack = *ppScratch;
	if (NULL != stack) {
		if (NULL != stack->baseAddr)
			free(stack->baseAddr);

		free(stack);
		*ppScratch = NULL;
	}

	return STATUS_OK;
}

int32_t stack_openFrameWithName(vStack_t *stack, const char *name) {
	if (stack_getAvailSize(stack) >= sizeof(void*)) {
		/* Push current start of frame address to stack, and advance the currAddr
		 * to next available stack. */
		memcpy(stack->currAddr, &stack->currFrameAddr, sizeof(void*));
		stack->currFrameAddr = stack->currAddr;
		stack->currAddr += sizeof(void*);
		/* Increment stack frame depth */
		stack->frameDepth++;
	} else {
		return STATUS_ERROR;
	}

#ifdef DEBUG
	size_t len = strlen(name);
	if (stack_getAvailSize(stack) >= (len + 1)) {
		memcpy(stack->currAddr, name, len + 1);
		stack->currAddr += len + 1;
	} else {
		return STATUS_ERROR;
	}
#else
	(void) name;
#endif

	return STATUS_OK;
}

int32_t stack_closeFrameWithName(vStack_t *stack, const char *name) {
#ifdef DEBUG
	char *ptr = stack->currFrameAddr + sizeof(void*);
	ASSERT(strcmp(name, ptr) != 0, "Potential stack memory leaks!");
#else
	(void) name;
#endif

	if (stack->frameDepth > 0) {
        stack->currAddr = stack->currFrameAddr;
        memcpy(&stack->currFrameAddr, stack->currFrameAddr, sizeof(void*));

        stack->frameDepth--;
        return STATUS_OK;
	} else {
	    return STATUS_ERROR;
	}

#if 0
        if (stack->currFrameAddr < stack->startAddr ||
            stack->currFrameAddr >= stack->endAddr) {
            /* Under normal operation, currFrameAddr will be between startAddr
             * (inclusive) and endAddr (exclusive). This condition will be violated
             * when closeFrame() is called more than openFrame(), but still, it
             * must not crash. */
            stack->currFrameAddr = stack->startAddr;
        }
#endif

}

int32_t stack_reset(vStack_t *stack) {
	stack->currAddr = stack->startAddr;
	stack->currFrameAddr = stack->startAddr;
	stack->frameDepth = 0;
	return STATUS_OK;
}

void* stack_alignedAlloc(vStack_t *stack, uint32_t alignment, uint32_t size) {
	uintptr_t mask;
	uintptr_t ptr;
	uintptr_t endPtr;

	mask = (uintptr_t)(alignment - 1);

	/* Align start pointer */
	ptr = (uintptr_t) stack->currAddr;
	if (mask & ptr) {
	    /* LSB not zero with the alignment mask */
		ptr = (ptr & ~mask) + (uintptr_t)alignment;
	}

	endPtr = (uintptr_t) stack->endAddr;
	if (endPtr - ptr >= size) {
		/* Has enough size */
		stack->currAddr = (void*)(ptr + size * alignment);
	} else {
		/* Insufficient size */
		ptr = (uintptr_t) NULL;
	}

	return (void*) ptr;
}

uint32_t stack_getSize(const vStack_t *stack) {
	return (stack->endAddr - stack->startAddr);
}

uint32_t stack_getAvailSize(const vStack_t *stack) {
	return (stack->endAddr - stack->currAddr);
}
