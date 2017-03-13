/*
 * ds1307.c
 *
 *  Created on: 19/06/2013
 *      Author: lcc
 */

#include <stdio.h>
#include <stdlib.h>
#include "util/status.h"
#include "hw/i2c.h"
#include "module/ds1307-rtc/ds1307.h"

#if 0
/* Default clock value upon creation */
#define DS1307_DEFAULT_HR_FORMAT	(DS1307_24HR_FORMAT)
#define DS1307_DEFAULT_HOUR			(0)
#define DS1307_DEFAULT_MINUTE		(0)
#define DS1307_DEFAULT_SECOND		(0)
#define DS1307_DEFAULT_DATE			(1)
#define DS1307_DEFAULT_MONTH		(1)
#define DS1307_DEFAULT_YEAR			(2000)
#define DS1307_DEFAULT_DAY			(DS1307_SATURDAY)
#endif

struct ds1307_s {
	i2c_t i2c;
};

int32_t ds1307_create(ds1307_t **ppDs1307, const ds1307Cfg_t *cfg) {
	ds1307_t *ds1307;

	ds1307 = (ds1307_t*) malloc(sizeof(ds1307_t));
	if (NULL == ds1307) {
		return STATUS_ERROR_MALLOC;
	}

	*ppDs1307 = ds1307;
	ds1307->i2c.i2cIdx = cfg->i2cIdx;
	ds1307->i2c.slaveAddr = DS1307_I2C_ADDR;

#if 0
	/* Must not reset the time and date every time a instance is created. This is
	 * because, if the module is running on battery, this will then reset the time.
	 * The desired behaviour is to retain the current time. */
	/* Set to default value upon creation */
	ds1307_changeFormat(ds1307, DS1307_DEFAULT_HR_FORMAT);
	ds1307_setTime(ds1307, DS1307_DEFAULT_HOUR, DS1307_DEFAULT_MINUTE, DS1307_DEFAULT_SECOND);
	ds1307_setDate(ds1307, DS1307_DEFAULT_DATE, DS1307_DEFAULT_MONTH, DS1307_DEFAULT_YEAR);
	ds1307_setDay(ds1307, DS1307_DEFAULT_DAY);
	ds1307_start(ds1307);
#endif

	return STATUS_OK;
}

int32_t ds1307_destroy(ds1307_t **ppDs1307) {
	if (NULL != *ppDs1307) {
		free(*ppDs1307);
		*ppDs1307 = NULL;
	}

	return STATUS_OK;
}

int32_t ds1307_sw_setRate(ds1307_t *ds1307, ds1307SQWRateSelect rate) {
	return i2c_updateReg(&ds1307->i2c, DS1307_REG_CONTROL, DS1307_SQWE|DS1307_MASK_RS, rate | DS1307_SQWE);
}

int32_t ds1307_changeFormat(ds1307_t *ds1307, uint8_t format) {
	uint8_t current;

	i2c_readRegN(&ds1307->i2c, DS1307_REG_HOUR, &current, 1);

	if (format == 0 && (current & DS1307_12HR_FORMAT) != 0) { // convert from 12-hour to 24-hour format
		current &= ~(DS1307_12HR_FORMAT); // clear bit 6

		if ((current & DS1307_PM) != 0) {
			current &= ~(DS1307_PM);
			current += 0x12;

			if ((current & DS1307_MASK_HOUR) >= 10)
				current += 6;
		}
	} else if (format != 0 && (current & DS1307_12HR_FORMAT) == 0) {	// convert from 24-hour to 12-hour format
		current |= DS1307_12HR_FORMAT;
		if ((current & (DS1307_MASK_TEN_HOUR | DS1307_MASK_HOUR)) > 0x12) {
			current -= 0x12;

			if ((current & DS1307_MASK_HOUR) >= 10)
				current -= 6;

			current |= DS1307_PM;
		}
	} // do nothing if existing format is same

	return i2c_updateReg(&ds1307->i2c, DS1307_REG_HOUR, 0xFF, current);
}

