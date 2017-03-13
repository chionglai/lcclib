/*
 * test_buffer.c
 *
 *  Created on: 8 Jul 2016
 *      Author: chiong
 */

#include <stdio.h>
#include "util/buffer.h"
#include "debug/assert.h"
#include "test_buffer.h"

#define TEST_BUFFER2D_NROW		(3)
#define TEST_BUFFER2D_NCOL		(5)

void test_buffer2dCreateDestroy(void) {
	buffer2d_t *pBuffer = NULL;
	uint32_t *pArr;
	uint32_t i;

	/* Test create */
	buffer2d_create(&pBuffer, TEST_BUFFER2D_NROW, TEST_BUFFER2D_NCOL, sizeof(*pArr), BUFFER2D_LAYOUT_COLUMN_WISE);
	ASSERT(NULL != pBuffer, "2D buffer not created correctly.")

	/* Test getters */
	ASSERT(buffer2d_getNumRow(pBuffer) == TEST_BUFFER2D_NROW, "Incorrect number of rows.");
	ASSERT(buffer2d_getNumCol(pBuffer) == TEST_BUFFER2D_NCOL, "Incorrect number of columns.");
	ASSERT(buffer2d_getElemSize(pBuffer) == sizeof(*pArr), "Incorrect element size.");

	pArr = BUFFER2D_getBufferAsType(pBuffer, uint32_t);
	ASSERT(NULL != pArr, "Null buffer data.");
	for (i = 0; i < (TEST_BUFFER2D_NROW * TEST_BUFFER2D_NCOL); i++) {
		ASSERT(pArr[i] == 0, "Buffer data not initialised correctly.");
	}

	/* Test destroy */
	buffer2d_destroy(&pBuffer);
	ASSERT(NULL == pBuffer, "2D buffer not destroyed correctly.");
}

void test_buffer2dPutGetSingle(void) {
	buffer2d_t *pBuffer = NULL;
	uint32_t iRow, jCol;
	float val, temp;

	/* Test row wise */
	val = 12.345f;
	iRow = 2;
	jCol = 4;
	buffer2d_create(&pBuffer, TEST_BUFFER2D_NROW, TEST_BUFFER2D_NCOL, sizeof(val), BUFFER2D_LAYOUT_ROW_WISE);
	buffer2d_putDataSingle(pBuffer, iRow, jCol, &val);
	buffer2d_getDataSingle(pBuffer, iRow, jCol, &temp);
	ASSERT(temp == val, "put and/or get single data failed.");
	buffer2d_destroy(&pBuffer);

	/* Test column wise */
	val = -12.345f;
	iRow = 1;
	jCol = 3;
	buffer2d_create(&pBuffer, TEST_BUFFER2D_NROW, TEST_BUFFER2D_NCOL, sizeof(val), BUFFER2D_LAYOUT_COLUMN_WISE);
	buffer2d_putDataSingle(pBuffer, iRow, jCol, &val);
	buffer2d_getDataSingle(pBuffer, iRow, jCol, &temp);
	ASSERT(temp == val, "put and/or get single data failed.");
	buffer2d_destroy(&pBuffer);
}

void test_buffer2dPutGetRow(void) {
	buffer2d_t *pBuffer = NULL;
	uint32_t iRow, i;
	float val[TEST_BUFFER2D_NCOL], temp[TEST_BUFFER2D_NCOL];
	float *pData;

	for (i = 0; i< TEST_BUFFER2D_NCOL; i++) {
		val[i] = 3.1f * (i+1);
	}

	/* Test row wise */
	iRow = 2;
	buffer2d_create(&pBuffer, TEST_BUFFER2D_NROW, TEST_BUFFER2D_NCOL, sizeof(val[0]), BUFFER2D_LAYOUT_ROW_WISE);
	buffer2d_putDataRow(pBuffer, iRow, val);
	buffer2d_getDataRow(pBuffer, iRow, temp);

	for (i = 0; i < TEST_BUFFER2D_NCOL; i++) {
		ASSERT(temp[i] == val[i], "put and/or get row data for row wise failed.");
	}

	pData = BUFFER2D_getBufferAsType(pBuffer, float);
	pData += iRow * TEST_BUFFER2D_NCOL;
	for (i = 0; i < TEST_BUFFER2D_NCOL; i++) {
		ASSERT(pData[i] == val[i], "Row data for row wise layout not correct.");
	}
	buffer2d_destroy(&pBuffer);

	/* Test column wise */
	iRow = 2;
	buffer2d_create(&pBuffer, TEST_BUFFER2D_NROW, TEST_BUFFER2D_NCOL, sizeof(val[0]), BUFFER2D_LAYOUT_COLUMN_WISE);
	buffer2d_putDataRow(pBuffer, iRow, val);
	buffer2d_getDataRow(pBuffer, iRow, temp);

	for (i = 0; i < TEST_BUFFER2D_NCOL; i++) {
		ASSERT(temp[i] == val[i], "put and/or get row data for column wise failed.");
	}

	pData = BUFFER2D_getBufferAsType(pBuffer, float);
	pData += iRow;
	for (i = 0; i < TEST_BUFFER2D_NCOL; i++) {
		ASSERT(pData[i * TEST_BUFFER2D_NROW] == val[i], "Row data for column wise layout not correct.");
	}
	buffer2d_destroy(&pBuffer);
}

