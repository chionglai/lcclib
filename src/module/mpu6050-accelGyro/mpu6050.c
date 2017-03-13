/*
 * mpu6050.c
 *
 *  Created on: 22/06/2013
 *      Author: lcc
 */

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "hw/i2c.h"
#include "util/status.h"
#include "module/mpu6050-accelGyro/mpu6050.h"

struct mpu6050_s {
	/* I2C instance to be used */
	i2c_t i2c;
};

int32_t mpu6050_create(mpu6050_t **ppMpu6050, const mpu6050Cfg_t *cfg) {
	uint8_t value;
	mpu6050_t *mpu6050;

	mpu6050 = (mpu6050_t*) malloc(sizeof(mpu6050_t));
	if (NULL == mpu6050) {
		return STATUS_ERROR_MALLOC;
	}
	*ppMpu6050 = mpu6050;
	mpu6050->i2c.i2cIdx = cfg->i2cIdx;
	mpu6050->i2c.slaveAddr = MPU6050_I2C_ADDR;

	// reset MPU6050
	mpu6050_setMode(mpu6050, MPU6050_RESET, 0);
	do {
		i2c_readRegN(&mpu6050->i2c, MPU6050_REG_PWR_MGMT1, &value, 1);
	} while (value & MPU6050_RESET);

	i2c_updateReg(
		&mpu6050->i2c,
		MPU6050_REG_I2C_MST_DELAY_CTRL,
		MPU6050_DELAY_ES_SHADOW,
		MPU6050_DELAY_ES_SHADOW);

	// set i2c master clock to use internal 8MHz
	i2c_updateReg(
		&mpu6050->i2c,
		MPU6050_REG_PWR_MGMT1,
		MPU6050_MASK_CLK_SEL,
		MPU6050_CLKSEL_INT_8MHZ);
	i2c_updateReg(
		&mpu6050->i2c,
		MPU6050_REG_I2C_MST_CTRL,
		MPU6050_MULT_MST_EN|MPU6050_WAIT_FOR_ES|MPU6050_I2C_MST_P_NSR|MPU6050_MASK_I2C_MST_CLK,
		MPU6050_MULT_MST_EN|		//< Allow multiple master to control over primary i2c
		MPU6050_WAIT_FOR_ES|		//< Allow sync-ing external sensor data
		MPU6050_I2C_MST_P_NSR|		//< Set I2C_MST_P_NSR to allow multiple i2c slave
		MPU6050_I2C_MST_CLK_400);	//< set i2c master clock to be 400kHz

	i2c_updateReg(
		&mpu6050->i2c,
		MPU6050_REG_CONFIG,
		MPU6050_MASK_DLPF_CFG,
		MPU6050_DLPF_CFG6); // enable LP filter
	mpu6050_accel_setHpf(mpu6050, MPU6050_HPF_5);	// set high pass filter for motion detection

	mpu6050_setMode(mpu6050, MPU6050_NORMAL, 0);

	// set to default full scale
	i2c_updateReg(&mpu6050->i2c, MPU6050_REG_ACCEL_CONFIG, MPU6050_MASK_FS_SEL, MPU6050_AFS_SEL_8);
	i2c_updateReg(&mpu6050->i2c, MPU6050_REG_GYRO_CONFIG, MPU6050_MASK_FS_SEL, MPU6050_GFS_SEL_1000);

	return STATUS_OK;
}


int32_t mpu6050_destroy(mpu6050_t **ppMpu6050) {
	if (*ppMpu6050) {
		free(*ppMpu6050);
		*ppMpu6050 = NULL;
	}

	return STATUS_OK;
}


int32_t mpu6050_setMode(mpu6050_t *mpu6050, mpu6050Mode mode, uint8_t wakeUpFreq) {
	int32_t status;

	if (mode == MPU6050_CYCLE) {
		if ((status = i2c_updateReg(
				&mpu6050->i2c,
				MPU6050_REG_PWR_MGMT1,
				MPU6050_RESET|MPU6050_SLEEP|MPU6050_CYCLE,
				MPU6050_CYCLE)) != STATUS_OK)
			return status;

		return i2c_updateReg(
				&mpu6050->i2c,
				MPU6050_REG_PWR_MGMT2,
				MPU6050_MASK_LP_WAKE_CTRL,
				wakeUpFreq);
	} else {
		/* For SLEEP, RESET, NORMAL. */
		return i2c_updateReg(
				&mpu6050->i2c,
				MPU6050_REG_PWR_MGMT1,
				MPU6050_RESET|MPU6050_SLEEP|MPU6050_CYCLE,
				mode);
	}
}


int32_t mpu6050_setFreeFall(mpu6050_t *mpu6050, uint8_t thres, uint8_t count, mpu6050DecRate decRate) {
	uint8_t buffer[2];
	int32_t status;

	buffer[0] = thres;
	buffer[1] = count;

	if ((status = i2c_writeRegN(&mpu6050->i2c, MPU6050_REG_FF_THR, buffer, 2)) != STATUS_OK)
		return status;

	return i2c_updateReg(
			&mpu6050->i2c,
			MPU6050_REG_MOT_DETECT_CTRL,
			MPU6050_MASK_FF_COUNT,
			(decRate << 2));
}


int32_t mpu6050_setMotion(mpu6050_t *mpu6050, uint8_t thres, uint8_t count, mpu6050DecRate decRate) {
	uint8_t buffer[2];
	int32_t status;

	buffer[0] = thres;
	buffer[1] = count;

	if ((status = i2c_writeRegN(&mpu6050->i2c, MPU6050_REG_MOT_THR, buffer, 2)) != STATUS_OK)
		return status;

	return i2c_updateReg(
			&mpu6050->i2c,
			MPU6050_REG_MOT_DETECT_CTRL,
			MPU6050_MASK_MOT_COUNT,
			decRate);
}