int32_t ds1307_getTime(ds1307_t *ds1307, int32_t* const hour, int32_t* const minute, int32_t* const second, ds1307AmPmFlag* const amPmFlag) {
	uint8_t buffer[3];
	int32_t status;

	if ((status = i2c_readRegN(&ds1307->i2c, DS1307_REG_SECOND, buffer, 3)) != STATUS_OK)
		return status;

	*second = ((buffer[0] & DS1307_MASK_TEN_SECOND) >> 4) * 10
			+ (buffer[0] & DS1307_MASK_SECOND);
	*minute = ((buffer[1] & DS1307_MASK_TEN_MINUTE) >> 4) * 10
			+ (buffer[1] & DS1307_MASK_MINUTE);

	if ((buffer[2] & DS1307_12HR_FORMAT) == 0) { 	// 24-hour format
		*amPmFlag = DS1307_24HR_FORMAT;
		*hour   = ((buffer[2] & DS1307_MASK_TEN_HOUR) >> 4) * 10
					+ (buffer[2] & DS1307_MASK_HOUR);
	} else {	// 12-hour format
		*hour   = ((buffer[2] & DS1307_MASK_TEN_HOUR & ~DS1307_PM) >> 4) * 10
					+ (buffer[2] & DS1307_MASK_HOUR);
		if ((buffer[2] & DS1307_PM) == 0) {		// AM
			*amPmFlag = DS1307_12HR_AM;
		} else {	//PM
			*amPmFlag = DS1307_12HR_PM;
		}
	}

	return STATUS_OK;
}

int32_t ds1307_getDay(ds1307_t *ds1307, ds1307DayOfWeek* const day) {
	uint8_t temp;
	int32_t status;

	if ((status = i2c_readRegN(&ds1307->i2c, DS1307_REG_DAY, &temp, 1)) != STATUS_OK)
		return status;

	*day = temp;

	return STATUS_OK;
}

int32_t ds1307_getDate(ds1307_t *ds1307, int32_t* const date, int32_t* const month, int32_t* const year) {
	uint8_t buffer[3];
	int32_t status;

	if ((status = i2c_readRegN(&ds1307->i2c, DS1307_REG_DATE, buffer, 3)) != STATUS_OK)
		return status;

	*date  = ((buffer[0] & DS1307_MASK_TEN_DATE) >> 4) * 10
			+ (buffer[0] & DS1307_MASK_DATE);
	*month = ((buffer[1] & DS1307_MASK_TEN_MONTH) >> 4) * 10
			+ (buffer[1] & DS1307_MASK_MONTH);
	*year  = ((buffer[2] & DS1307_MASK_TEN_YEAR) >> 4) * 10
			+ (buffer[2] & DS1307_MASK_YEAR);

	if ((status = i2c_readRegN(&ds1307->i2c, DS1307_REG_HUNDRED_YEAR, buffer, 1)) != STATUS_OK)
		return status;

	*year += buffer[0]*100;

	return STATUS_OK;
}

int32_t ds1307_setTime(ds1307_t *ds1307, int32_t hour, int32_t minute, int32_t second) {
	uint8_t buffer;
	int32_t status;

	if (second < 0 || second > 59) {
		return STATUS_ERROR_PARAM;
	}

	// mask is not necessary, only as extra security
	second = (((second/10) << 4) & DS1307_MASK_TEN_SECOND) | ((second % 10) & DS1307_MASK_SECOND);

	if (minute < 0 || minute > 59) {
		return STATUS_ERROR_PARAM;
	}

	// mask is not necessary, only as extra security
	minute = (((minute/10) << 4) & DS1307_MASK_TEN_MINUTE) | ((minute % 10) & DS1307_MASK_MINUTE);

	if (hour < 0 || hour > 23) {
		return STATUS_ERROR_PARAM;
	}

	// determine if DS1307 is in 12 or 24-hour format
	i2c_readRegN(&ds1307->i2c, DS1307_REG_HOUR, &buffer, 1);
	if ((buffer & DS1307_12HR_FORMAT) > 0 && hour > 12) { 	//12-hour format
		hour -= 12;

		// mask is not necessary, only as extra security
		hour = DS1307_PM | (((hour/10) << 4) & DS1307_MASK_TEN_HOUR) | ((hour % 10) & DS1307_MASK_HOUR);
	} else {
		// mask is not necessary, only as extra security
		hour = (((hour/10) << 4) & DS1307_MASK_TEN_HOUR) | ((hour % 10) & DS1307_MASK_HOUR);
	}

	if ((status = i2c_updateReg(&ds1307->i2c, DS1307_REG_HOUR, DS1307_MASK_TEN_HOUR|DS1307_MASK_HOUR, hour)) != STATUS_OK)
		return status;

	if ((status = i2c_updateReg(&ds1307->i2c, DS1307_REG_MINUTE, DS1307_MASK_TEN_MINUTE|DS1307_MASK_MINUTE, minute)) != STATUS_OK)
		return status;

	if ((status = i2c_updateReg(&ds1307->i2c, DS1307_REG_SECOND, DS1307_MASK_TEN_SECOND|DS1307_MASK_SECOND, second)) != STATUS_OK)
		return status;

	return STATUS_OK;
}