void test_buffer2dPutGetCol(void) {
	buffer2d_t *pBuffer = NULL;
	uint32_t jCol, i;
	float val[TEST_BUFFER2D_NROW], temp[TEST_BUFFER2D_NROW];
	float *pData;

	for (i = 0; i< TEST_BUFFER2D_NROW; i++) {
		val[i] = 7.3f * (i+1);
	}

	/* Test row wise */
	jCol = 2;
	buffer2d_create(&pBuffer, TEST_BUFFER2D_NROW, TEST_BUFFER2D_NCOL, sizeof(val[0]), BUFFER2D_LAYOUT_ROW_WISE);
	buffer2d_putDataCol(pBuffer, jCol, val);
	buffer2d_getDataCol(pBuffer, jCol, temp);

	for (i = 0; i < TEST_BUFFER2D_NROW; i++) {
		ASSERT(temp[i] == val[i], "put and/or get column data for row wise failed.");
	}

	pData = BUFFER2D_getBufferAsType(pBuffer, float);
	pData += jCol;
	for (i = 0; i < TEST_BUFFER2D_NROW; i++) {
		ASSERT(pData[i * TEST_BUFFER2D_NCOL] == val[i], "Column data for row wise layout not correct.");
	}
	buffer2d_destroy(&pBuffer);

	/* Test column wise */
	jCol = 2;
	buffer2d_create(&pBuffer, TEST_BUFFER2D_NROW, TEST_BUFFER2D_NCOL, sizeof(val[0]), BUFFER2D_LAYOUT_COLUMN_WISE);
	buffer2d_putDataCol(pBuffer, jCol, val);
	buffer2d_getDataCol(pBuffer, jCol, temp);

	for (i = 0; i < TEST_BUFFER2D_NROW; i++) {
		ASSERT(temp[i] == val[i], "put and/or get column data for column wise failed.");
	}

	pData = BUFFER2D_getBufferAsType(pBuffer, float);
	pData += jCol * TEST_BUFFER2D_NROW;
	for (i = 0; i < TEST_BUFFER2D_NROW; i++) {
		ASSERT(pData[i] == val[i], "Column data for row wise layout not correct.");
	}
	buffer2d_destroy(&pBuffer);
}

void test_buffer2dFill(void) {
	buffer2d_t *pBuffer = NULL;
	uint32_t i, count;
	float val;
	float *pData;

	val = 123.45f;
	buffer2d_create(&pBuffer, TEST_BUFFER2D_NROW, TEST_BUFFER2D_NCOL, sizeof(val), BUFFER2D_LAYOUT_ROW_WISE);
	buffer2d_fill(pBuffer, &val);
	pData = (float*) buffer2d_getBuffer(pBuffer);

	count = buffer2d_getNumRow(pBuffer) * buffer2d_getNumCol(pBuffer);
	for (i = 0; i < count; i++) {
		ASSERT(pData[i] == val, "Incorrect fill().");
	}
	buffer2d_destroy(&pBuffer);

}

void test_bufferAll(void) {
	test_buffer2dCreateDestroy();
	test_buffer2dPutGetSingle();
	test_buffer2dPutGetRow();
	test_buffer2dPutGetCol();
	test_buffer2dFill();
}
