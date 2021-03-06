/*
 * mpu6050.h
 *
 *  Created on: 22/06/2013
 *      Author: lcc
 */

#ifndef MPU6050_H
#define MPU6050_H

#include <stdint.h>

#define MPU6050_I2C_ADDR					0x68

/// === DO NOT WRITE DIRECTLY TO THESE REGISTERS ===
#define MPU6050_REG_XA_OFFS_H				0x06
#define MPU6050_REG_XA_OFFS_L				0x07
#define MPU6050_REG_YA_OFFS_H				0x08
#define MPU6050_REG_YA_OFFS_L				0x09
#define MPU6050_REG_ZA_OFFS_H				0x0A
#define MPU6050_REG_ZA_OFFS_L				0x0B
#define MPU6050_REG_XG_OFFS_H				0x13
#define MPU6050_REG_XG_OFFS_L				0x14
#define MPU6050_REG_YG_OFFS_H				0x15
#define MPU6050_REG_YG_OFFS_L				0x16
#define MPU6050_REG_ZG_OFFS_H				0x17
#define MPU6050_REG_ZG_OFFS_L				0x18
/// ================================================


#define MPU6050_REG_FF_THR					0x1D
#define MPU6050_REG_FF_DUR					0x1E
#define MPU6050_REG_MOT_THR					0x1F
#define MPU6050_REG_MOT_DUR					0x20
#define MPU6050_REG_ZRMOT_THR				0x21
#define MPU6050_REG_ZRMOT_DUR				0x22

#define MPU6050_REG_SELF_TEST_X				0x0D
#define MPU6050_REG_SELF_TEST_Y				0x0E
#define MPU6050_REG_SELF_TEST_Z				0x0F
#define MPU6050_REG_SELF_TEST_A				0x10
#define MPU6050_REG_SMPLRT_DIV				0x19
#define MPU6050_REG_CONFIG					0x1A
#define MPU6050_REG_GYRO_CONFIG				0x1B
#define MPU6050_REG_ACCEL_CONFIG			0x1C
#define MPU6050_REG_FIFO_EN					0x23
#define MPU6050_REG_I2C_MST_CTRL			0x24

/// Macro to get the register address for I2C_SLV_ADDR
/// SLV0 = 0x25
/// SLV1 = 0x28
/// SLV2 = 0x2B
/// SLV3 = 0x2E
/// SLV4 = 0x31
#define MPU6050_REG_I2C_SLV_ADDR(X) (0x25+3*X)

/// Macro to get the register address for I2C_SLV_REG
/// SLV0 = 0x26
/// SLV1 = 0x29
/// SLV2 = 0x2C
/// SLV3 = 0x2F
/// SLV4 = 0x32
#define MPU6050_REG_I2C_SLV_REG(X) (0x26+3*X)

/// Macro to get the register address for I2C_SLV_CTRL
/// SLV0 = 0x27
/// SLV1 = 0x2A
/// SLV2 = 0x2D
/// SLV3 = 0x30
/// SLV4 = 0x34
#define MPU6050_REG_I2C_SLV_CTRL(X) ((X == 4)? 0x34 : (0x27+3*X))

/// Macro to get the register address for I2C_SLV_DO
/// SLV0 = 0x63
/// SLV1 = 0x64
/// SLV2 = 0x65
/// SLV3 = 0x66
/// SLV4 = 0x33
#define MPU6050_REG_I2C_SLV_DO(X) ((X == 4)? 0x33 : (0x63+X))

#define MPU6050_REG_I2C_SLV4_DI				0x35

#define MPU6050_REG_I2C_MST_STATUS			0x36
#define MPU6050_REG_INT_PIN_CFG				0x37
#define MPU6050_REG_INT_ENABLE				0x38
#define MPU6050_REG_INT_STATUS				0x3A
#define MPU6050_REG_ACCEL_XOUT_H			0x3B
#define MPU6050_REG_ACCEL_XOUT_L			0x3C
#define MPU6050_REG_ACCEL_YOUT_H			0x3D
#define MPU6050_REG_ACCEL_YOUT_L			0x3E
#define MPU6050_REG_ACCEL_ZOUT_H			0x3F
#define MPU6050_REG_ACCEL_ZOUT_L			0x40
#define MPU6050_REG_TEMP_OUT_H				0x41
#define MPU6050_REG_TEMP_OUT_L				0x42
#define MPU6050_REG_GYRO_XOUT_H				0x43
#define MPU6050_REG_GYRO_XOUT_L				0x44
#define MPU6050_REG_GYRO_YOUT_H				0x45
#define MPU6050_REG_GYRO_YOUT_L				0x46
#define MPU6050_REG_GYRO_ZOUT_H				0x47
#define MPU6050_REG_GYRO_ZOUT_L				0x48

#define MPU6050_REG_EXT_SENS_DATA_BASE		0x49
#define MPU6050_REG_EXT_SENS_DATA(X) (MPU6050_REG_EXT_SENS_DATA_BASE+X)

#define MPU6050_REG_MOT_DETECT_STATUS		0x61
#define MPU6050_REG_I2C_MST_DELAY_CTRL		0x67
#define MPU6050_REG_SIGNAL_PATH_RESET		0x68
#define MPU6050_REG_MOT_DETECT_CTRL			0x69
#define MPU6050_REG_USER_CTRL				0x6A
#define MPU6050_REG_PWR_MGMT1				0x6B
#define MPU6050_REG_PWR_MGMT2				0x6C
#define MPU6050_REG_FIFO_COUNT_H			0x72
#define MPU6050_REG_FIFO_COUNT_L			0x73
#define MPU6050_REG_FIFO_DATA				0x74
#define MPU6050_REG_WHO_AM_I				0x75


