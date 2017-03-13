

#ifndef HMC5883_H
#define HMC5883_H

#include <stdint.h>

#define	HMC5883_I2C_ADDR	0x1E	// HMC5883 I2C slave address

// Register addresses for HMC5883
#define HMC5883_REG_CRA 		0		/// Configuration register A
#define HMC5883_REG_CRB 		1		/// Configuration register B
#define HMC5883_REG_MR			2		/// Mode register
#define HMC5883_REG_DOX_MSB		3		/// Data output X MSB register
#define HMC5883_REG_DOX_LSB		4		/// Data output X LSB register
#define HMC5883_REG_DOZ_MSB		5		/// Data output Z MSB register
#define HMC5883_REG_DOZ_LSB		6		/// Data output Z LSB register
#define HMC5883_REG_DOY_MSB		7		/// Data output Y MSB register
#define HMC5883_REG_DOY_LSB 	8		/// Data output Y LSB register
#define HMC5883_REG_SR			9		/// Status register
#define HMC5883_REG_IRA			10		/// Identification register A
#define HMC5883_REG_IRB			11		/// Identification register B
#define HMC5883_REG_IRC			12		/// Identification register C

/// ADC value when there is overflow or underflow for a given channel X, Y or Z
#define HMC5883_VALUE_ERROR			-4096	

/// Mask and configuration defines for HMC5883_CRA register
#define HMC5883_MASK_MA		0x60
#define HMC5883_MASK_DOR	0x1C
#define HMC5883_MASK_MS		0x03

/// \brief hmc5883MAControl
/// Number of samples averaged per measurement output for HMC5883_CRA
typedef enum {
	HMC5883_MA_1 	= 0x00,		///< Averaged over 1 sample
	HMC5883_MA_2	= 0x20,		///<               2 
	HMC5883_MA_4	= 0x40,		///<               4
	HMC5883_MA_8	= 0x60		///<               8
} hmc5883MAControl;	

/// \brief hmc5883DORate
/// Data output rates for HMC5883_CRA
typedef enum {
	HMC5883_DOR_0_75	= 0x00,		///< 0.75Hz
	HMC5883_DOR_1_5		= 0x04,		///< 1.5Hz
	HMC5883_DOR_3		= 0x08,		///< 3Hz
	HMC5883_DOR_7_5		= 0x0C,		///< 7.5Hz
	HMC5883_DOR_15		= 0x10,		///< 15Hz
	HMC5883_DOR_30		= 0x14,		///< 30Hz
	HMC5883_DOR_75		= 0x18		///< 75Hz
} hmc5883DORate;

/// \brief hmc5883MSControl
/// Measurement mode for HMC5883_CRA
typedef enum {
	HMC5883_MS_NORMAL	= 0x00,		///< Normal measurement configuration
	HMC5883_MS_POS_BIAS	= 0x01,		///< Positive bias configuration for X, Y, Z axes
	HMC5883_MS_NEG_BIAS	= 0x02		///< Negative bias configuration for X, Y, Z axes
} hmc5883MSControl;


/// Mask and configuration settings for HMC5883_CRB register
#define HMC5883_MASK_GN		0xE0

/// \brief hmc5883GNControl
/// Gain settings for hmc5883. See Table 9,page 13 in datasheet for recommended gain settings.
typedef enum {
	HMC5883_GN_1370	= 0x00,		///< Actual Gauss = Digital value read / 1370
	HMC5883_GN_1090	= 0x20,		///< Actual Gauss = Digital value read / 1090
	HMC5883_GN_820	= 0x40,		///< Actual Gauss = Digital value read / 820
	HMC5883_GN_660	= 0x60,		///<                                   / 660
	HMC5883_GN_440	= 0x80,		///<                                   / 440
	HMC5883_GN_390	= 0xA0,		///<                                   / 390
	HMC5883_GN_330	= 0xC0,		///<                                   / 330
	HMC5883_GN_230	= 0xE0		///<                                   / 230
} hmc5883GNControl;


/// Mask and configuration settings for HMC5883_MR
#define HMC5883_MR_HS_MASK	0x80
#define HMC5883_MR_MD_MASK	0x03

/// \brief hmc5883MDSelect
/// Measurement mode selection for HMC5883_MR
typedef enum {
	HMC5883_MD_CONT		= 0x00,		///< Continuous mode
	HMC5883_MD_SINGLE	= 0x01,		///< Single measurement mode
	HMC5883_MD_IDLE		= 0x02		///< Idle mode
} hmc5883MDSelect;


