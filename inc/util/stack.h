/*
 * stack.h
 *
 *  Created on: 19 Mar 2016
 *      Author: lcc
 *
 *  Implementation of stack memory. The stack memory can be used to dynamically
 *  allocate memory for temporary/intermediate variables whose scope does not extend
 *  outside the function they are allocated.
 *
 *  This implementation allows the same stack to be used in nested function call by a
 *  matching stack_openFrame() and stack_closeFrame() calls. E.g.
 *
 *
 *  void func0(...) {
 *  	stack_openFrame(stack);
 *  	...
 *  	stack_coseFrame(stack);
 *
 *  }
 *
 *  void func1(...) {
 *  	stack_openFrame(stack);
 *  	...
 *  	func0(...);
 *  	...
 *  	stack_closeFrame(stack);
 *  }
 *
 *  Structure of stack
 *                                      currAddr
 *                                         v
 *  +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
 *  |  X  |  X  |     |     |     |  Y  |     |     |     |     |     |
 *  +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
 *     ^           ^                 ^                                   ^
 *  baseAddr     startAddr      currFrameAddr                         endAddr (exclusive)
 *
 *  baseAddr is the raw start address from malloc. May not be the startAddr as it
 *      may not be aigned.
 *  startAddr is the actual start address of stack, with user-specified alignment.
 *      Before this, there may be unused memory (marked by 'X') due to alignment.
 *  currFrameAddr is the 'start' address of the current stack frame. This address
 *      is not used to allocate stack memory for the current frame as it contains
 *      Y = address of previous start frame address. The value Y is popped back to
 *      currFrameAddr on closeFrame() and is pushed to stack on openFrame().
 *  currAddr is the current start address to allocate memory is stack. Every time
 *      memory is allocated, currAddr will be updated.
 *
 */

#ifndef INC_STACK_H_
#define INC_STACK_H_

#include <stdint.h>

/**
 * @brief Macro to open current frame in stack with the name as the calling function
 * 		name.
 * @param[in/out] stack A stack memory instance.
 * @return STATUS_OK if successful, STATUS_ERROR* otherwise.
 */
#define stack_openFrame(stack)	\
	stack_openFrameWithName((stack), __func__)

/**
 * @brief Macro to close current frame in stack with the name as the calling function
 * 		name.
 * @param[in/out] stack A stack memory instance.
 * @return STATUS_OK if successful, STATUS_ERROR* otherwise.
 */
#define stack_closeFrame(stack)	\
		stack_closeFrameWithName((stack), __func__)

/**
 * @brief Macro to allocate a memory in stack with byte alignment.
 * @param[in/out] stack Scratch memory instance.
 * @param[in] size Size, in byte, of memory to allocate.
 * @return The address of the allocated memory in stack memory. NULL if allocation
 * 		fails.
 */
#define stack_alloc(stack, size)	\
	stack_alignedAlloc((stack), 1, (size));

typedef struct vStack_s vStack_t;

typedef struct {
	/* Alignment, in byte. Must be power of 2. Undefined behaviour if not. */
	uint32_t alignment;
	/* Size, in bytes */
	uint32_t size;
} vStackCfg_t;


#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief To create a stack memory instance.
 * @param[in/out] ppScratch Address to store the newly created stack memory instance.
 * @param[in] cfg Configuration used to create the stack memory instance.
 * @return STATUS_OK if success, STATUS_ERROR* otherwise.
 */
int32_t stack_create(vStack_t **ppScratch, const vStackCfg_t *cfg);

/**
 * @brief To destroy and release the resources of a stack memory instance.
 * @param[in/out] ppScratch Address of stack memory instance to be destroyed.
 * @return STATUS_OK if success, STATUS_ERROR* otherwise.
 */
int32_t stack_destroy(vStack_t **ppScratch);