#define MPU6050_MASK_EXT_SYNC_SET			0x38
#define MPU6050_MASK_DLPF_CFG				0x07

typedef enum {				//<  Accel. bandwidth (Hz), delay (ms); Gyro. bandwidth (Hz), delay (ms), Fs (kHz)
	MPU6050_DLPF_CFG0 = 0,	//<          260             0.0                256            0.98         8
	MPU6050_DLPF_CFG1,		//<          184             2.0                188            1.9          1
	MPU6050_DLPF_CFG2,		//<           94             3.0                 98            2.8          1
	MPU6050_DLPF_CFG3,		//<           44             4.9                 42            4.8          1
	MPU6050_DLPF_CFG4,		//<           21             8.5                 20            8.3          1
	MPU6050_DLPF_CFG5,		//<           10            13.8                 10           13.4          1
	MPU6050_DLPF_CFG6		//<            5            19.0                  5           18.6          1
} mpu6050LowPassFilter;


#define MPU6050_MASK_ACCEL_HPF				0x07

typedef enum {
	MPU6050_HPF_RESET	= 0x00,
	MPU6050_HPF_5		= 0x01,
	MPU6050_HPF_2_5		= 0x02,
	MPU6050_HPF_1_25	= 0x03,
	MPU6050_HPF_0_63	= 0x04,
	MPU6050_HPF_HOLD	= 0x07
} mpu6050AccelHighPassFilter;

#define MPU6050_X_ST						0x80
#define MPU6050_Y_ST						0x40
#define MPU6050_Z_ST						0x20
#define MPU6050_XYZ_ST						(MPU6050_X_ST | MPU6050_Y_ST | MPU6050_Z_ST)
#define MPU6050_MASK_FS_SEL					0x18

typedef enum {
	MPU6050_ST_FAIL         = 0x00,
	MPU6050_ST_PASS_X   	= 0x80,		//< X-axis only self-test pass
	MPU6050_ST_PASS_Y   	= 0x40,		//< Y-axis only self-test pass
	MPU6050_ST_PASS_Z   	= 0x20,		//< Z-axis only self-test pass
	MPU6050_ST_PASS_XYZ 	= 0xE0		//< All X,Y,Z-axes self-test pass
} mpu6050SelfTestResult;

typedef enum {
	MPU6050_GFS_SEL_250  	= 0x00,		//< Gyroscope full scale = +/-250 degree/second
	MPU6050_GFS_SEL_500  	= 0x08,		//< Gyroscope full scale = +/-500 degree/second
	MPU6050_GFS_SEL_1000 	= 0x10,		//< Gyroscope full scale = +/-1000 degree/second
	MPU6050_GFS_SEL_2000 	= 0x18		//< Gyroscope full scale = +/-2000 degree/second
} mpu6050GyroFullScale;

typedef enum {
	MPU6050_AFS_SEL_2  		= 0x00,		//< Accelerometer full scale = +/-2g
	MPU6050_AFS_SEL_4  		= 0x08,		//< Accelerometer full scale = +/-4g
	MPU6050_AFS_SEL_8  		= 0x10,		//< Accelerometer full scale = +/-8g
	MPU6050_AFS_SEL_16 		= 0x18		//< Accelerometer full scale = +/-16g
} mpu6050AccelFullScale;


#define MPU6050_TEMP_FIFO_EN				0x80
#define MPU6050_XG_FIFO_EN					0x40
#define MPU6050_YG_FIFO_EN					0x20
#define MPU6050_ZG_FIFO_EN					0x10
#define MPU6050_ACCEL_FIFO_EN				0x08
#define MPU6050_SLV2_FIFO_EN				0x04
#define MPU6050_SLV1_FIFO_EN				0x02
#define MPU6050_SLV0_FIFO_EN				0x01
#define MPU6050_MULT_MST_EN					0x80
#define MPU6050_WAIT_FOR_ES					0x40
#define MPU6050_SLV3_FIFO_EN				0x20
#define MPU6050_I2C_MST_P_NSR				0x10
#define MPU6050_MASK_I2C_MST_CLK			0x0F

#define MPU6050_FIFO_LENGTH					1024	//< MPU6050 has a 1024-bytes FIFO buffer

/// bit[15:8]  follows the same REG_FIFO_EN structure.
/// bit[7:0] only follows the SLV3_FIFO_EN bit for REG_I2C_MST_CTRL
typedef enum {
	MPU6050_FIFO_TEMP			= 0x8000,	//< Pushing temperature reading to FIFO
	MPU6050_FIFO_GYRO_X			= 0x4000,	//< Pushing gyro x-axis reading to FIFO
	MPU6050_FIFO_GYRO_Y			= 0x2000,	//< Pushing gyro y-axis reading to FIFO
	MPU6050_FIFO_GYRO_Z			= 0x1000,	//< Pushing gyro z-axis reading to FIFO
	MPU6050_FIFO_GYRO_XYZ		= 0x7000,	//< Pushing gyro x,y,z-axes reading to FIFO
	MPU6050_FIFO_ACCEL_XYZ		= 0x0800,	//< Pushing accel x,y,z-axes reading to FIFO
	MPU6050_FIFO_SLV0			= 0x0100,	//< Pushing i2c slave 0 reading to FIFO
	MPU6050_FIFO_SLV1			= 0x0200,	//< Pushing i2c slave 1 reading to FIFO
	MPU6050_FIFO_SLV2			= 0x0400,	//< Pushing i2c slave 2 reading to FIFO
	MPU6050_FIFO_SLV3			= 0x0020,	//< Pushing i2c slave 3 reading to FIFO
	MPU6050_FIFO_SLV_ALL		= 0x0720,	//< Pushing i2c slave 0,1,2,3 reading to FIFO
	MPU6050_FIFO_ALL			= 0xFF20	//< Pushing all reading to FIFO
} mpu6050FifoEnMask;


