/*
 * buffer.h
 *
 *  Created on: 19 Mar 2016
 *      Author: lcc
 *
 *  2D buffer implementation. The data is internally stored as 1D contiguous buffer.
 *  The data layout can either be row wise or column wise. Because of this flexibility,
 *  the same implementation can be used for storing multi-channel data (e.g. audio data)
 *  that allows for both interleaved and non-interleaved data. This multichannel buffer
 *  is implemented as macro as it uses the same API. For simplicity, the multi-channel
 *  data will always be interpreted such that the columns represents the channels and
 *  the row represent the time, as follows.
 *
 *  		channel_0, channel_1, ..., channel_k
 *  n = 0      x          x       ...     x
 *  n = 1      x          x               x
 *  ...
 *  n = N-1    x          x               x
 *
 *  For this interpretation, the buffer will have non-interleaved data when layout ==
 *  BUFFER2D_LAYOUT_COLUMN_WISE and interleaved data when layout == BUFFER2D_LAYOUT_ROW_WISE
 */

#ifndef INC_BUFFER_H_
#define INC_BUFFER_H_

#include <stdint.h>
#include "status.h"

/* 2D buffer internal data layout. */
#define BUFFER2D_LAYOUT_COLUMN_WISE		(0x00)
#define BUFFER2D_LAYOUT_ROW_WISE		(0x01)

/**
 * @brief Macro to return the pointer to internal data of 2D buffer as specific type.
 * @param[in] pSelf 2D buffer instance.
 * @param[in] type Data type.
 */
#define BUFFER2D_getBufferAsType(pSelf, type)	\
	((type*) buffer2d_getBuffer(pSelf))

/* Layout for multi-channel buffer. */
#define MCBUFFER_LAYOUT_NON_INTERLEAVED	BUFFER2D_LAYOUT_COLUMN_WISE
#define MCBUFFER_LAYOUT_INTERLEAVED		BUFFER2D_LAYOUT_ROW_WISE

/* All (in macro only) implementation for multi-channel buffer. */
/**
 * @brief Create a multi-channel buffer.
 * @param[out] ppSelf Pointer to store newly created 2D buffer instance.
 * @param[in] nSample Number of samples per channel.
 * @param[in] nChannel Number of channels.
 * @param[in] elemSize Size, in bytes, for each sample/element.
 * @param[in] Either MCBUFFER_LAYOUT_[INTERLEAVED|NON_INTERLEAVED]
 * @return Status code.
 */
#define MCBUFFER_create(ppSelf, nSample, nChannel, elemSize, layout)	\
	buffer2d_create(ppSelf, nSample, nChannel, elemSize, layout);

/**
 * @brief Macro to destroy a multi-channel buffer.
 * @param[in] ppSelf Buffer to be destroyed.
 * @return Status code.
 */
#define MCBUFFER_destroy(ppSelf)	\
	buffer2d_destroy(ppSelf)

/**
 * @brief Macro
 * @param[in/out] pSelf Multi-channel buffer instance.
 * @param[in]
 * @param[in]
 */
#define MCBUFFER_putDataAtChannel(pSelf, channel, data)	\
	buffer2d_putDataCol(pSelf, channel, data)

/**
 * @brief Macro
 * @param[in/out] pSelf Multi-channel buffer instance.
 * @param[in]
 * @param[in]
 */
#define MCBUFFER_getDataAtChannel(pSelf, channel, data)	\
	buffer2d_getDataCol(pSelf, channel, data)

/**
 * @brief Macro
 * @param[in/out] pSelf Multi-channel buffer instance.
 * @param[in]
 * @param[in]
 */
#define MCBUFFER_putDataAtIndex(pSelf, n, data)	\
	buffer2d_putDataRow(pSelf, n, data)

/**
 * @brief Macro
 * @param[in/out] pSelf Multi-channel buffer instance.
 * @param[in]
 * @param[in]
 */
#define MCBUFFER_getDataAtIndex(pSelf, n, data)	\
	buffer2d_getDataRow(pSelf, n, data)

/**
 * @brief Macro
 * @param[in] pSelf Multi-channel buffer instance.
 * @return
 */
#define MCBUFFER_getNumChannel(pSelf)	\
	buffer2d_getNumCol(pSelf)

/**
 * @brief Macro
 * @param[in] pSelf Multi-channel buffer instance.
 * @return
 */
#define MCBUFFER_getNumSamplePerChannel(pSelf)	\
	buffer2d_getNumRow(pSelf)

/**
 * @brief Macro
 * @param[in] pSelf Multi-channel buffer instance.
 * @return
 */
#define MCBUFFER_getElemSize(pSelf)	\
	buffer2d_getElemSize(pSelf)

/**
 * @brief Macro
 * @param[in] pSelf Multi-channel buffer instance.
 * @param[in]
 * @return
 */
#define MCBUFFER_getBufferAsType(pSelf, type)	\
	BUFFER2D_getBufferAsType(pSelf, type)

/**
 * @brief Data struct for 2D buffer stored as 1d array.
 */
typedef struct buffer2d_s buffer2d_t;

/**
 * @brief Data struct for multi-channel buffer stored as 1d array.
 */