int32_t mpu6050_setZeroMotion(mpu6050_t *mpu6050, uint8_t thres, uint8_t count) {
	uint8_t buffer[2];

	buffer[0] = thres;
	buffer[1] = count;

	return i2c_writeRegN(&mpu6050->i2c, MPU6050_REG_MOT_THR, buffer, 2);
}


int32_t mpu6050_i2c_setClockDivider(mpu6050_t *mpu6050, mpu6050I2cMstClk div) {
	return i2c_updateReg(&mpu6050->i2c, MPU6050_REG_I2C_MST_CTRL, MPU6050_MASK_I2C_MST_CLK, div);
}


int32_t mpu6050_i2c_setMode(mpu6050_t *mpu6050, mpu6050I2CMode mode) {
	uint8_t value;
	int32_t status;

	switch (mode) {
	case MPU6050_I2C_DISABLE:
		if ((status = i2c_updateReg(
						&mpu6050->i2c,
						MPU6050_REG_INT_PIN_CFG,
						MPU6050_I2C_BYPASS_EN,
						0x00)) != STATUS_OK)
			return status;

		return i2c_updateReg(
				&mpu6050->i2c,
				MPU6050_REG_USER_CTRL,
				MPU6050_I2C_MST_EN,
				0x00);
		break;

	case MPU6050_I2C_NORMAL:
		if ((status = i2c_updateReg(
						&mpu6050->i2c,
						MPU6050_REG_INT_PIN_CFG,
						MPU6050_I2C_BYPASS_EN,
						0x00)) != STATUS_OK)
			return status;

		return i2c_updateReg(
				&mpu6050->i2c,
				MPU6050_REG_USER_CTRL,
				MPU6050_I2C_MST_EN,
				MPU6050_I2C_MST_EN);
		break;

	case MPU6050_I2C_BYPASS:
		if ((status = i2c_updateReg(
						&mpu6050->i2c,
						MPU6050_REG_INT_PIN_CFG,
						MPU6050_I2C_BYPASS_EN,
						MPU6050_I2C_BYPASS_EN)) != STATUS_OK)
			return status;

		return i2c_updateReg(
				&mpu6050->i2c,
				MPU6050_REG_USER_CTRL,
				MPU6050_I2C_MST_EN,
				0x00);
		break;

	case MPU6050_I2C_RESET:
		// read current REG_USER_REG
		if ((status = i2c_readRegN(
						&mpu6050->i2c,
						MPU6050_REG_USER_CTRL,
						&value,
						1)) != STATUS_OK)
			return status;

		// need to disable I2C master when resetting it
		if ((status = i2c_updateReg(
						&mpu6050->i2c,
						MPU6050_REG_USER_CTRL,
						MPU6050_I2C_MST_EN|MPU6050_I2C_MST_RESET,
						MPU6050_I2C_MST_RESET)) != STATUS_OK)
			return status;

		// restore I2C_MST_EN value and return
		return i2c_updateReg(
				&mpu6050->i2c,
				MPU6050_REG_USER_CTRL,
				MPU6050_I2C_MST_EN,
				value);
		break;

	default:
		return 0;
		break;
	}

	return STATUS_OK;
}


int32_t mpu6050_i2c_initSlave(mpu6050_t *mpu6050, mpu6050I2cSlave slave, uint8_t i2cAddr, uint8_t slaveReg) {
	uint8_t buffer[2];
	int32_t i, step;
	int32_t status;

	if (slave == MPU6050_I2C_SLV_ALL) {
		i = 0;		// starting slave index 0
		step = 1;	// step by 1 on each loop
	} else {
		i = slave;	// starting slave index as specified
		step = MPU6050_I2C_SLV_ALL;		// only loop for once
	}

	buffer[0] = i2cAddr & MPU6050_MASK_I2C_SLV_ADDR;
	buffer[1] = slaveReg;

	while (i < MPU6050_I2C_SLV_ALL) {
		if ((status = i2c_writeRegN(
						&mpu6050->i2c,
						MPU6050_REG_I2C_SLV_ADDR(i),
						buffer,
						2)) != STATUS_OK)
			return status;

		i += step;
	}

	return STATUS_OK;
}


int32_t mpu6050_i2c_setReadFromSlave(mpu6050_t *mpu6050, mpu6050I2cSlave slave, uint32_t len) {
	int32_t i, step;
	int32_t status;

	if (slave == MPU6050_I2C_SLV_ALL) {
		i = 0;		// starting slave index 0
		step = 1;	// step by 1 on each loop
	} else {
		i = slave;	// starting slave index as specified
		step = MPU6050_I2C_SLV_ALL;		// only loop for once
	}

	while (i < MPU6050_I2C_SLV_ALL) {
		if ((status = i2c_updateReg(
						&mpu6050->i2c,
						MPU6050_REG_I2C_SLV_ADDR(i),
						MPU6050_I2C_SLV_RW,
						MPU6050_I2C_SLV_RW)) != STATUS_OK)
			return status;

		if ((status = i2c_updateReg(
						&mpu6050->i2c,
						MPU6050_REG_I2C_SLV_CTRL(i),
						MPU6050_MASK_I2C_SLV_LEN,
						len)) != STATUS_OK)
			return status;

		i += step;
	}

	return STATUS_OK;
}