typedef enum {
	MPU6050_I2C_MST_CLK_348 = 0,	//< 348 kHz MPU6050 I2C master clock
	MPU6050_I2C_MST_CLK_333,		//< 333 kHz
	MPU6050_I2C_MST_CLK_320,		//< 320 kHz
	MPU6050_I2C_MST_CLK_308,		//< 308 kHz
	MPU6050_I2C_MST_CLK_296,		//< 296 kHz
	MPU6050_I2C_MST_CLK_286,		//< 286 kHz
	MPU6050_I2C_MST_CLK_276,		//< 276 kHz
	MPU6050_I2C_MST_CLK_267,		//< 267 kHz
	MPU6050_I2C_MST_CLK_258,		//< 258 kHz
	MPU6050_I2C_MST_CLK_500,		//< 500 kHz
	MPU6050_I2C_MST_CLK_471,		//< 471 kHz
	MPU6050_I2C_MST_CLK_444,		//< 444 kHz
	MPU6050_I2C_MST_CLK_421,		//< 421 kHz
	MPU6050_I2C_MST_CLK_400,		//< 333 kHz
	MPU6050_I2C_MST_CLK_381,		//< 381 kHz
	MPU6050_I2C_MST_CLK_364			//< 364 kHz
} mpu6050I2cMstClk;

typedef enum {
	MPU6050_I2C_DISABLE		= 0x00,		//< I2C_BYPASS_EN = 0, I2C_MST_EN = 0
	MPU6050_I2C_NORMAL		= 0x01,		//< I2C_BYPASS_EN = 0, I2C_MST_EN = 1
	MPU6050_I2C_BYPASS		= 0x02,		//< I2C_BYPASS_EN = 1, I2C_MST_EN = 0
	MPU6050_I2C_RESET		= 0x03		//< I2C_MST_RESET = 1
} mpu6050I2CMode;

#define MPU6050_I2C_SLV_RW					0x80
#define MPU6050_MASK_I2C_SLV_ADDR			0x7F
#define MPU6050_I2C_SLV_EN					0x80
#define MPU6050_I2C_SLV_BYTE_SW				0x40
#define MPU6050_I2C_SLV_REG_DIS				0x20
#define MPU6050_I2C_SLV_GRP					0x10
#define MPU6050_MASK_I2C_SLV_LEN			0x0F

#define MPU6050_I2C_SLV4_INT_EN				0x40
#define MPU6050_MASK_I2C_MST_DLY			0x1F	//< I2C slave access is further decreases to SampleRate/(1+I2C_MST_DLY)

typedef enum {
	MPU6050_I2C_SLV0 		= 0,
	MPU6050_I2C_SLV1 		= 1,
	MPU6050_I2C_SLV2 		= 2,
	MPU6050_I2C_SLV3 		= 3,
	MPU6050_I2C_SLV4 		= 4,
	MPU6050_I2C_SLV_ALL 	= 5,	//< This value must equals to the number of I2C slaves
} mpu6050I2cSlave;


#define MPU6050_PASS_THRU					0x80
#define MPU6050_I2C_SLV4_DONE				0x40
#define MPU6050_I2C_LOST_ARB				0x20
#define MPU6050_I2C_SLV_NACK(X)				(0x01 << X)
#define MPU6050_I2C_SLV_DLY_EN(X)			(0x01 << X)
#define MPU6050_DELAY_ES_SHADOW				0x80

#define MPU6050_MOT_XNEG					0x80
#define MPU6050_MOT_XPOS					0x40
#define MPU6050_MOT_YNEG					0x20
#define MPU6050_MOT_YPOS					0x10
#define MPU6050_MOT_ZNEG					0x08
#define MPU6050_MOT_ZPOS					0x04
#define MPU6050_ZRMOT						0x01

#define MPU6050_MASK_ACCEL_ON_DELAY			0x30
#define MPU6050_MASK_FF_COUNT				0x0C
#define MPU6050_MASK_MOT_COUNT				0x03

typedef enum {
	MPU6050_COUNT_RESET			= 0x00,		//< Reset counter on non-qualifying sample
	MPU6050_COUNT_DEC1			= 0x01,		//< Counter decrease by 1
	MPU6050_COUNT_DEC2			= 0x02,		//< Counter decrease by 2
	MPU6050_COUNT_DEC4			= 0x03		//< Counter decrease by 4
} mpu6050DecRate;