/**
 * @brief To start a new 'frame' in stack to be used within the scope of a matching
 * 		pair of stack_openFrameWithName() and stack_closeFrameWithName().
 * @details Before the stack can be used in a function, stack_openFrameWithName() must
 * 		be called. This will push the current start address of the stack instance to
 * 		the stack itself, allowing for nested use. However, each open frame must be
 * 		accompanied with its close frame (with same name) once finish using the
 * 		'frame'. E.g.
 *
 * 		stack_openFrameWithName(stack, "a");
 * 		stack_alignedAlloc(..);		// memory allocated in frame "a"
 * 		stack_openFrameWithName(stack, "b");
 * 		stack_alignedAlloc(..);		// memory allocated in frame "b"
 * 		// do something
 * 		stack_closeFrameWithName(stack, "b");
 * 		// do other thing
 * 		stack_closeFrameWithName(stack, "a");
 *
 * 		Note that the openFrame() and closeFrame() are to allow the stack to be
 * 		used in nested manner in a function. If no nested manner is required,
 * 		the stack can be used as it is without calling the open and close frame
 * 		functions. In this case, the stack can be reset to its initial state
 * 		every time, before it is used.
 *
 * @param[in/out] stack A stack memory instance.
 * @param[in] name A string to uniquely identify the current frame opened in the stack.
 * 		Only used when DEBUG is #defined to check for a matching open and close frame.
 * @return STATUS_OK if successful, STATUS_ERROR* otherwise.
 */
int32_t stack_openFrameWithName(vStack_t *stack, const char *name);

/**
 * @brief To end the current 'frame' in stack.
 * @details Must be called once finish using the current frame in stack. Each frame
 * 		in stack opened must be closed to avoid memory leak.
 * @param[in/out] stack A stack memory instance.
 * @param[in] name A string to uniquely identify the current frame closed in the stack.
 * 		Only used when DEBUG is #defined to check for a matching open and close frame.
 * @return STATUS_OK if successful, STATUS_ERROR* otherwise.
 */
int32_t stack_closeFrameWithName(vStack_t *stack, const char *name);

/**
 * @brief Reset the stack memory.
 * @details This function resets the internal pointer of stack memory instance as a
 * 		way to 'free' all the temporary variables in it. It does not erase the actual
 * 		data in the stack memory.
 * @param[in/out] stack Scratch memory instance.
 * @return STATUS_OK if success, STATUS_ERROR* otherwise.
 */
int32_t stack_reset(vStack_t *stack);

/**
 * @brief Allocate an aligned memory in stack memory.
 * @details If padding is required, the memory before the newly aligned and allocated
 * 		memory is padded and thus, cannot be reused. E.g.
 *
 * 		Address 0 1 2 3 4 5 6 7 8
 * 		          ^     ^
 * 		    Current 	Newly allocated memory with alignment = 4
 * 		    pointer
 *
 * 		Memory at address 2 and 3 are padding and will be wasted. Even if after the
 * 		above allocation, a new single byte stack memory is desired, it will NOT
 * 		be allocated at address 2 or 3, but will be allocated after the above
 * 		allocation.
 * @param[in/out] stack Scratch memory instance.
 * @param[in] alignment Alignment, in bytes, of the memory to be allocated. Must
 * 		be power of 2, otherwise undefined behaviour.
 * @param[in] size Size, in number of elements (not bytes), of memory to allocate.
 * @return The address of the allocated memory in stack memory. NULL if allocation
 * 		fails.
 */
void* stack_alignedAlloc(vStack_t *stack, uint32_t alignment, uint32_t size);

/**
 * @brief Get the total size of the stack memory.
 * @param[in] stack Scratch memory instance.
 * @return The total size, in bytes.
 */
uint32_t stack_getSize(const vStack_t *stack);

/**
 * @brief Get the currently available/remaining size of the stack memory.
 * @param[in] stack Scratch memory instance.
 * @return The available/remaining size, in bytes.
 */
uint32_t stack_getAvailSize(const vStack_t *stack);


#ifdef __cplusplus
}
#endif

#endif /* INC_STACK_H_ */