int32_t mpu6050_i2c_setWriteToSlave(mpu6050_t *mpu6050, mpu6050I2cSlave slave, uint8_t data) {
	int32_t i, step;
	int32_t status;

	if (slave == MPU6050_I2C_SLV_ALL) {
		i = 0;		// starting slave index 0
		step = 1;	// step by 1 on each loop
	} else {
		i = slave;	// starting slave index as specified
		step = MPU6050_I2C_SLV_ALL;		// only loop for once
	}

	while (i < MPU6050_I2C_SLV_ALL) {
		if ((status = i2c_updateReg(
						&mpu6050->i2c,
						MPU6050_REG_I2C_SLV_ADDR(i),
						MPU6050_I2C_SLV_RW,
						0x00)) != STATUS_OK)
			return status;

		if ((status = i2c_updateReg(
						&mpu6050->i2c,
						MPU6050_REG_I2C_SLV_CTRL(i),
						MPU6050_MASK_I2C_SLV_LEN,
						0x01)) != STATUS_OK)
			return status;

		if ((status = i2c_writeRegN(
						&mpu6050->i2c,
						MPU6050_REG_I2C_SLV_DO(i),
						&data,
						1)) != STATUS_OK)
			return status;

		i += step;
	}
	return 1;
}


int32_t mpu6050_i2c_enableSlave(mpu6050_t *mpu6050, mpu6050I2cSlave slave) {
	int32_t i, step;
	int32_t status;

	if (slave == MPU6050_I2C_SLV_ALL) {
		i = 0;		// starting slave index 0
		step = 1;	// step by 1 on each loop
	} else {
		i = slave;	// starting slave index as specified
		step = MPU6050_I2C_SLV_ALL;		// only loop for once
	}

	while (i < MPU6050_I2C_SLV_ALL) {
		if ((status = i2c_updateReg(
						&mpu6050->i2c,
						MPU6050_REG_I2C_SLV_CTRL(i),
						MPU6050_I2C_SLV_EN,
						MPU6050_I2C_SLV_EN)) != STATUS_OK)
			return status;
		i += step;
	}

	return STATUS_OK;
}


int32_t mpu6050_i2c_disableSlave(mpu6050_t *mpu6050, mpu6050I2cSlave slave) {
	int32_t i, step;
	int32_t status;

	if (slave == MPU6050_I2C_SLV_ALL) {
		i = 0;		// starting slave index 0
		step = 1;	// step by 1 on each loop
	} else {
		i = slave;	// starting slave index as specified
		step = MPU6050_I2C_SLV_ALL;		// only loop for once
	}

	while (i < MPU6050_I2C_SLV_ALL) {
		if ((status = i2c_updateReg(
						&mpu6050->i2c,
						MPU6050_REG_I2C_SLV_CTRL(i),
						MPU6050_I2C_SLV_EN,
						0x00)) != STATUS_OK)
			return status;
		i += step;
	}

	return STATUS_OK;
}


int32_t mpu6050_calibrate(mpu6050_t *mpu6050) {
	uint8_t buffer[14];
	uint8_t backupValue[4];
	float avg[7];
	int32_t i, j;
	uint16_t tempShort;

	memset(avg, 0, sizeof(avg));

	// 0. Read current register values to be restored at end of self-test
	i2c_readRegN(&mpu6050->i2c, MPU6050_REG_SMPLRT_DIV, backupValue, 4);

	// disable all accelerometer, gyroscope and temp sensor
	mpu6050_accel_disable(mpu6050, MPU6050_AG_XYZ);
	mpu6050_gyro_disable(mpu6050, MPU6050_AG_XYZ);
	mpu6050_temp_disable(mpu6050);

	// set accel and gyro full scale according to recommended full scale for self test
	i2c_updateReg(&mpu6050->i2c, MPU6050_REG_ACCEL_CONFIG, MPU6050_MASK_FS_SEL, MPU6050_AFS_SEL_8);
	i2c_updateReg(&mpu6050->i2c, MPU6050_REG_GYRO_CONFIG, MPU6050_MASK_FS_SEL, MPU6050_GFS_SEL_250);
	// Set sample rate divider to 0
	i2c_updateReg(&mpu6050->i2c, MPU6050_REG_SMPLRT_DIV, 0xFF, 0x00);
	// Deactivate low pass filter
	i2c_updateReg(&mpu6050->i2c, MPU6050_REG_CONFIG, MPU6050_MASK_DLPF_CFG, MPU6050_DLPF_CFG0);

	// use DATA_RDY interrupt to signify data is ready to be read. Enable DATA_RDY
	// interrupt and read from its status register to clear the interrupt flag
	i2c_updateReg(&mpu6050->i2c, MPU6050_REG_INT_ENABLE, MPU6050_INT_DATA_RDY, MPU6050_INT_DATA_RDY);
	i2c_readRegN(&mpu6050->i2c, MPU6050_REG_INT_STATUS, buffer, 1);

	// enable all accelerometer, gyroscope and temp
	mpu6050_accel_enable(mpu6050, MPU6050_AG_XYZ);
	mpu6050_gyro_enable(mpu6050, MPU6050_AG_XYZ);
	mpu6050_temp_enable(mpu6050);

	// using DATA_RDY interrupt to signify the data is ready, read and average
	for (i = 0; i <= SELF_TEST_AVG_COUNT; i++) {
		do {
			i2c_readRegN(&mpu6050->i2c, MPU6050_REG_INT_STATUS, buffer, 1);
		} while ((buffer[0] & MPU6050_INT_DATA_RDY) == 0);

		i2c_readRegN(&mpu6050->i2c, MPU6050_REG_ACCEL_XOUT_H, buffer, 14);
		for (j = 0; j < 7; j++) {
			tempShort = uint8ToUint16(buffer, 2*j);
			avg[j] += tempShort;
		}
	}

	// disable all accelerometer, gyroscope and temp sensor
	mpu6050_accel_disable(mpu6050, MPU6050_AG_XYZ);
	mpu6050_gyro_disable(mpu6050, MPU6050_AG_XYZ);
	mpu6050_temp_disable(mpu6050);

	// disable DATA_RDY interrupt
	i2c_updateReg(&mpu6050->i2c, MPU6050_REG_INT_ENABLE, MPU6050_INT_DATA_RDY, 0);

	// putting averaged data into buffer
	for (i = 0, j = 0; j < 7; j++) {
		tempShort = (uint16_t)(round(avg[j]/(SELF_TEST_AVG_COUNT+1)));

		buffer[i] = (tempShort >> 8) & 0xFF;
		buffer[i+1] = tempShort & 0xFF;
	}

	// write offset to offset registers for accelerometer
	i2c_writeRegN(&mpu6050->i2c, MPU6050_REG_XA_OFFS_H, buffer, 6);
	// write to offset register for gyro
	i2c_writeRegN(&mpu6050->i2c, MPU6050_REG_XG_OFFS_H, &buffer[8], 6);

	// Note that averaged temperature reading in buffer[7:6] are ignored

	// restore register values
	i2c_writeRegN(&mpu6050->i2c, MPU6050_REG_SMPLRT_DIV, backupValue, 4);

	return STATUS_OK;
}


