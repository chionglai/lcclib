/*
 * util.h
 *
 *  Created on: 19 Mar 2016
 *      Author: lcc
 */

#ifndef INC_UTIL_H_
#define INC_UTIL_H_


/**
 * @brief Macro to swap two variable.
 * @param[in] a First value to be swapped.
 * @param[in] b Second value to be swapped.
 * @param[in] t Temporary variable for storage.
 * @note All a, b and t must be same data type.
 */
#define UTIL_swap(a, b, t)	{ t = a; a = b; b = t; }

/**
 * @brief Macro to swap two integer variable.
 * @param[in] a First value to be swapped.
 * @param[in] b Second value to be swapped.
 * @note All a and b must be same data type.
 */
#define UTIL_swapInt(a, b)	{ a ^= b; b ^= a; a ^= b; }

/**
 * @brief Macro to clip a value between a lower and upper bound (both inclusive).
 * @param[in] val Variable containing the value to be clipped. Can be a dereferencing
 * 		pointer, e.g. *pVal.
 * @param[in] lower Lower bound, inclusive.
 * @param[in] upper Upper bound, inclusive.
 */
#define UTIL_clip(val, lower, upper)	\
	{ val = (val) < (lower)? (lower) : (val); val = (val) > (upper)? (upper) : (val); }


/**
 * @brief Macro to read a 8-bit value from memory.
 * @param[in] addr Address of memory to read from.
 * @return Value read in unsigned format.
 */
#define UTIL_read8(addr) 	(*(const uint8_t *)(addr))

/**
 * @brief Macro to read a 16-bit value from memory.
 * @param[in] addr Address of memory to read from. Must be aligned.
 * @return Value read in unsigned format.
 */
#define UTIL_read16(addr) 	(*(const uint16_t *)(addr))

/**
 * @brief Macro to read a 32-bit value from memory.
 * @param[in] addr Address of memory to read from. Must be aligned.
 * @return Value read in unsigned format.
 */
#define UTIL_read32(addr) 	(*(const uint32_t *)(addr))

/**
 * @brief Macro to write a 8-bit value to memory.
 * @param[in] addr Address of memory to write to.
 * @param[in] value Value to write.
 */
#define UTIL_write8(addr, value)	*((uint8_t *)(addr)) = (value)

/**
 * @brief Macro to write a 16-bit value to memory.
 * @param[in] addr Address of memory to write to.
 * @param[in] value Value to write.
 */
#define UTIL_write16(addr, value)	*((uint16_t *)(addr)) = (value)

/**
 * @brief Macro to write a 32-bit value to memory.
 * @param[in] addr Address of memory to write to.
 * @param[in] value Value to write.
 */
#define UTIL_write32(addr, value)	*((uint32_t *)(addr)) = (value)

#ifdef __cplusplus
extern "C" {
#endif



#ifdef __cplusplus
}
#endif
#endif /* INC_UTIL_H_ */
