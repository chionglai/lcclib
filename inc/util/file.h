/*
 * file.h
 *
 *  Created on: 8 Mar 2016
 *      Author: lcc
 *
 * Utility functions for reading data from file.
 *
 */

#ifndef __FILE_H_INCLUDED
#define __FILE_H_INCLUDED

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Macro to read in binary format from the given file until EOF.
 * @param[out] buffer Buffer to store read data. Must have sufficient length.
 * @param[in] size Size, in bytes, of each element.
 * @param[in] filename Filename of file to read from.
 * @returns Number of bytes read, or STATUS_ERROR*.
 */
#define file_readBin(buffer, size, filename)	\
		file_readBinN(buffer, size, 0, filename)

/**
 * @brief Read in binary format from the given filename.
 * @param[out] buffer Buffer to store read data. Must have sufficient length.
 * @param[in] size Size, in bytes, of each element.
 * @param[in] count Number of elements to read. 0 to read all till EOF.
 * @param[in] filename Filename of file to read from.
 * @returns Number of bytes read, or STATUS_ERROR*.
 */
int32_t file_readBinN(void* buffer, uint32_t size, uint32_t count, const char* filename);

#ifdef __cplusplus
}
#endif

#endif /* __FILE_H_INCLUDED */