int32_t mpu6050_setSampleRate(mpu6050_t *mpu6050, uint32_t sr) {
	uint8_t buffer;
	uint32_t maxSR;
	int32_t divider;
	int32_t status;

	if ((status = i2c_readRegN(
					&mpu6050->i2c,
					MPU6050_REG_CONFIG,
					&buffer,
					1)) != STATUS_OK)
		return status;

	buffer &= MPU6050_MASK_DLPF_CFG;
	if (buffer == 0 || buffer == 7) {
		maxSR = 8000;
	} else {
		maxSR = 1000;
	}

	if (sr < (maxSR >> 8) || sr > maxSR) {
		return STATUS_ERROR;
	}
	divider = (int32_t)round(maxSR/sr) - 1;

	if (divider < 0)
		buffer = 0;
	else if (divider > 255)
		buffer = 255;
	else
		buffer = (uint8_t)divider;

	return i2c_writeRegN(&mpu6050->i2c, MPU6050_REG_SMPLRT_DIV, &buffer, 1);
}


int32_t mpu6050_temp_enable(mpu6050_t *mpu6050) {
	return i2c_updateReg(&mpu6050->i2c, MPU6050_REG_PWR_MGMT1, MPU6050_TEMP_DIS, 0x00);
}


int32_t mpu6050_temp_disable(mpu6050_t *mpu6050) {
	return i2c_updateReg(&mpu6050->i2c, MPU6050_REG_PWR_MGMT1, MPU6050_TEMP_DIS, MPU6050_TEMP_DIS);
}


int32_t mpu6050_temp_reset (mpu6050_t *mpu6050) {
	return i2c_updateReg(&mpu6050->i2c, MPU6050_REG_SIGNAL_PATH_RESET, MPU6050_TEMP_RESET, MPU6050_TEMP_RESET);
}


int32_t mpu6050_temp_readRaw(mpu6050_t *mpu6050, int16_t* temp) {
	uint8_t buffer[2];
	int16_t value;
	int32_t status;

	// read from sensor register
	if ((status = i2c_readRegN(
					&mpu6050->i2c,
					MPU6050_REG_TEMP_OUT_H,
					buffer,
					2)) != STATUS_OK)
		return 0;

	value = uint8ToUint16(buffer, 0);
	*temp = value;

	return STATUS_OK;
}


int32_t mpu6050_temp_read(mpu6050_t *mpu6050, float* temp) {
	int16_t value;
	int32_t status;

	value = 0;
	if ((status = mpu6050_temp_readRaw(mpu6050, &value)) != STATUS_OK)
		return status;

	*temp = value/340.0 + 36.53;

	return STATUS_OK;
}