int32_t ds1307_setDay(ds1307_t *ds1307, ds1307DayOfWeek day) {
	if (day < 1 || day > 7)
		/* Invalid day of week */
		return STATUS_ERROR_PARAM;

	return i2c_updateReg(&ds1307->i2c, DS1307_REG_DAY, DS1307_MASK_DAY, day);
}

int32_t ds1307_setDate(ds1307_t *ds1307, int32_t date, int32_t month, int32_t year) {
	int32_t maxDate;	// max date for each month
	uint8_t hundredYear;
	int32_t status;

	switch (month) {
	case 1:
	case 3:
	case 5:
	case 7:
	case 8:
	case 10:
	case 12:
		maxDate = 31;
		break;
	case 2:
		if ((year % 4) == 0)	// is leap year
			maxDate = 29;
		else
			maxDate = 28;
		break;
	default:
		maxDate = 30;
		break;
	}

	if (date < 1 || date > maxDate) {
		return STATUS_ERROR_PARAM;
	}

	date = (((date/10) << 4) & DS1307_MASK_TEN_DATE) | ((date % 10) & DS1307_MASK_DATE);

	if (month < 1 || month > 12) {
		return STATUS_ERROR_PARAM;
	}

	month = (((month/10) << 4) & DS1307_MASK_TEN_MONTH) | ((month % 10) & DS1307_MASK_MONTH);

	hundredYear = year/100;
	year %= 100;	// only get the 2-digit year
	year = (((year/10) << 4) & DS1307_MASK_TEN_YEAR) | ((year % 10) & DS1307_MASK_YEAR);

	if ((status = i2c_writeRegN(&ds1307->i2c, DS1307_REG_HUNDRED_YEAR, &hundredYear, 1)) != STATUS_OK)
		return status;

	if ((status = i2c_updateReg(&ds1307->i2c, DS1307_REG_YEAR, DS1307_MASK_TEN_YEAR|DS1307_MASK_YEAR, year)) != STATUS_OK)
		return status;

	if ((status = i2c_updateReg(&ds1307->i2c, DS1307_REG_MONTH, DS1307_MASK_TEN_MONTH|DS1307_MASK_MONTH, month)) != STATUS_OK)
		return status;

	if ((status = i2c_updateReg(&ds1307->i2c, DS1307_REG_DATE, DS1307_MASK_TEN_DATE|DS1307_MASK_DATE, date)) != STATUS_OK)
		return status;

	return STATUS_OK;
}


int32_t ds1307_readRam(ds1307_t *ds1307, uint8_t ramStartIndex, uint8_t* const buffer, uint32_t len) {
	if ((ramStartIndex + len) > DS1307_MAX_USER_RAM) {
		/* ERROR: Exceeded user addressable RAM! */
		return STATUS_ERROR_PARAM;
	}

	return i2c_readRegN(&ds1307->i2c, DS1307_REG_RAM_BASE + ramStartIndex, buffer, len);
}

int32_t ds1307_writeRam(ds1307_t *ds1307, uint8_t ramStartIndex, const uint8_t* const data, uint32_t len) {
	if ((ramStartIndex + len) > DS1307_MAX_USER_RAM) {
		/* ERROR: Exceeded user addressable RAM */
		return STATUS_ERROR_PARAM;
	}

	return i2c_writeRegN(&ds1307->i2c, DS1307_REG_RAM_BASE + ramStartIndex, data, len);
}


/**
 * Temporary fix
 */
int32_t ds1307_updateReg(ds1307_t *ds1307, uint8_t regAddr, uint8_t mask, uint8_t data) {
	return i2c_updateReg(&ds1307->i2c, regAddr, mask, data);
}

int32_t ds1307_readRegN(ds1307_t *ds1307, uint8_t regStartAddr, uint8_t* const buffer, uint32_t len) {
	return i2c_readRegN(&ds1307->i2c, regStartAddr, buffer, len);
}

int32_t ds1307_writeRegN(ds1307_t *ds1307, uint8_t regStartAddr, const uint8_t* const data, uint32_t len) {
	return i2c_writeRegN(&ds1307->i2c, regStartAddr, data, len);
}