#define MPU6050_INT_LEVEL					0x80
#define MPU6050_INT_OPEN					0x40
#define MPU6050_LATCH_INT_EN				0x20
#define MPU6050_INT_RD_CLEAR				0x10
#define MPU6050_FSYNC_INT_LEVEL				0x08
#define MPU6050_FSYNC_INT_EN				0x04
#define MPU6050_I2C_BYPASS_EN				0x02

// The bit positions for interrupt enable and interrupt status are same, so only define once
#define MPU6050_INT_FF						0x80
#define MPU6050_INT_MOT						0x40
#define MPU6050_INT_ZMOT					0x20
#define MPU6050_INT_FIFO_OFLOW				0x10
#define MPU6050_INT_I2C_MST					0x08
#define MPU6050_INT_DATA_RDY				0x01

#define MPU6050_GYRO_RESET					0x04
#define MPU6050_ACCEL_RESET					0x02
#define MPU6050_TEMP_RESET					0x01

#define MPU6050_FIFO_EN						0x40
#define MPU6050_I2C_MST_EN					0x20
#define MPU6050_I2C_IF_DIS					0x10
#define MPU6050_FIFO_RESET					0x04
#define MPU6050_I2C_MST_RESET				0x02
#define MPU6050_SIG_COND_RESET				0x01
#define MPU6050_TEMP_DIS					0x08
#define MPU6050_MASK_CLK_SEL				0x07
#define MPU6050_MASK_LP_WAKE_CTRL			0xC0


typedef enum {
	MPU6050_RESET			= 0x80,
	MPU6050_SLEEP			= 0x40,
	// In CYCLE mode, the device will power off all its components except for primary I2C and
	// accelerometer and temperature. Gyroscope will be power down regardless. To further
	// disable temperature, call temp_disable() before setting mode to CYCLE.
	MPU6050_CYCLE			= 0x20,
	MPU6050_NORMAL			= 0x00
} mpu6050Mode;

typedef enum {
	MPU6050_CLKSEL_INT_8MHZ			= 0x00,		//< Internal 8MHz oscillator
	MPU6050_CLKSEL_XG				= 0x01,		//< PLL with X-axis gyro reference
	MPU6050_CLKSEL_YG				= 0x02,		//< PLL with Y-axis gyro reference
	MPU6050_CLKSEL_ZG				= 0x03,		//< PLL with Z-axis gyro reference
	MPU6050_CLKSEL_EXT_32_768KHZ	= 0x04,		//< PLL with external 32.768kHz reference
	MPU6050_CLKSEL_EXT_19_2MHZ		= 0x05,		//< PLL with external 19.2MHz reference
	MPU6050_CLKSEL_STOP				= 0x07		//< Stop the clock and keep timing generator in reset
} mpu6050ClkSelect;

typedef enum {
	MPU6050_WAKEUP_FREQ_1_25	= 0x00,		//< MPU6050 will wake up at 1.25Hz to log sensor reading
	MPU6050_WAKEUP_FREQ_5		= 0x40,		//< 5Hz
	MPU6050_WAKEUP_FREQ_20		= 0x80,		//< 20Hz
	MPU6050_WAKEUP_FREQ_40		= 0xC0		//< 40Hz
} mpu6050WakeUpFreq;

typedef enum {
	MPU6050_AG_X 	= 0x04,
	MPU6050_AG_Y	= 0x02,
	MPU6050_AG_Z	= 0x01,
	MPU6050_AG_XYZ	= 0x07
} mpu6050AccelGyroXYZ;

/// defines for self test use only
#define SELF_TEST_AVG_COUNT					10
#define SELF_TEST_FT_MIN				   -14
#define SELF_TEST_FT_MAX					14

typedef struct mpu6050_s mpu6050_t;

typedef struct {
	/* Index of I2C to be used. */
	uint8_t i2cIdx;
} mpu6050Cfg_t;

// macro to convert 2 bytes to 16-bit integer
#define uint8ToUint16(bytes, idx) ((bytes[idx] << 8) + bytes[idx+1])