mpu6050SelfTestResult mpu6050_accel_selfTest(mpu6050_t *mpu6050) {
	uint8_t buffer[6];
	uint8_t srDiv, config, fs;
	int16_t x, y, z;
	float xFt, yFt, zFt, changeFt;
	float xAvgStr, yAvgStr, zAvgStr;	//< Accel self test response
	int32_t i;
	int32_t tempInt;
	mpu6050SelfTestResult result;

	x = y = z = 0;

	// 0. Read current register values to be restored at end of self-test
	i2c_readRegN(&mpu6050->i2c, MPU6050_REG_SMPLRT_DIV, &srDiv, 1);
	i2c_readRegN(&mpu6050->i2c, MPU6050_REG_CONFIG, &config, 1);
	config &= MPU6050_MASK_EXT_SYNC_SET | MPU6050_MASK_DLPF_CFG;
	i2c_readRegN(&mpu6050->i2c, MPU6050_REG_ACCEL_CONFIG, &fs, 1);
	fs &= MPU6050_MASK_FS_SEL;

	// Implement accelerometer self test refer section 4.1 page 9 of Register Map and Description document
	// 1. Configure MPU6050 module for self-test
	// Set the accelerometer full scale as recommended
	i2c_updateReg(&mpu6050->i2c, MPU6050_REG_ACCEL_CONFIG, MPU6050_MASK_FS_SEL, MPU6050_AFS_SEL_8);
	// Set sample rate divider to 0
	i2c_updateReg(&mpu6050->i2c, MPU6050_REG_SMPLRT_DIV, 0xFF, 0x00);
	// Deactivate low pass filter
	i2c_updateReg(&mpu6050->i2c, MPU6050_REG_CONFIG, MPU6050_MASK_DLPF_CFG, MPU6050_DLPF_CFG0);

	// Disable accelerometer before test
	mpu6050_accel_disable(mpu6050, MPU6050_AG_XYZ);

	// use DATA_RDY interrupt to signify data is ready to be read. Enable DATA_RDY
	// interrupt and read from its status register to clear the interrupt flag
	i2c_updateReg(&mpu6050->i2c, MPU6050_REG_INT_ENABLE, MPU6050_INT_DATA_RDY, MPU6050_INT_DATA_RDY);
	i2c_readRegN(&mpu6050->i2c, MPU6050_REG_INT_STATUS, buffer, 1);

	// 2. Read accel output with self test disabled
	// Disable self test
	i2c_updateReg(&mpu6050->i2c, MPU6050_REG_ACCEL_CONFIG, MPU6050_XYZ_ST, 0);
	// Enable accelerometer
	mpu6050_accel_enable(mpu6050, MPU6050_AG_XYZ);

	xAvgStr = yAvgStr = zAvgStr = 0;
	for (i = 0; i < SELF_TEST_AVG_COUNT; i++) {
		do {
			i2c_readRegN(&mpu6050->i2c, MPU6050_REG_INT_STATUS, buffer, 1);
		} while((buffer[0] & MPU6050_INT_DATA_RDY) == 0);

		mpu6050_accel_readRaw(mpu6050, &x, &y, &z);

		xAvgStr -= x;
		yAvgStr -= y;
		zAvgStr -= z;
	}

	// 3. Enable self test. R28, ACCEL_CONFIG
	i2c_updateReg(&mpu6050->i2c, MPU6050_REG_ACCEL_CONFIG, MPU6050_XYZ_ST, MPU6050_XYZ_ST);

	// 4. Read accel output with self test enabled
	for (i = 0; i < SELF_TEST_AVG_COUNT; i++) {
		do {
			i2c_readRegN(&mpu6050->i2c, MPU6050_REG_INT_STATUS, buffer, 1);
		} while((buffer[0] & MPU6050_INT_DATA_RDY) == 0);

		mpu6050_accel_readRaw(mpu6050, &x, &y, &z);

		xAvgStr += x;
		yAvgStr += y;
		zAvgStr += z;
	}

	// Disable accelerometer after test
	mpu6050_accel_disable(mpu6050, MPU6050_AG_XYZ);

	xAvgStr /= SELF_TEST_AVG_COUNT;
	yAvgStr /= SELF_TEST_AVG_COUNT;
	zAvgStr /= SELF_TEST_AVG_COUNT;

	// 5. Calculate factory trim from XA_TEST, YA_TEST and ZA_TEST
	i2c_readRegN(&mpu6050->i2c, MPU6050_REG_SELF_TEST_X, buffer, 4);
	tempInt = ((buffer[0] & 0xE0) >> 3) | ((buffer[3] >> 4) & 0x03);
	if (tempInt == 0) {
		xFt = 0;
	} else {
		xFt = 4096*0.34*pow(0.92/0.34, (tempInt - 1.0)/30.0);
	}
	tempInt = ((buffer[1] & 0xE0) >> 3) | ((buffer[3] >> 2) & 0x03);
	if (tempInt == 0) {
		yFt = 0;
	} else {
		yFt = 4096*0.34*pow(0.92/0.34, (tempInt - 1.0)/30.0);
	}
	tempInt = ((buffer[2] & 0xE0) >> 3) | (buffer[3] & 0x03);
	if (tempInt == 0) {
		zFt = 0;
	} else {
		zFt = 4096*0.34*pow(0.92/0.34, (tempInt - 1.0)/30.0);
	}

	// 6. Calculate change from factory trim and check if within limits
	result = MPU6050_ST_FAIL;
	changeFt = (xAvgStr - xFt)/xFt;
	if (SELF_TEST_FT_MIN <= changeFt && changeFt <= SELF_TEST_FT_MAX)
		result |= MPU6050_ST_PASS_X;

	changeFt = (yAvgStr - yFt)/yFt;
	if (SELF_TEST_FT_MIN <= changeFt && changeFt <= SELF_TEST_FT_MAX)
		result |= MPU6050_ST_PASS_Y;

	changeFt = (zAvgStr - zFt)/zFt;
	if (SELF_TEST_FT_MIN <= changeFt && changeFt <= SELF_TEST_FT_MAX)
		result |= MPU6050_ST_PASS_Z;

	// 5. Disable self test, and restore register value.
	i2c_updateReg(&mpu6050->i2c, MPU6050_REG_ACCEL_CONFIG, MPU6050_XYZ_ST|MPU6050_MASK_FS_SEL, fs);
	i2c_writeRegN(&mpu6050->i2c, MPU6050_REG_SMPLRT_DIV, &srDiv, 1);
	i2c_writeRegN(&mpu6050->i2c, MPU6050_REG_CONFIG, &config, 1);

	return result;
}


int32_t mpu6050_accel_enable(mpu6050_t *mpu6050, mpu6050AccelGyroXYZ mask) {
	uint8_t temp;

	temp = (mask & MPU6050_AG_XYZ) << 3;

	return i2c_updateReg(&mpu6050->i2c, MPU6050_REG_PWR_MGMT2, temp, 0x00);
}


int32_t mpu6050_accel_disable(mpu6050_t *mpu6050, mpu6050AccelGyroXYZ mask) {
	uint8_t temp;

	temp = (mask & MPU6050_AG_XYZ) << 3;

	return i2c_updateReg(&mpu6050->i2c, MPU6050_REG_PWR_MGMT2, temp, temp);
}


int32_t mpu6050_accel_reset (mpu6050_t *mpu6050) {
	return i2c_updateReg(
			&mpu6050->i2c,
			MPU6050_REG_SIGNAL_PATH_RESET,
			MPU6050_ACCEL_RESET,
			MPU6050_ACCEL_RESET);
}


int32_t mpu6050_accel_setHpf(mpu6050_t *mpu6050, mpu6050AccelHighPassFilter mode) {
	return i2c_updateReg(
			&mpu6050->i2c,
			MPU6050_REG_ACCEL_CONFIG,
			MPU6050_MASK_ACCEL_HPF,
			mode);
}


