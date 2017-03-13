/*
 * ds1307.h
 *
 *  Created on: 14/06/2013
 *      Author: lcc
 */


#ifndef DS1307_H
#define DS1307_H

#include <stdint.h>

#define DS1307_I2C_ADDR				0x68

/// defines for register address
#define DS1307_REG_SECOND			0x00
#define DS1307_REG_MINUTE			0x01
#define DS1307_REG_HOUR				0x02
#define DS1307_REG_DAY				0x03
#define DS1307_REG_DATE				0x04
#define DS1307_REG_MONTH			0x05
#define DS1307_REG_YEAR				0x06
#define DS1307_REG_CONTROL			0x07
#define DS1307_REG_HUNDRED_YEAR		0x08	// use one byte of RAM to store hundred-year
#define DS1307_REG_RAM_BASE			0x09

#define DS1307_MAX_USER_RAM			55	// maximum user accessible RAM

/// defines for masks
#define DS1307_MASK_TEN_SECOND		0x70
#define DS1307_MASK_SECOND			0x0F
#define DS1307_MASK_TEN_MINUTE		0x70
#define DS1307_MASK_MINUTE			0x0F
#define DS1307_MASK_TEN_HOUR		0x30	// bit5 will be set for PM and clear for AM when in 12hr mode
#define DS1307_MASK_HOUR			0x0F
#define DS1307_MASK_DAY				0x07
#define DS1307_MASK_TEN_DATE		0x30
#define DS1307_MASK_DATE			0x0F
#define DS1307_MASK_TEN_MONTH		0x10
#define DS1307_MASK_MONTH			0x0F
#define DS1307_MASK_TEN_YEAR		0xF0
#define DS1307_MASK_YEAR			0x0F

#define DS1307_MASK_RS				0x03

#define DS1307_CLOCK_HALT			0x80	// set to stop the clock, clear to start
#define DS1307_12HR_FORMAT			0x40	// set for 12-hour mode, clear for 2-hour mode
#define DS1307_PM					0x20	// set for PM, clear for AM
#define DS1307_OUT					0x80
#define DS1307_SQWE					0x10

typedef enum {
	DS1307_SUNDAY = 1,
	DS1307_MONDAY,
	DS1307_TUESDAY,
	DS1307_WEDNESDAY,
	DS1307_THURSDAY,
	DS1307_FRIDAY,
	DS1307_SATURDAY
} ds1307DayOfWeek;

typedef enum {
	DS1307_SQW_DISABLE		= 0x00,
	DS1307_SQW_1HZ 			= 0x10,
	DS1307_SQW_4_096HZ 		= 0x11,
	DS1307_SQW_8_192HZ 		= 0x12,
	DS1307_SQW_32_768HZ 	= 0x13
} ds1307SQWRateSelect;

typedef enum {
	DS1307_24HR_FORMAT		= 0,
	DS1307_12HR_PM			= 1,
	DS1307_12HR_AM			= 2
} ds1307AmPmFlag;

typedef struct {
	/* I2C index used for this ds1307_t instance. */
	uint8_t i2cIdx;
} ds1307Cfg_t;

typedef struct ds1307_s ds1307_t;

/// Macro to enable square wave output on SQW/OUT pin.
/// \param[in/out] ds1307 DS1307 instance.
/// \return STATUS_OK if success, STATUS_ERROR otherwise.
#define ds1307_sw_enable(ds1307)		\
	i2c_updateReg(&((ds1307)->i2c), DS1307_REG_CONTROL, DS1307_SQWE, DS1307_SQWE);

/// Macro to disable square wave output on SQW/OUT pin.
/// \param[in/out] ds1307 DS1307 instance.
/// \return STATUS_OK if success, STATUS_ERROR otherwise.
#define ds1307_sw_disable(ds1307)		\
	i2c_updateReg(&((ds1307)->i2c), DS1307_REG_CONTROL, DS1307_SQWE, 0);

/// Macro to start the clock ticking of DS1307.
/// \param[in/out] ds1307 DS1307 instance.
/// \return STATUS_OK if success, STATUS_ERROR otherwise.
#define ds1307_start(ds1307)			\
		i2c_updateReg(&((ds1307)->i2c), DS1307_REG_SECOND, DS1307_CLOCK_HALT, 0);

/// Macro to stop the clock ticking of DS1307.
/// \param[in/out] ds1307 DS1307 instance.
/// \return STATUS_OK if success, STATUS_ERROR otherwise.
#define ds1307_stop(ds1307)				\
		i2c_updateReg(&((ds1307)->i2c), DS1307_REG_SECOND, DS1307_CLOCK_HALT, DS1307_CLOCK_HALT);


