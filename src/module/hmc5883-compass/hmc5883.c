
#include <stdlib.h>
#include "util/status.h"
#include "hw/i2c.h"
#include "module/hmc5883-compass/hmc5883.h"

struct hmc5883_s {
	i2c_t i2c;
};

int32_t hmc5883_create(hmc5883_t **ppHmc5883, const hmc5883Cfg_t *cfg) {
	hmc5883_t *hmc5883;

	if (NULL == cfg) {
		return STATUS_ERROR;
	}

	hmc5883 = (hmc5883_t*) malloc(sizeof(hmc5883_t));
	if (NULL == hmc5883) {
		return STATUS_ERROR_MALLOC;
	}
	*ppHmc5883 = hmc5883;
	hmc5883->i2c.i2cIdx = cfg->i2cIdx;
	hmc5883->i2c.slaveAddr = HMC5883_I2C_ADDR;
	
	hmc5883_setAvgSample(hmc5883, HMC5883_MA_8);
	hmc5883_setDataRate(hmc5883, HMC5883_DOR_3);
	hmc5883_setGain(hmc5883, HMC5883_GN_1090);
	hmc5883_setMode(hmc5883, HMC5883_MD_CONT);
	
	return hmc5883_selfTest(hmc5883);
}


int32_t hcm5883_destroy(hmc5883_t **ppHmc5883) {
	if (NULL != *ppHmc5883) {
		free(*ppHmc5883);
		*ppHmc5883 = NULL;
	}

	return STATUS_OK;
}

int32_t hmc5883_selfTest(hmc5883_t *hmc5883) {
	uint8_t buffer[6];
	uint32_t nAvg = 10;	// 10 averaging sample.
	int16_t x, y, z;
	int32_t i;
	int32_t xSum, ySum, zSum;
	uint8_t prev[3];
	int32_t status;

	// get previous CRB and MR register value to restore to after self test
	if ((status = i2c_readRegN(&hmc5883->i2c, HMC5883_REG_CRA, prev, 3)) != STATUS_OK)
		return status;

	buffer[0] = HMC5883_MA_8 | HMC5883_DOR_15 | HMC5883_MS_POS_BIAS;
	buffer[1] = HMC5883_GN_390;
	buffer[2] = HMC5883_MD_CONT;

	if ((status = i2c_writeRegN(&hmc5883->i2c, HMC5883_REG_CRA, buffer, 3)) != STATUS_OK) {
		goto restore;
	}

	// poll DRDY on status register to wait till data is ready for reading
	while(hmc5883_isDataReady(hmc5883) == 0);

	xSum = ySum = zSum = 0;
	for (i = 0; i < nAvg; i++) {
		if ((status = hmc5883_read(hmc5883, &x, &y, &z)) != STATUS_OK) {
			goto restore;
		}

		xSum += x;
		ySum += y;
		zSum += z;

		while(hmc5883_isDataReady(hmc5883) == 0);
	}

	xSum /= nAvg;
	ySum /= nAvg;
	zSum /= nAvg;
	if (xSum < 243 || xSum > 575 || ySum < 243 || ySum > 575 || zSum < 243 || zSum > 575) {
		status = 0;
		goto restore;
	}

	status = STATUS_OK;

restore:
	i2c_writeRegN(&hmc5883->i2c, HMC5883_REG_CRA, prev, 3);
	return status;
}


int32_t hmc5883_setAvgSample(hmc5883_t *hmc5883, hmc5883MAControl avg) {
	return i2c_updateReg(&hmc5883->i2c, HMC5883_REG_CRA, HMC5883_MASK_MA, avg);
}


int32_t hmc5883_setDataRate(hmc5883_t *hmc5883, hmc5883DORate rate) {
	return i2c_updateReg(&hmc5883->i2c, HMC5883_REG_CRA, HMC5883_MASK_DOR, rate);
}


int32_t hmc5883_setGain(hmc5883_t *hmc5883, hmc5883GNControl gain) {
	if (0 <= gain && gain <= 7)
		gain <<= 5;

	return i2c_updateReg(&hmc5883->i2c, HMC5883_REG_CRB, HMC5883_MASK_GN, gain);
}


int32_t hmc5883_setMode(hmc5883_t *hmc5883, hmc5883MDSelect mode) {
	return i2c_updateReg(&hmc5883->i2c, HMC5883_REG_MR, HMC5883_MASK_MS, mode);
}


int32_t hmc5883_getId(hmc5883_t *hmc5883, uint8_t* const id) {
	return i2c_readRegN(&hmc5883->i2c, HMC5883_REG_IRA, id, 3);
}


int32_t hmc5883_isDataReady(hmc5883_t *hmc5883) {
	uint8_t buffer;
	int32_t status;

	if ((status = i2c_readRegN(&hmc5883->i2c, HMC5883_REG_SR, &buffer, 1)) != STATUS_OK)
		return status;

	return ((buffer & HMC5883_SR_DRDY) != 0);
}


int32_t hmc5883_read(hmc5883_t *hmc5883, int16_t* x, int16_t* y, int16_t* z) {
	uint8_t buffer[6];
	int32_t status;

	if ((status = i2c_readRegN(&hmc5883->i2c, HMC5883_REG_DOX_MSB, buffer, 6)) != STATUS_OK)
		return status;

	*x = (buffer[0] << 8) | buffer[1];
	*z = (buffer[2] << 8) | buffer[3];
	*y = (buffer[4] << 8) | buffer[5];

	return STATUS_OK;
}


int32_t hmc5883_readGauss(hmc5883_t *hmc5883, float* x, float* y, float* z) {
	int16_t rawX, rawY, rawZ;
	uint8_t gain;
	float factor = 1.0f;
	int32_t status;

	if ((status = hmc5883_read(hmc5883, &rawX, &rawY, &rawZ)) != STATUS_OK)
		return status;

	if ((status = i2c_readRegN(&hmc5883->i2c, HMC5883_REG_CRB, &gain, 1)) != STATUS_OK)
		return status;

	gain &= HMC5883_MASK_GN;
	switch(gain) {	// gain is reused
	case HMC5883_GN_1370:
		factor = 1370;
		break;
	case HMC5883_GN_1090:
		factor = 1090;
		break;
	case HMC5883_GN_820:
		factor = 820;
		break;
	case HMC5883_GN_660:
		factor = 660;
		break;
	case HMC5883_GN_440:
		factor = 440;
		break;
	case HMC5883_GN_390:
		factor = 390;
		break;
	case HMC5883_GN_330:
		factor = 330;
		break;
	case HMC5883_GN_230:
		factor = 230;
		break;
	}

	*x = rawX/factor;
	*y = rawY/factor;
	*z = rawZ/factor;

	return STATUS_OK;
}