int32_t mpu6050_accel_readRaw(mpu6050_t *mpu6050, int16_t* xa, int16_t* ya, int16_t* za) {
	uint8_t buffer[6];
	int32_t status;

	// read from sensor register
	if ((status = i2c_readRegN(
					&mpu6050->i2c,
					MPU6050_REG_ACCEL_XOUT_H,
					buffer,
					6)) != STATUS_OK)
		return status;

	*xa = uint8ToUint16(buffer, 0);
	*ya = uint8ToUint16(buffer, 2);
	*za = uint8ToUint16(buffer, 4);

	return STATUS_OK;
}


int32_t mpu6050_accel_read(mpu6050_t *mpu6050, float* xa, float* ya, float* za) {
	uint8_t fullScale;
	int16_t rawX, rawY, rawZ;
	float factor;
	int32_t status;

	if ((status = i2c_readRegN(
					&mpu6050->i2c,
					MPU6050_REG_ACCEL_CONFIG,
					&fullScale,
					1) ) != STATUS_OK)
		return status;

	fullScale &= MPU6050_MASK_FS_SEL;
	switch(fullScale) {
	case MPU6050_AFS_SEL_2:
		factor = 16384.0;
		break;
	case MPU6050_AFS_SEL_4:
		factor = 8192.0;
		break;
	case MPU6050_AFS_SEL_8:
		factor = 4096.0;
		break;
	case MPU6050_AFS_SEL_16:
		factor = 2048.0;
		break;
	default:
		return STATUS_ERROR;
		break;
	}

	if ((status = mpu6050_accel_readRaw(mpu6050, &rawX, &rawY, &rawZ)) != STATUS_OK)
		return status;

	*xa = rawX/factor;
	*ya = rawY/factor;
	*za = rawZ/factor;

	return STATUS_OK;
}


mpu6050SelfTestResult mpu6050_gyro_selfTest(mpu6050_t *mpu6050) {
	uint8_t buffer[6];
	uint8_t srDiv, config, fs;
	int16_t x, y, z;
	float xFt, yFt, zFt, changeFt;
	float xAvgStr, yAvgStr, zAvgStr;	//< Gyro self test response
	int32_t i;
	int32_t tempInt;
	mpu6050SelfTestResult result;

	x = y = z = 0;

	// 0. Read current register values to be restored at end of self-test
	i2c_readRegN(&mpu6050->i2c, MPU6050_REG_SMPLRT_DIV, &srDiv, 1);
	i2c_readRegN(&mpu6050->i2c, MPU6050_REG_CONFIG, &config, 1);
	config &= MPU6050_MASK_EXT_SYNC_SET | MPU6050_MASK_DLPF_CFG;
	i2c_readRegN(&mpu6050->i2c, MPU6050_REG_GYRO_CONFIG, &fs, 1);
	fs &= MPU6050_MASK_FS_SEL;

	// Implement gyroscope self test refer section 4.1 page 9 of Register Map and Description document
	// 1. Configure MPU6050 module for self-test
	// Set the gyroscope full scale as recommended
	i2c_updateReg(&mpu6050->i2c, MPU6050_REG_GYRO_CONFIG, MPU6050_MASK_FS_SEL, MPU6050_GFS_SEL_250);
	// Set sample rate divider to 0
	i2c_updateReg(&mpu6050->i2c, MPU6050_REG_SMPLRT_DIV, 0xFF, 0x00);
	// Deactivate low pass filter
	i2c_updateReg(&mpu6050->i2c, MPU6050_REG_CONFIG, MPU6050_MASK_DLPF_CFG, MPU6050_DLPF_CFG0);

	// Disable gyro before test
	mpu6050_gyro_disable(mpu6050, MPU6050_AG_XYZ);

	// use DATA_RDY interrupt to signify data is ready to be read. Enable DATA_RDY
	// interrupt and read from its status register to clear the interrupt flag
	i2c_updateReg(&mpu6050->i2c, MPU6050_REG_INT_ENABLE, MPU6050_INT_DATA_RDY, MPU6050_INT_DATA_RDY);
	i2c_readRegN(&mpu6050->i2c, MPU6050_REG_INT_STATUS, buffer, 1);

	// 2. Read gyro output with self test disabled
	// Disable self test
	i2c_updateReg(&mpu6050->i2c, MPU6050_REG_ACCEL_CONFIG, MPU6050_XYZ_ST, 0);
	// Enable accelerometer
	mpu6050_gyro_enable(mpu6050, MPU6050_AG_XYZ);

	xAvgStr = yAvgStr = zAvgStr = 0;
	for (i = 0; i < SELF_TEST_AVG_COUNT; i++) {
		do {
			i2c_readRegN(&mpu6050->i2c, MPU6050_REG_INT_STATUS, buffer, 1);
		} while((buffer[0] & MPU6050_INT_DATA_RDY) == 0);

		mpu6050_gyro_readRaw(mpu6050, &x, &y, &z);

		xAvgStr -= x;
		yAvgStr -= y;
		zAvgStr -= z;
	}

	// 3. Enable self test. R27, GYRO_CONFIG
	i2c_updateReg(&mpu6050->i2c, MPU6050_REG_GYRO_CONFIG, MPU6050_XYZ_ST, MPU6050_XYZ_ST);

	// 4. Read gyro output with self test enabled
	for (i = 0; i < SELF_TEST_AVG_COUNT; i++) {
		do {
			i2c_readRegN(&mpu6050->i2c, MPU6050_REG_INT_STATUS, buffer, 1);
		} while((buffer[0] & MPU6050_INT_DATA_RDY) == 0);

		mpu6050_gyro_readRaw(mpu6050, &x, &y, &z);

		xAvgStr += x;
		yAvgStr += y;
		zAvgStr += z;
	}

	// Disable accelerometer after test
	mpu6050_gyro_disable(mpu6050, MPU6050_AG_XYZ);

	xAvgStr /= SELF_TEST_AVG_COUNT;
	yAvgStr /= SELF_TEST_AVG_COUNT;
	zAvgStr /= SELF_TEST_AVG_COUNT;

	// 5. Calculate factory trim from XG_TEST, YG_TEST and ZG_TEST
	i2c_readRegN(&mpu6050->i2c, MPU6050_REG_SELF_TEST_X, buffer, 3);
	tempInt = buffer[0] & 0x1F;
	if (tempInt == 0) {
		xFt = 0;
	} else {
		xFt = 25.0*131*pow(1.046, tempInt - 1.0);
	}
	tempInt = buffer[1] & 0x1F;
	if (tempInt == 0) {
		yFt = 0;
	} else {
		yFt = -25.0*131*pow(1.046, tempInt - 1.0);
	}
	tempInt = buffer[2] & 0x1F;
	if (tempInt == 0) {
		zFt = 0;
	} else {
		zFt = 25.0*131*pow(1.046, tempInt - 1.0);
	}

	// 6. Calculate change from factory trim and check if within limits
	result = MPU6050_ST_FAIL;
	changeFt = (xAvgStr - xFt)/xFt;
	if (SELF_TEST_FT_MIN <= changeFt && changeFt <= SELF_TEST_FT_MAX)
		result |= MPU6050_ST_PASS_X;

	changeFt = (yAvgStr - yFt)/yFt;
	if (SELF_TEST_FT_MIN <= changeFt && changeFt <= SELF_TEST_FT_MAX)
		result |= MPU6050_ST_PASS_Y;

	changeFt = (zAvgStr - zFt)/zFt;
	if (SELF_TEST_FT_MIN <= changeFt && changeFt <= SELF_TEST_FT_MAX)
		result |= MPU6050_ST_PASS_Z;

	// 5. Disable self test, and restore register value.
	i2c_updateReg(&mpu6050->i2c, MPU6050_REG_GYRO_CONFIG, MPU6050_XYZ_ST|MPU6050_MASK_FS_SEL, fs);
	i2c_writeRegN(&mpu6050->i2c, MPU6050_REG_SMPLRT_DIV, &srDiv, 1);
	i2c_writeRegN(&mpu6050->i2c, MPU6050_REG_CONFIG, &config, 1);

	return result;
}