typedef struct buffer2d_s mcbuffer_t;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Create a 2D buffer.
 * @param[out] ppSelf Pointer to store newly created 2D buffer instance.
 * @param[in] nRow Number of rows.
 * @param[in] nCol Number of columns.
 * @param[in] elemSize Size, in bytes of each element.
 * @param[in] layout To determine how the data is interleaved internally.
 * 		Though any value is valid, if we consider each column to be a 'channel'
 * 		and each row to be a 'time-sliced' data, useful values are:
 * 		1. BUFFER2D_NSTRIDE_ROW_WISE: For interleaved data
 * 		2. BUFFER2D_NSTRIDE_COLUMN_WISE: For non-interleaved data
 * @return Status code.
 */
int32_t buffer2d_create(buffer2d_t **ppSelf, uint32_t nRow, uint32_t nCol, uint32_t elemSize, uint8_t layout);

/**
 * @brief Destroy and free a 2D buffer instance.
 * @param[in/out] ppSelf 2D buffer instance to be destroyed. Once destroyed,
 * 		*ppSelf will be NULL.
 * @return Status code.
 */
int32_t buffer2d_destroy(buffer2d_t **ppSelf);

/**
 * @brief Put a single element at row i and column j.
 * @param[in/out] pSelf 2D buffer instance.
 * @param[in] iRow Index of row. Must be within the number of rows when pSelf is created.
 * @param[in] jCol Index of column. Must be within the number of columns when pSelf is created.
 * @param[in] data Data to be put in buffer.
 */
void buffer2d_putDataSingle(buffer2d_t *pSelf, uint32_t iRow, uint32_t jCol, const void *data);

/**
 * @brief Put a row of data into 2D buffer.
 * @param[in/out] pSelf 2D buffer instance.
 * @param[in] Index of row. Must be within the number of rows when pSelf is created.
 * @param[in] data Data to be put into. The data length must match the number
 * 		of columns of 2D buffer.
 */
void buffer2d_putDataRow(buffer2d_t *pSelf, uint32_t iRow, void *data);

/**
 * @brief Put a column of data into 2D buffer.
 * @param[in/out] pSelf 2D buffer instance.
 * @param[in] Index of column. Must be within the number of columns when pSelf is created.
 * @param[in] data Data to be put into. The data length must match the number
 * 		of rows of 2D buffer.
 */
void buffer2d_putDataCol(buffer2d_t *pSelf, uint32_t jCol, void *data);

/**
 * @brief Get a single element at row i and column j.
 * @param[in] pSelf 2D buffer instance.
 * @param[in] iRow Index of row. Must be within the number of rows when pSelf is created.
 * @param[in] jCol Index of column. Must be within the number of columns when pSelf is created.
 * @param[out] data Pointer to store the returned single data.
 */
void buffer2d_getDataSingle(const buffer2d_t *pSelf, uint32_t iRow, uint32_t jCol, void *data);

/**
 * @brief Get a row of data from 2D buffer.
 * @param[in] pSelf 2D buffer instance.
 * @param[in] Index of row. Must be within the number of rows when pSelf is created.
 * @param[out] data Buffer to contain the data from buffer instance. The buffer size must
 * 		be sufficient to contain the data.
 */
void buffer2d_getDataRow(buffer2d_t *pSelf, uint32_t iRow, void *data);

/**
 * @brief Get a column of data from 2D buffer.
 * @param[in] pSelf 2D buffer instance.
 * @param[in] Index of column. Must be within the number of columns when pSelf is created.
 * @param[out] data Buffer to contain the data from buffer instance. The buffer size must
 * 		be sufficient to contain the data.
 */
void buffer2d_getDataCol(buffer2d_t *pSelf, uint32_t jCol, void *data);

/**
 * @brief Fill the buffer with the given single element.
 * @param[in/out] 2D buffer instance.
 * @param[in] elem Element for filling.
 */
void buffer2d_fill(buffer2d_t *pSelf, void *elem);

/**
 * @brief Get the size, in bytes of 2D buffer element.
 * @param[in] pSelf 2D buffer instance.
 * @return Element size, in bytes
 */
uint32_t buffer2d_getElemSize(const buffer2d_t *pSelf);

/**
 * @brief Get the number of rows.
 * @param[in] pSelf 2D buffer instance.
 * @return Number of rows.
 */
uint32_t buffer2d_getNumRow(buffer2d_t *pSelf);

/**
 * @brief Get the number of columns.
 * @param[in] pSelf 2D buffer instance.
 * @return Number of columns.
 */
uint32_t buffer2d_getNumCol(buffer2d_t *pSelf);

/**
 * @brief Get the address of the internal buffer..
 * @param[in] pSelf 2D buffer instance.
 * @return Address of internal buffer
 */
const void* buffer2d_getBuffer(buffer2d_t *pSelf);

/**
 * @brief Print the content of 2D buffer on stdout.
 * @param[in] 2D buffer instance.
 */
void buffer2d_print(buffer2d_t *pSelf);

#ifdef __cplusplus
}
#endif

#endif /* INC_BUFFER_H_ */