#ifdef __cplusplus
extern "C" {
#endif


/*
 * todo: gesture recognition: gesture_init, gesture_read
 * Dropped, since too complicated to implement as weekend project.

int32_t mpu6050_gesture_init(void);

// types of gesture:
// 1. rotate X,Y,X - e.g. earth rotate on its axis
// 2. circulate X,Y,Z - e.g. earth circulate around sun
// 3. tilt X,Y,Z
// 4. shake X, Y, Z
// 5. tap X,Y,Z
// 6. zero motion
int32_t mpu6050_gesture_read(uint8_t* gesture, uint8_t* position);
*/


/// To initialise MPU6050 module for normal reading from sensor register of
/// MPU6050 internal sensors only (temperature, accelerometer and gyroscope).
/// Does not configure for external sensors reading (through auxiliary I2C bus).
/// \param[out] ppMpu6050 Address to store the newly created mpu6050_t instance.
/// \param[in] cfg Configuration to create the mpu6050_t instance.
/// \return STATUS_OK if success, STATUS_ERROR or STATUS_ERROR_MALLOC otherwise.
int32_t mpu6050_create(mpu6050_t **ppMpu6050, const mpu6050Cfg_t *cfg);

/// To destroy MPU6050 instance. Once destroyed, the instance must not be used anymore.
/// \param[in/out] ppMpu6050 Address of mpu6050_t instance to be destroyed. Once
/// 	destroyed, *ppMpu6050 will be NULL.
/// \return STATUS_OK if success, STATUS_ERROR otherwise.
int32_t mpu6050_destroy(mpu6050_t **ppMpu6050);

/// DO NOT USE THIS FUNCTION!
/// To calibrate the accelerometer and gyroscope, and update their x,y,z offset registers.
/// Upon powering down and up MPU6050, their x,y,z offset registers are reset back to
/// default value. The offset value is highly dependent on the full scale settings.
/// When this function returns, all the accelerometer, gyroscope and temperature sensor
/// will be disabled..
/// \param[in/out] mpu6050 Pointer to mpu6050 instance.
/// \return STATUS_OK if success, STATUS_ERROR otherwise.
int32_t mpu6050_calibrate(mpu6050_t *mpu6050);

/// To set the data sampling rate for accelerometer and gyroscope. It is in fact
/// setting the value for sampling rate divider in REG_SMPLRT_DIV. Maximum sampling
/// rate for gyroscope is 8kHz when digital low pass filter is disabled DLPF_CFG of
/// REG_CONFIG is 0 or 7, and 1kHz otherwise. Maximum sampling rate for accelerometer
/// is always 1kHz. Return STATUS_ERROR if specified sample rate cannot be achieved.
/// \param[in/out] mpu6050 Pointer to mpu6050 instance.
/// \param[in] sr Desired sampling rate in Hz.
/// \return STATUS_OK if success, STATUS_ERROR otherwise.
int32_t mpu6050_setSampleRate(mpu6050_t *mpu6050, uint32_t sr);

/// To set the mode for MPU6050. In CYCLE mode, at most the temperature and accel
/// readings can be taken. Gyro will be disabled or in standby.
/// \param[in/out] mpu6050 Pointer to mpu6050 instance.
/// \param[in] mode Operation mode for MPU6050 \ref mpu6050Mode.
/// \param[in] wakeUpFreq Wake up frequency when putting MPU6050 into cycle mode.
/// 	Ignored in other mode.
/// \return STATUS_OK if success, STATUS_ERROR otherwise.
int32_t mpu6050_setMode(mpu6050_t *mpu6050, mpu6050Mode mode, uint8_t wakeUpFreq);

/// To set the threshold and timer count for MPU6050 to detect a free fall motion.
/// A free fall is detected/flaged when the each of the accel axis reading is less
/// than the threshold value, which will cause the free fall duration counter to
/// increment. A free fall interrupt is triggered when the counter reaches the
/// count value.
/// NOTE: Seems like zero motion is not working. The hardware may not support, based
/// on Register Map doc v4.0
/// \param[in/out] mpu6050 Pointer to mpu6050 instance.
/// \param[in] thres The threshold, with 32mg/LSB, e.g. thres*32 = threshold in mg.
/// \param[in] count The duration, in ms. The duration counter ticks at 1kHz.
/// \param[in] decRate The duration counter decrement rate for non-qualifying samples.
/// 	\ref mpu6050DecRate.
/// \return STATUS_OK if success, STATUS_ERROR otherwise.
int32_t mpu6050_setFreeFall(mpu6050_t *mpu6050, uint8_t thres, uint8_t count, mpu6050DecRate decRate);

/// To set the threshold and timer count for MPU6050 to detect a motion.
/// A motion is detected/flaged when the each of the accel axis reading exceeds
/// the threshold value, which will cause the motion duration counter to
/// increment. A motion interrupt is triggered when the counter reaches the
/// count value.
/// \param[in/out] mpu6050 Pointer to mpu6050 instance.
/// \param[in] thres The threshold, with 32mg/LSB, e.g. thres*32 = threshold in mg.
/// \param[in] count The duration, in ms. The duration counter ticks at 1kHz.
/// \param[in] decRate The duration counter decrement rate for non-qualifying samples.
/// 	\ref mpu6050DecRate.
/// \return STATUS_OK if success, STATUS_ERROR otherwise.
int32_t mpu6050_setMotion(mpu6050_t *mpu6050, uint8_t thres, uint8_t count, mpu6050DecRate decRate);

/// To set the threshold and timer count for MPU6050 to detect a zero motion.
/// A zero motion is detected/flagged when the each of the accel axis reading is less
/// than the threshold value, which will cause the zero motion duration counter to
/// increment. Unlike Free Fall or Motion detection, Zero Motion detection triggers
/// an interrupt both when Zero Motion is first detected and when Zero Motion is no
/// longer detected.
/// NOTE: Seems like zero motion is not working. The hardware may not support, based
/// on Register Map doc v4.0
/// \param[in/out] mpu6050 Pointer to mpu6050 instance.
/// \param[in] thres The threshold, with 32mg/LSB, e.g. thres*32 = threshold in mg.
/// \param[in] count The duration. The duration counter ticks at 16Hz, i.e. 1LSB = 64ms
/// \return STATUS_OK if success, STATUS_ERROR otherwise.
int32_t mpu6050_setZeroMotion(mpu6050_t *mpu6050, uint8_t thres, uint8_t count);


/// To set the I2C master clock frequency for MPU6050 module.
/// \param[in/out] mpu6050 Pointer to mpu6050 instance.
/// \param[in] div Clock divider \ref mpu6050I2CMstClk.
/// \return STATUS_OK if success, STATUS_ERROR otherwise.
int32_t mpu6050_i2c_setClockDivider(mpu6050_t *mpu6050, mpu6050I2cMstClk div);

/// To set the operation mode of MPU6050's I2C.
/// \param[in/out] mpu6050 Pointer to mpu6050 instance.
/// \param[in] mode MPU6050 I2C mode. \ref mpu6050I2CMode.
/// \return STATUS_OK if success, STATUS_ERROR otherwise.
int32_t mpu6050_i2c_setMode(mpu6050_t *mpu6050, mpu6050I2CMode mode);

/// To set the I2C address and read/write register address of the I2C slave,
/// and initialise BYTE_SW, REG_DIS and GRP to a default value. I2C slave
/// will not be enabled/disabled in this function, and must be explicitly
/// enabled/disabled by calling the enable/disable function or write directly
/// to REG_I2C_SLV_CTRL.
/// \param[in/out] mpu6050 Pointer to mpu6050 instance.
/// \param[in] slave Slave number 0-4 \ref mpu6050I2CSlave.
/// \param[in] i2cAddr The slave I2C address. This address will be stored in
/// 	REG_I2C_SLV_ADDR.
/// \param[in] slaveReg The register address in the slave, from/to where data
/// 	will be read/written. This address will be stored in REG_I2C_SLV_REG.
/// \return STATUS_OK if success, STATUS_ERROR otherwise.
int32_t mpu6050_i2c_initSlave(mpu6050_t *mpu6050, mpu6050I2cSlave slave, uint8_t i2cAddr, uint8_t slaveReg);

/// To set the I2C slave to read mode, i.e. data is read from I2C slave.
/// \param[in/out] mpu6050 Pointer to mpu6050 instance.
/// \param[in] slave Slave number 0-4 \ref mpu6050I2CSlave.
/// \param[in] len Number of bytes to read from I2C slave.
//// \return STATUS_OK if success, STATUS_ERROR otherwise.
int32_t mpu6050_i2c_setReadFromSlave(mpu6050_t *mpu6050, mpu6050I2cSlave slave, uint32_t len);

/// To set the I2C slave to write mode, i.e. data is written to I2C slave.
/// \param[in/out] mpu6050 Pointer to mpu6050 instance.
/// \param[in] slave Slave number 0-4 \ref mpu6050I2CSlave.
/// \param[in] data A single byte data to write to I2C slave. This data will be stored
/// 	in DO register of the specified I2C slave.
/// \return STATUS_OK if success, STATUS_ERROR otherwise.
int32_t mpu6050_i2c_setWriteToSlave(mpu6050_t *mpu6050, mpu6050I2cSlave slave, uint8_t data);

/// To enable I2C slave for read/write.
/// \param[in/out] mpu6050 Pointer to mpu6050 instance.
/// \param[in] slave Slave number 0-4 \ref mpu6050I2CSlave.
/// \return STATUS_OK if success, STATUS_ERROR otherwise.
int32_t mpu6050_i2c_enableSlave(mpu6050_t *mpu6050, mpu6050I2cSlave slave);

/// To disable I2C slave for read/write.
/// \param[in/out] mpu6050 Pointer to mpu6050 instance.
/// \param[in] slave Slave number 0-4 \ref mpu6050I2CSlave.
/// \return STATUS_OK if success, STATUS_ERROR otherwise.
int32_t mpu6050_i2c_disableSlave(mpu6050_t *mpu6050, mpu6050I2cSlave slave);

/// To enable pushing internal and/or external sensor reading to FIFO. Will
/// automatically reset FIFO buffer to ensure that the new data pushed to
/// FIFO buffer is synchronised. FIFO_EN bit in REG_USER_CTRL register will
/// be automatically enabled if there is at least one sensor reading to be
/// pushed to FIFO. Only the corresponding mask with bit 1 in fifoEnMask
/// will be enabled. The one with bit 0 will not be altered. Normal function
/// call will be:
///		mpu6050_fifo_disable(MPU6050_FIFO_ALL);	// disable all FIFO
///		mpu6050_fifo_enable(desired);			// enable the desired FIFO
/// \param[in/out] mpu6050 Pointer to mpu6050 instance.
/// \param[in] fifoEnMask FIFO enable values \ref mpu6050FifoEnMask.
/// \return STATUS_OK if success, STATUS_ERROR otherwise.
int32_t mpu6050_fifo_enable(mpu6050_t *mpu6050, mpu6050FifoEnMask fifoEnMask);

/// To disable pushing internal and/or external sensor reading to FIFO. Will
/// automatically reset FIFO buffer to ensure that the new data pushed to
/// FIFO buffer is synchronised. If no sensor reading is to be pushed to FIFO,
/// i.e. all sensors FIFO_EN are disabled, FIFO_EN bit in REG_USER_CTRL
/// register will be automatically disabled. Only the corresponding mask with
/// bit 1 in fifoEnMask will be disabled. The one with bit 0 will not be altered.
/// \param[in/out] mpu6050 Pointer to mpu6050 instance.
/// \param[in] fifoEnMask FIFO disable values \ref mpu6050FifoEnMask.
/// \return STATUS_OK if success, STATUS_ERROR otherwise.
int32_t mpu6050_fifo_disable(mpu6050_t *mpu6050, mpu6050FifoEnMask fifoEnMask);

/// To reset the FIFO buffer. After reset, the value of FIFO_EN in REG_USER_CTRL
/// remains as its previous value before reset.
/// \param[in/out] mpu6050 Pointer to mpu6050 instance.
/// \return STATUS_OK if success, STATUS_ERROR otherwise.
int32_t mpu6050_fifo_reset(mpu6050_t *mpu6050);

/// To read a number of bytes from FIFO buffer.
/// \param[in/out] mpu6050 Pointer to mpu6050 instance.
/// \param[out] data Byte array containing the values read from FIFO buffer.
/// 	Must has sufficient length.
/// \param[in] len Maximum number of bytes to be read from FIFO buffer.
/// \return Actual number of bytes read from FIFO buffer. This value will be less
/// 	than len only when FIFO buffer does not have enough data to read from.
///		STATUS_ERROR if error occurs.
int32_t mpu6050_fifo_read(mpu6050_t *mpu6050, uint8_t* const data, uint32_t len);

/// To write a number of bytes to FIFO buffer. This function is implemented
/// only for completeness and should not normally be used. If the FIFO does not
/// have enough free space, only part of the data will be written to FIFO buffer.
/// \param[in/out] mpu6050 Pointer to mpu6050 instance.
/// \param[in] data Byte array containing the values to be written to FIFO buffer.
/// \param[in] len Number of bytes to be written to FIFO buffer.
/// \return Actual number of bytes written to FIFO buffer. This value will be less
/// 	than len only when FIFO buffer does not have enough free space to write to.
///		STATUS_ERROR if error occurs.
int32_t mpu6050_fifo_write(mpu6050_t *mpu6050, const uint8_t* const data, uint32_t len);

/// To enable temperature sensor, i.e putting it out of standby mode by clearing
/// the bits in REG_PWR_MGMT1.
/// \param[in/out] mpu6050 Pointer to mpu6050 instance.
/// \return STATUS_OK if success, STATUS_ERROR otherwise.
int32_t mpu6050_temp_enable(mpu6050_t *mpu6050);

/// To disable temperature sensor, i.e putting it into standby mode by setting
/// the bits in REG_PWR_MGMT1.
/// \param[in/out] mpu6050 Pointer to mpu6050 instance.
/// \return STATUS_OK if success, STATUS_ERROR otherwise.
int32_t mpu6050_temp_disable(mpu6050_t *mpu6050);

/// To reset the temperature sensor. This will revert the ADC signal path to their
/// power up configuration.
/// \param[in/out] mpu6050 Pointer to mpu6050 instance.
/// \return STATUS_OK if success, STATUS_ERROR otherwise.
int32_t mpu6050_temp_reset(mpu6050_t *mpu6050);

/// To read raw temperature datafrom sensor register.
/// \param[in/out] mpu6050 Pointer to mpu6050 instance.
/// \param[out] temp Raw temperature reading.
/// \return STATUS_OK if success, STATUS_ERROR otherwise.
int32_t mpu6050_temp_readRaw(mpu6050_t *mpu6050, int16_t* temp);

/// To read temperature data, in degree Celcius, from sensor register.
/// \param[in/out] mpu6050 Pointer to mpu6050 instance.
/// \param[out] temp Temperature reading in degree Celcius.
/// \return STATUS_OK if success, STATUS_ERROR otherwise.
int32_t mpu6050_temp_read(mpu6050_t *mpu6050, float* temp);

/// To perform accelerometer self test as per section 4.1 page 9 of Register
/// Map and Description document. When this function returns, the accelerometer
/// will be disabled.
/// \param[in/out] mpu6050 Pointer to mpu6050 instance.
/// \return The result of self test. \ref mpu6050SelfTestResult
mpu6050SelfTestResult mpu6050_accel_selfTest(mpu6050_t *mpu6050);

/// To enable accelerometer, i.e putting it out of standby mode by clearing the
/// bits in REG_PWR_MGMT2. Only the axis with the bit 1 will be enabled, the
/// axis with bit 0 will remain unaltered, and will not be enabled/disabled.
/// \param[in/out] mpu6050 Pointer to mpu6050 instance.
/// \param[in] xyzMask Mask to independently enable accelerometer x, y, z-axis.
/// 	\ref mpu6050AccelGyroXYZ.
/// \return STATUS_OK if success, STATUS_ERROR otherwise.
int32_t mpu6050_accel_enable(mpu6050_t *mpu6050, mpu6050AccelGyroXYZ xyzMask);

/// To disable accelerometer, i.e putting it into standby mode by setting
/// the bits in REG_PWR_MGMT2. Only the axis with the bit 1 will be disabled, the
/// axis with bit 0 will remain unaltered, and will not be enabled/disabled.
/// \param[in/out] mpu6050 Pointer to mpu6050 instance.
/// \param[in] xyzMask Mask to independently disable accelerometer x, y, z-axis.
/// 	\ref mpu6050AccelGyroXYZ.
/// \return STATUS_OK if success, STATUS_ERROR otherwise.
int32_t mpu6050_accel_disable(mpu6050_t *mpu6050, mpu6050AccelGyroXYZ xyzMask);

/// To reset the accelerometer. This will revert the ADC signal path to their
/// power up configuration.
/// \param[in/out] mpu6050 Pointer to mpu6050 instance.
/// \return STATUS_OK if success, STATUS_ERROR otherwise.
int32_t mpu6050_accel_reset(mpu6050_t *mpu6050);

/// To configure the high pass filter for accelerometer. ACCEL_HPF configures
/// the DHPF available in the path leading to motion detectors (Free Fall,
/// Motion threshold, and Zero Motion). The high pass filter output is not
/// available to the data registers (see Figure in Section 8 of the MPU-6000/MPU-6050
/// Product Specification document).
/// \param[in/out] mpu6050 Pointer to mpu6050 instance.
/// param[in] mode The mode for high pass filter \ref mpu6050AccelHighPassFilter
/// \return STATUS_OK if success, STATUS_ERROR otherwise.
int32_t mpu6050_accel_setHpf(mpu6050_t *mpu6050, mpu6050AccelHighPassFilter mode);

/// To read raw accelerometer data for all x, y and z-axis from sensor register.
/// \param[in/out] mpu6050 Pointer to mpu6050 instance.
/// \param[out] xa Raw x-axis accelerometer reading.
/// \param[out] ya Raw y-axis accelerometer reading.
/// \param[out] za Raw z-axis accelerometer reading.
/// \return STATUS_OK if success, STATUS_ERROR otherwise.
int32_t mpu6050_accel_readRaw(mpu6050_t *mpu6050, int16_t* xa, int16_t* ya, int16_t* za);

/// To read accelerometer data for all x, y and z-axis, in unit of g,
/// from sensor register.
/// \param[in/out] mpu6050 Pointer to mpu6050 instance.
/// \param[out] xa x-axis accelerometer reading, in unit of g.
/// \param[out] yz y-axis accelerometer reading, in unit of g.
/// \param[out] za z-axis accelerometer reading, in unit of g.
/// \return STATUS_OK if success, STATUS_ERROR otherwise.
int32_t mpu6050_accel_read(mpu6050_t *mpu6050, float* xa, float* ya, float* za);

/// To perform gyroscope self test as per section 4.1 page 9 of Register
/// Map and Description document. When this function returns, the gyroscope
/// will be disabled.
/// \param[in/out] mpu6050 Pointer to mpu6050 instance.
/// \return The result of self test. \ref mpu6050SelfTestResult
mpu6050SelfTestResult mpu6050_gyro_selfTest(mpu6050_t *mpu6050);

/// To enable gyroscope, i.e putting  it out of standby mode by clearing the
/// bits in REG_PWR_MGMT2. Only the axis with the bit 1 will be enabled, the
/// axis with bit 0 will remain unaltered, and will not be enabled/disabled.
/// \param[in/out] mpu6050 Pointer to mpu6050 instance.
/// \param[in] xyzMask Mask to independently enable gyroscope x, y, z-axis.
/// 	\ref mpu6050AccelGyroXYZ.
/// \return STATUS_OK if success, STATUS_ERROR otherwise.
int32_t mpu6050_gyro_enable(mpu6050_t *mpu6050, mpu6050AccelGyroXYZ xyzMask);

/// To disable gyroscope, i.e putting it into standby mode by setting
/// the bits in REG_PWR_MGMT2. Only the axis with the bit 1 will be disabled, the
/// axis with bit 0 will remain unaltered, and will not be enabled/disabled.
/// \param[in/out] mpu6050 Pointer to mpu6050 instance.
/// \param[in] xyzMask Mask to independently disable gyroscope x, y, z-axis.
/// 	\ref mpu6050AccelGyroXYZ.
/// \return STATUS_OK if success, STATUS_ERROR otherwise.
int32_t mpu6050_gyro_disable(mpu6050_t *mpu6050, mpu6050AccelGyroXYZ xyzMask);

/// To reset the gyroscope. This will revert the ADC signal path to their
/// power up configuration.
/// \param[in/out] mpu6050 Pointer to mpu6050 instance.
/// \return STATUS_OK if success, STATUS_ERROR otherwise.
int32_t mpu6050_gyro_reset(mpu6050_t *mpu6050);

/// To read raw gyroscope data for all x, y and z-axis from sensor register.
/// \param[in/out] mpu6050 Pointer to mpu6050 instance.
/// \param[out] xg Raw x-axis gyroscope reading.
/// \param[out] yg Raw y-axis gyroscope reading.
/// \param[out] zg Raw z-axis gyroscope reading.
/// \return STATUS_OK if success, STATUS_ERROR otherwise.
int32_t mpu6050_gyro_readRaw(mpu6050_t *mpu6050, int16_t* xg, int16_t* yg, int16_t* zg);

/// To read gyroscope data for all x, y and z-axis, in unit of degree/second,
/// from sensor register.
/// \param[in/out] mpu6050 Pointer to mpu6050 instance.
/// \param[out] xg x-axis gyroscope reading, in unit of degree/second.
/// \param[out] yg y-axis gyroscope reading, in unit of degree/second.
/// \param[out] zg z-axis gyroscope reading, in unit of degree/second.
/// \return STATUS_OK if success, STATUS_ERROR otherwise.
int32_t mpu6050_gyro_read(mpu6050_t *mpu6050, float* xg, float* yg, float* zg);


/**
 * Temporary fix. Exposing direct access to module registers.
 */
int32_t mpu6050_updateReg(mpu6050_t *mpu6050, uint8_t regAddr, uint8_t mask, uint8_t data);

int32_t mpu6050_readRegN(mpu6050_t *mpu6050, uint8_t regStartAddr, uint8_t* const buffer, uint32_t len);

int32_t mpu6050_writeRegN(mpu6050_t *mpu6050, uint8_t regStartAddr, const uint8_t* const data, uint32_t len);



#ifdef __cplusplus
}
#endif

#endif