int32_t mpu6050_gyro_enable(mpu6050_t *mpu6050, mpu6050AccelGyroXYZ mask) {
	return i2c_updateReg(&mpu6050->i2c, MPU6050_REG_PWR_MGMT2, mask, 0x00);
}


int32_t mpu6050_gyro_disable(mpu6050_t *mpu6050, mpu6050AccelGyroXYZ mask) {
	return i2c_updateReg(&mpu6050->i2c, MPU6050_REG_PWR_MGMT2, mask, mask);
}


int32_t mpu6050_gyro_reset (mpu6050_t *mpu6050) {
	return i2c_updateReg(&mpu6050->i2c, MPU6050_REG_SIGNAL_PATH_RESET, MPU6050_GYRO_RESET, MPU6050_GYRO_RESET);
}


int32_t mpu6050_gyro_readRaw(mpu6050_t *mpu6050, int16_t* xg, int16_t* yg, int16_t* zg) {
	uint8_t buffer[6];
	int32_t status;

	// read from sensor register
	if ((status = i2c_readRegN(&mpu6050->i2c, MPU6050_REG_GYRO_XOUT_H, buffer, 6)) != STATUS_OK)
		return status;

	*xg = uint8ToUint16(buffer, 0);
	*yg = uint8ToUint16(buffer, 2);
	*zg = uint8ToUint16(buffer, 4);

	return STATUS_OK;
}


int32_t mpu6050_gyro_read(mpu6050_t *mpu6050, float* xg, float* yg, float* zg) {
	uint8_t fullScale;
	int16_t rawX, rawY, rawZ;
	float factor;
	int32_t status;

	if ((status = i2c_readRegN(
					&mpu6050->i2c,
					MPU6050_REG_GYRO_CONFIG,
					&fullScale,
					1)) != STATUS_OK)
		return status;

	fullScale &= MPU6050_MASK_FS_SEL;
	switch(fullScale) {
	case MPU6050_GFS_SEL_250:
		factor = 131.0;
		break;
	case MPU6050_GFS_SEL_500:
		factor = 65.5;
		break;
	case MPU6050_GFS_SEL_1000:
		factor = 32.8;
		break;
	case MPU6050_GFS_SEL_2000:
		factor = 16.4;
		break;
	default:
		return STATUS_ERROR;
		break;
	}

	if ((status = mpu6050_gyro_readRaw(mpu6050, &rawX, &rawY, &rawZ)) != STATUS_OK)
		return status;

	*xg = rawX/factor;
	*yg = rawY/factor;
	*zg = rawZ/factor;

	return STATUS_OK;
}


int32_t mpu6050_fifo_enable(mpu6050_t *mpu6050, mpu6050FifoEnMask fifoEnMask) {
	uint8_t fifoEn;
	uint16_t value;
	uint8_t buffer[2];
	int32_t status;

	if ((fifoEn = (uint8_t)((fifoEnMask >> 8) & 0xFF)) != 0) {
		if((status = i2c_updateReg(
						&mpu6050->i2c,
						MPU6050_REG_FIFO_EN,
						fifoEn,
						fifoEn)) != STATUS_OK)
			return status;
	}

	if ((fifoEn = (uint8_t)(fifoEnMask & 0xFF)) != 0) {
		if ((status = i2c_updateReg(
						&mpu6050->i2c,
						MPU6050_REG_I2C_MST_CTRL,
						fifoEn,
						fifoEn)) != STATUS_OK)
			return status;
	}

	// disable FIFO_EN in REG_USER_CTRL and reset
	if ((status = i2c_updateReg(
					&mpu6050->i2c,
					MPU6050_REG_USER_CTRL,
					MPU6050_FIFO_EN|MPU6050_FIFO_RESET,
					MPU6050_FIFO_RESET)) != STATUS_OK)
		return status;

	// enable FIFO_EN in REG_USER_CTRL if at least one FIFO is enabled
	if ((status = i2c_readRegN(
					&mpu6050->i2c,
					MPU6050_REG_FIFO_EN,
					buffer,
					2)) != STATUS_OK)
		return status;

	value = uint8ToUint16(buffer, 0);
	if ((value & MPU6050_FIFO_ALL) != 0) {
		return i2c_updateReg(
				&mpu6050->i2c,
				MPU6050_REG_USER_CTRL,
				MPU6050_FIFO_EN,
				MPU6050_FIFO_EN);
	}

	return STATUS_OK;
}