///Defines for HMC5883_SR
#define HMC5883_SR_LOCK		0x02	///< Data output register lock
#define HMC5883_SR_DRDY		0x01	///< Data ready

typedef struct {
	/* Index of I2C peripheral to use. */
	uint8_t i2cIdx;
} hmc5883Cfg_t;

typedef struct hmc5883_s hmc5883_t;

/// To init the HMC5883 3-axis compass module
/// \param[out] ppHmc5883 Address to store the created hcm5883_t instance.
/// \param[in] cfg Configuration to create hcm5883_t instance.
/// \return STATUS_OK if successful initialisation. STATUS_ERROR otherwise.
int32_t hmc5883_create(hmc5883_t **ppHmc5883, const hmc5883Cfg_t *cfg);

/// To destroy the hcm5883_t instance and release all its resources.
/// \param[in/out] ppHmc5883 Address of hcm5883_t instance to be destroyed.
/// \return STATUS_OK if successful, STATUS_ERROR otherwise.
int32_t hcm5883_destroy(hmc5883_t **ppHmc5883);

/// To perform self test.
/// \param[in/out] hmc5883 HCM5883 instance.
/// \return STATUS_OK if self-test is passed. STATUS_ERROR otherwise.
int32_t hmc5883_selfTest(hmc5883_t *hmc5883);

/// To set the number of averaging sample. Can also use hmc5883_updateReg.
/// \param[in/out] hmc5883 HCM5883 instance.
/// \param[in] avg The number of averaging samples. \ref hmc5883MAControl
/// \return STATUS_OK if set is successful. STATUS_ERROR otherwise.
int32_t hmc5883_setAvgSample(hmc5883_t *hmc5883, hmc5883MAControl avg);

/// To set output data rate. Can also use hmc5883_updateReg.
/// \param[in/out] hmc5883 HCM5883 instance.
/// \param[in] rate The data output rate. \ref hmc5883DORate
/// \return STATUS_OK if set is successful. STATUS_ERROR otherwise.
int32_t hmc5883_setDataRate(hmc5883_t *hmc5883, hmc5883DORate rate);

/// To set the ADC gain. Can also use hmc5883_updateReg.
/// \param[in/out] hmc5883 HCM5883 instance.
/// \param[in] gain The ADC gain, either 0 <= gain <= 7 or using \ref hmc5883GNControl.
/// \return STATUS_OK if set is successful. STATUS_ERROR otherwise.
int32_t hmc5883_setGain(hmc5883_t *hmc5883, hmc5883GNControl gain);

/// To set the reading mode, which can be continuous measurement,
/// single measurement or idle. Can also use hmc5883_updateReg.
/// \param[in/out] hmc5883 HCM5883 instance.
/// \param[in] mode Operating mode. \ref hmc5883MDSelect
/// \return STATUS_OK if set is successful. STATUS_ERROR otherwise.
int32_t hmc5883_setMode(hmc5883_t *hmc5883, hmc5883MDSelect mode);

/// To get the ID of the HMC5883 module.
/// \param[in/out] hmc5883 HCM5883 instance.
/// \param[out] id The 3-bytes ID, where id[0]=IRA, id[1]=IRB
///             and id[2]=IRC. The buffer id must be 3 bytes long.
/// \return STATUS_OK if read is successful. STATUS_ERROR otherwise.
int32_t hmc5883_getId(hmc5883_t *hmc5883, uint8_t* const id);

/// Poll the status register to check if data is ready for read.
/// \param[in/out] hmc5883 HCM5883 instance.
/// \return 1 if data is ready, 0 otherwise. STATUS_ERROR is error occurs.
int32_t hmc5883_isDataReady(hmc5883_t *hmc5883);

/// To read the X, Y and Z raw value.
/// \param[in/out] hmc5883 HCM5883 instance.
/// \param[out] x The raw X value in integer.
/// \param[out] y The raw Y value in integer.
/// \param[out] z The raw Z value in integer.
/// \return STATUS_OK if read is successful. STATUS_ERROR otherwise.
int32_t hmc5883_read(hmc5883_t *hmc5883, int16_t* x, int16_t* y, int16_t* z);

/// To read the X, Y and Z Gauss value. The units are milli-Gauss.
/// \param[in/out] hmc5883 HCM5883 instance.
/// \param[out] x The Gauss value in X direction.
/// \param[out] y The Gauss value in Y direction.
/// \param[out] z The Gauss value in Z direction.
/// \return STATUS_OK if read is successful. STATUS_ERROR otherwise.
int32_t hmc5883_readGauss(hmc5883_t *hmc5883, float* x, float* y, float* z);

#endif
