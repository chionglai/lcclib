/*
 * std.h
 *
 *  Created on: 13 Jul 2016
 *      Author: chiong
 */

#ifndef INC_STD_H_
#define INC_STD_H_

#include <stdint.h>

#define STD_MAX_NUM_CHAR	(32)

/**
 * @brief Function to read a line from standard input.
 * @details If the str/line has length that exceeds the size of str buffer,
 * 		this function will only read the first len-1 characters, inserting
 * 		a null character at the end. Whenever a newline '\n', carriage return
 * 		'\r' and EOF (-1) is encountered, the line is considered finished
 * 		and those characters will be replaced by a null character,
 * 		indicating end of string.
 * @param[out] str Buffer to store the string read. Must have len size.
 * 		Upon successful read, will always terminated with a null character.
 * @param[in] len Length of str buffer.
 * @param[in] msg Prompt message;
 * @return Number of character read, excluding null and newline character.
 * 		STATUS_ERROR if error in reading.
 */
int32_t std_in_readLine(char *str, uint32_t len, char *msg);

/**
 * @brief Read an integer from standard input.
 * @details If this function returns 0, the value *out is not modified.
 * 		This allows the default value to be assigned to *out before
 * 		calling this function.
 * @param[out] out Pointer to contain integer read.
 * @param[in] msg Prompt message.
 * @return 1 if this function successfully read an integer, STATUS_ERROR otherwise.
 */
int32_t std_in_readInt(int *out, char *msg);

/**
 * @brief Read a float from standard input.
 * @details If this function returns 0, the value *out is not modified.
 * 		This allows the default value to be assigned to *out before
 * 		calling this function.
 * @param[out] out Pointer to contain float read.
 * @param[in] msg Prompt message.
 * @return 1 if this function successfully read a float, STATUS_ERROR otherwise.
 */
int32_t std_in_readFloat(float *out, char *msg);

#endif /* INC_STD_H_ */