// Same implementation as enable function. The only difference is the
// value written. Here, it will always be 0x00 to disable.
int32_t mpu6050_fifo_disable(mpu6050_t *mpu6050, mpu6050FifoEnMask fifoEnMask) {
	uint8_t fifoEn;
	uint16_t value;
	uint8_t buffer[2];
	int32_t status;

	if ((fifoEn = (uint8_t)((fifoEnMask >> 8) & 0xFF)) != 0) {
		if((status = i2c_updateReg(
						&mpu6050->i2c,
						MPU6050_REG_FIFO_EN,
						fifoEn,
						0x00)) != STATUS_OK)
			return status;
	}

	if ((fifoEn = (uint8_t)(fifoEnMask & 0xFF)) != 0) {
		if ((status = i2c_updateReg(
						&mpu6050->i2c,
						MPU6050_REG_I2C_MST_CTRL,
						fifoEn,
						0x00)) != STATUS_OK)
			return status;
	}

	// disable FIFO_EN in REG_USER_CTRL and reset FIFO. This is to flush the FIFO
	// whenever FIFO configuration has changed.
	if ((status = i2c_updateReg(
					&mpu6050->i2c,
					MPU6050_REG_USER_CTRL,
					MPU6050_FIFO_EN|MPU6050_FIFO_RESET,
					MPU6050_FIFO_RESET)) != STATUS_OK)
		return status;

	// read and check if there is still any FIFO enabled. If there is
	// re-enable FIFO_EN in REG_USER_CTRL.
	if ((status = i2c_readRegN(
					&mpu6050->i2c,
					MPU6050_REG_FIFO_EN,
					buffer,
					2)) != STATUS_OK)
		return status;

	value = uint8ToUint16(buffer, 0);
	if ((value & MPU6050_FIFO_ALL) != 0) {
		return i2c_updateReg(
				&mpu6050->i2c,
				MPU6050_REG_USER_CTRL,
				MPU6050_FIFO_EN,
				MPU6050_FIFO_EN);
	}

	return STATUS_OK;
}


int32_t mpu6050_fifo_reset(mpu6050_t *mpu6050) {
	uint8_t value;
	int32_t status;

	// read current REG_USER_REG
	if ((status = i2c_readRegN(
					&mpu6050->i2c,
					MPU6050_REG_USER_CTRL,
					&value,
					1)) != STATUS_OK)
		return status;

	// need to disable FIFO when resetting it
	if ((status = i2c_updateReg(
					&mpu6050->i2c,
					MPU6050_REG_USER_CTRL,
					MPU6050_FIFO_EN|MPU6050_FIFO_RESET,
					MPU6050_FIFO_RESET)) != STATUS_OK)
		return status;

	// restore FIFO_EN value and return
	return i2c_updateReg(&mpu6050->i2c, MPU6050_REG_USER_CTRL, MPU6050_FIFO_EN, value);
}


int32_t mpu6050_fifo_read(mpu6050_t *mpu6050, uint8_t* const data, uint32_t len) {
	uint16_t count;
	uint8_t buffer[2];
	int32_t i;
	int32_t status;

	if ((status = i2c_readRegN(
					&mpu6050->i2c,
					MPU6050_REG_FIFO_COUNT_H,
					buffer,
					2)) != STATUS_OK)
		return status;

	count = uint8ToUint16(buffer, 0);

	len = (count > len)? len : count;

	i = 0;
	do {
		status = i2c_readRegN(&mpu6050->i2c, MPU6050_REG_FIFO_DATA, &(data[i]), 1);
		i++;
	} while(i < len && status == STATUS_OK);

	return i;
}


// Note: this function has not been tested.
int32_t mpu6050_fifo_write(mpu6050_t *mpu6050, const uint8_t* const data, uint32_t len) {
	uint16_t count;
	int32_t freeSpace;
	uint8_t buffer[2];
	int32_t i;
	int32_t status;

	if ((status = i2c_readRegN(
					&mpu6050->i2c,
					MPU6050_REG_FIFO_COUNT_H,
					buffer,
					2)) != STATUS_OK)
		return status;

	count = uint8ToUint16(buffer, 0);
	freeSpace = MPU6050_FIFO_LENGTH - count;

	len = (freeSpace > len)? len : freeSpace;

	i = 0;
	do {
		status = i2c_writeRegN(&mpu6050->i2c, MPU6050_REG_FIFO_DATA, &(data[i]), 1);
		i++;
	} while (i < len && status == STATUS_OK);

	return i;
}


/**
 * Temporary fix
 */
int32_t mpu6050_updateReg(mpu6050_t *mpu6050, uint8_t regAddr, uint8_t mask, uint8_t data) {
	return i2c_updateReg(&mpu6050->i2c, regAddr, mask, data);
}

int32_t mpu6050_readRegN(mpu6050_t *mpu6050, uint8_t regStartAddr, uint8_t* const buffer, uint32_t len) {
	return i2c_readRegN(&mpu6050->i2c, regStartAddr, buffer, len);
}

int32_t mpu6050_writeRegN(mpu6050_t *mpu6050, uint8_t regStartAddr, const uint8_t* const data, uint32_t len) {
	return i2c_writeRegN(&mpu6050->i2c, regStartAddr, data, len);
}
