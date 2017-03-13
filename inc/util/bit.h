/*
 * bit.h
 *
 *  Created on: 8 Mar 2016
 *      Author: lcc
 *
 * For 1.8inch TFT LCD driven by ST7735. Although the pins are labeled as SDA, SCL, CS, it
 * does not follow any standard I2C and SPI convention.
 * 1. There is no I2C slave addressing,
 * 2. The SDA line acts as both (host -> LCD) and (LCD -> host),
 * 3. Chip select CS is active low for both command and data,
 * 4. For 4-line mode, RESET line is LO for command, and HI for data,
 * 5. For my module, it is hardware configured to use 4-line serial bus (to be confirmed)
 *
 */

#ifndef __BIT_H_INCLUDED
#define __BIT_H_INCLUDED

#include "xtype.h"

/**
 * @brief Macro to set a bit
 * @param[in] val Value whose bit to be set.
 * @param[in] pos Bit position to be set. LSB is bit[0].
 * @return Value with the bit in the given position set.
 */
#define BIT_bitSet(val, pos)  ((val) | (1ul << (pos)))

/**
 * @brief Macro to clear a bit.
 * @param[in] val Value whose bit to be cleared.
 * @param[in] pos Bit position to be cleared. LSB is bit[0].
 * @return Value with the bit in the given position cleared.
 */
#define BIT_bitClr(val, pos)  ((val) & ~(1ul << (pos)))

/**
 * @brief Macro to set all the bits according to the given mask.
 * @param[in] val A value.
 * @param[in] mask Mask for setting bits. All the bit[n] in val that correspond
 *      to bit[n] == 1 in mask will be set. bit[m] with bit[m] == 0 in mask
 *      remain unchanged.
 * @return Value with bits set according to mask. E.g. val = 0100b, mask = 1001b,
 *      return = 1101b.
 */
#define BIT_bitSetMask(val, mask)   ((val) | (mask))

/**
 * @brief Macro to clear all the bits according to the given mask.
 * @param[in] val A value.
 * @param[in] mask Mask for setting bits. All the bit[n] in val that correspond
 *      to bit[n] == 1 in mask will be cleared. bit[m] with bit[m] == 0 in mask
 *      remain unchanged.
 * @return Value with bits cleared according to mask. E.g. val = 1101b, mask = 1001b,
 *      return = 0100b.
 */
#define BIT_bitClrMask(val, mask)   ((val) & ~(mask))

/**
 * @brief Macro to test if a bit in the given position is set.
 * @param[in] val A value to be checked.
 * @param[in] pos Bit position. LSB is bit[0].
 * @return 1 if the bit at specified position is set. 0 otherwise.
 */
#define BIT_isBitSet(val, pos)  (((val) & 1ul << (pos))? TRUE : FALSE)

/**
 * @brief Macro to test if a bit in the given position is cleared.
 * @param[in] val A value to be checked.
 * @param[in] pos Bit position. LSB is bit[0].
 * @return 1 if the bit at specified position is cleared. 0 otherwise.
 */
#define BIT_isBitClr(val, pos)  !BIT_isBitSet(val, pos)

/**
 * @brief Macro to check if ALL the bits (as specified by the mask) are set.
 * @details For example, given val = 0b01110111 and mask = 0b01100010, the bit positions
 * 		of 1, 5 and 6 (specified by mask) in val will be checked. In this case, all bit[1,5,6]
 * 		in val are set so 1 will be returned.
 * @param[in] val A value to be checked.
 * @param[in] mask Mask, with bits that correspond to bit position in val to be checked
 * 		set to 1 and 0 to ignore.
 * @return 1 if ALL bits (as given by mask) in val are set, 0 otherwise.
 */
#define BIT_isAllBitSet(val, mask)  ((((val) & (mask)) == mask)? TRUE : FALSE)

/**
 * @brief Macro to check if ALL the bits (as specified by the mask) are cleared.
 * @param[in] val A value to be checked.
 * @param[in] mask Mask, with bits that correspond to bit position in val to be checked
 * 		set to 1 and 0 to ignore.
 * @return 1 if ALL bits (as given by mask) in val are cleared, 0 otherwise.
 */
#define BIT_isAllBitClr(val, mask)  !BIT_isAnyBitSet(val, mask)

/**
 * @brief Macro to check if ANY the bits (as specified by the mask) are set.
 * @details For example, given val = 0b00000111 and mask = 0b01100010, the bit positions
 * 		of 1, 5 and 6 (specified by mask) in val will be checked. In this case, bit[1]
 * 		(any of bit 1, 5, or 6) in val is set so 1 will be returned.
 * @param[in] val A value to be checked.
 * @param[in] mask Mask, with bits that correspond to bit position in val to be checked
 * 		set to 1 and 0 to ignore.
 * @return 1 if ANY bits (as given by mask) in val are set, 0 otherwise.
 */
#define BIT_isAnyBitSet(val, mask)  (((val) & (mask))? TRUE : FALSE)

/**
 * @brief Macro to check if ANY the bits (as specified by the mask) are cleared.
 * @param[in] val A value to be checked.
 * @param[in] mask Mask, with bits that correspond to bit position in val to be checked
 * 		set to 1 and 0 to ignore.
 * @return 1 if ANY bits (as given by mask) in val are cleared, 0 otherwise.
 */
#define BIT_isAnyBitClr(val, mask)  !BIT_isAllBitSet(val, mask)

#ifdef __cplusplus
extern "C" {
#endif


#ifdef __cplusplus
}
#endif

#endif /* __BIT_H_INCLUDED */
