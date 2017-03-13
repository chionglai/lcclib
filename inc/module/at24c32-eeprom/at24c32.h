

#ifndef AT24C32_H
#define AT24C32_H

#include <stdint.h>

#define	AT24C32_I2C_ADDR	0x50	// AT24C32 I2C slave address

#define	AT24C32_MAX_BYTE	4096

typedef struct {
	uint8_t i2cIdx;
} at24c32Cfg_t;

typedef struct at24c32_s at24c32_t;

/// To create a AT24C32 32K bit EEPROM instance.
/// \param[out] ppAt24c32 Address to store the newly created AT24C32 instance.
/// \param[in] cfg Configuration to create the AT24C32 instance.
/// \return STATUS_OK if successful creation. STATUS_ERROR* otherwise.
int32_t at24c32_create(at24c32_t **ppAt24c32, const at24c32Cfg_t *cfg);

/// To destroy a AT24C32 instance and release its resources.
/// \param[in/out] ppAt24c32 Address of AT24C32 instance to be destroyed.
int32_t at24c32_destroy(at24c32_t **ppAt24c32);

/** These 3 functions are the basic read and write for I2C supported module **/
/// To update masked bit(s) of the selected register address with the given value.
/// \param[in/out] at24c32 AT24C32 instance.
/// \param[in] regAddr The 16-bit register address to write value to.
/// \param[in] mask The mask specifying which bit to be updated with the given data.
/// \param[in] data The data byte to write to the specified register address.
/// \return STATUS_OK if register value is updated successfully. STATUS_ERROR* otherwise.
int32_t at24c32_updateReg(at24c32_t *at24c32, uint16_t regAddr, uint8_t mask, uint8_t data);

/// To read a number of byte starting from a given register address.
/// \param[in/out] at24c32 AT24C32 instance.
/// \param[in] regStartAddr The start of 16-bit register address to read values from.
/// \param[out] buffer A data buffer containing all the data read. The buffer must
///                    have sufficient length to contain all the read data.
/// \param[in] len The number of bytes to read.
/// \return STATUS_OK if read success. STATUS_ERROR* otherwise.
int32_t at24c32_readRegN(at24c32_t *at24c32, uint16_t regStartAddr, uint8_t* const buffer, uint32_t len);

/// To write a number of byte starting from a given register address.
/// \param[in/out] at24c32 AT24C32 instance.
/// \param[in] regStartAddr The start of 16-bit register address to read values from.
/// \param[in] buffer Data to be written. The data must have sufficient length as specified by len.
/// \param[in] len The number of bytes to write.
/// \return STATUS_OK if read success. STATUS_ERROR* otherwise.
int32_t at24c32_writeRegN(at24c32_t *at24c32, uint16_t regStartAddr, const uint8_t* const data, uint32_t len);

#endif