#ifdef __cplusplus
extern "C" {
#endif


/// Create a DS1307 real-time clock instance with supported parameter. Once created
/// the time will be set to 24 hour format at 00:00 and the date set to
/// 01 Jan 2000, Saturday. The time on DS1307 will be started.
/// \param[out] ppDs1307 Address to store the newly created DS1307 instance.
/// \param[in] cfg Configuration used to create a new DS1307 instance.
/// \return STATUS_OK if successfully created. STATUS_ERROR otherwise.
int32_t ds1307_create(ds1307_t **ppDs1307, const ds1307Cfg_t *cfg);

/// Destroy a DS1307 instance and releae its resources.
/// \param[in/out] ppDs1307 Address to store the newly created DS1307 instance.
/// \return STATUS_OK if successfully destroyed. STATUS_ERROR otherwise.
int32_t ds1307_destroy(ds1307_t **ppDs1307);

/// To set the rate of the square wave output on SQW/OUT pin.
/// \param[in/out] ds1307 DS1307 instance.
/// \param[in] rate The rate of the output square wave. \ref ds1307SQWRateSelect.
/// \return STATUS_OK if success, STATUS_ERROR otherwise.
int32_t ds1307_sw_setRate(ds1307_t *ds1307, ds1307SQWRateSelect rate);

/// To change the DS1307 internal hour representation format to 12 or 24-hour format.
/// Need to convert the value in DS1307_REG_HOUR when switching format since DS1307
/// does not automatically do so.
/// \param[in/out] ds1307 DS1307 instance.
/// \param[in] format 0 for 24-hour format, non-zero for 12-hour format. Can also use
/// 	the define DS1307_24HR_FORMAT or DS1307_12HR_FORMAT
int32_t ds1307_changeFormat(ds1307_t *ds1307, uint8_t format);

/// To get the time. The time returned can be in 24-hour or 12-hour format.
/// \param[in/out] ds1307 DS1307 instance.
/// \param[out] hour Hour in integer. If amPmFlag == DS1307_24HR, then hour is in 24-hour format.
/// \param[out] minute Minute in integer.
/// \param[out] second Second in integer.
/// \param[out] amPmFlag A flag to indicate the hour is in 24-hour format, 12-hour AM or PM. \ref ds1307AmPmFlag.
/// \return 1 if successfully read time from DS1307 module. 0 otherwise.
int32_t ds1307_getTime(ds1307_t *ds1307, int32_t* const hour, int32_t* const minute, int32_t* const second, ds1307AmPmFlag* const amPmFlag);

/// To get the day of week.
/// \param[in/out] ds1307 DS1307 instance.
/// \param[out] day The day of week as given by \ref ds1307DayOfWeek.
/// \return 1 if read successful. 0 otherwise.
int32_t ds1307_getDay(ds1307_t *ds1307, ds1307DayOfWeek* const day);

/// To get the date.
/// \param[in/out] ds1307 DS1307 instance.
/// \param[out] date The day of month in integer.
/// \param[out] month The month in integer.
/// \param[out] year The  4-digit year in integer.
/// \return 1 if read successful. 0 otherwise.
int32_t ds1307_getDate(ds1307_t *ds1307, int32_t* const date, int32_t* const month, int32_t* const year);

/// To set the time. Convert to 12-hour format and set the AM/PM flag
/// if the mode is in 12-hour mode when calling this function. DS1307
/// does not automatically convert hour to 12-hour format. Perform argument
/// checking to ensure 0 <= hour <= 23, 0 <= minute <= 59 and 0 <= second <= 59.
/// \param[in/out] ds1307 DS1307 instance.
/// \param[in] hour Hour in 24-hour format.
/// \param[in] minute Minute in integer.
/// \param[in] second Second in integer.
/// \return 1 if hour, minute and second provided are valid, 0 otherwise.
int32_t ds1307_setTime(ds1307_t *ds1307, int32_t hour, int32_t minute, int32_t second);

/// To set the day of week.
/// \param[in/out] ds1307 DS1307 instance.
/// \param[in] day Day of week in integer.
/// \return 1 if day provided is valid, 0 otherwise.
int32_t ds1307_setDay(ds1307_t *ds1307, ds1307DayOfWeek day);

/// To set the date. Automatically perform checking for valid date, month and year.
/// Valid date is 1 <= date <= 28, 29, 30 or 31 depending on the month and leap year.
/// Leap year is determined by years that is divisible by 4. Valid month is
/// 1 <= month <= 12. For year, its hundred value is stored in the first RAM.
/// \param[in/out] ds1307 DS1307 instance.
/// \param[in] date Date in integer.
/// \param[in] month Month in integer.
/// \param[in] year 4-digit year in integer.
/// \return 1 if all the date provided are valid, 0 otherwise.
int32_t ds1307_setDate(ds1307_t *ds1307, int32_t date, int32_t month, int32_t year);

/// To read a number of byte from non-volatile RAM. Perform checking to ensure that the
/// reading does not exceed the RAM index.
/// \param[in/out] ds1307 DS1307 instance.
/// \param[in] ramStartIndex The starting index which data is to be read. Not the RAM address.
/// \param[in] buffer Buffer containing the read data. The buffer must have sufficient length as specified by len.
/// \param[in] len The number of bytes to read.
/// \return 1 if read success. 0 otherwise.
int32_t ds1307_readRam(ds1307_t *ds1307, uint8_t ramStartIndex, uint8_t* const buffer, uint32_t len);

/// To write a number of byte into non-volatile RAM. Perform checking to ensure that the
/// data does not exceed the RAM space.
/// \param[in/out] ds1307 DS1307 instance.
/// \param[in] ramStartIndex The starting index which data is to be written. Not the RAM address.
/// \param[in] data Data to be written. The data must have sufficient length as specified by len.
/// \param[in] len The number of bytes to write.
/// \return 1 if write success. 0 otherwise.
int32_t ds1307_writeRam(ds1307_t *ds1307, uint8_t ramStartIndex, const uint8_t* const data, uint32_t len);

/**
 * Temporary fix. Exposing direct access to module registers.
 */
int32_t ds1307_updateReg(ds1307_t *ds1307, uint8_t regAddr, uint8_t mask, uint8_t data);

int32_t ds1307_readRegN(ds1307_t *ds1307, uint8_t regStartAddr, uint8_t* const buffer, uint32_t len);

int32_t ds1307_writeRegN(ds1307_t *ds1307, uint8_t regStartAddr, const uint8_t* const data, uint32_t len);


#ifdef __cplusplus
}
#endif

#endif
