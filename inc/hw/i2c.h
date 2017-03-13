/*
 * i2c.h
 *
 *  Created on: 19 Sep 2016
 *      Author: chiong
 *
 *  I2C wrapper for target dependent I2C middleware. Hardware initialisation and configuration
 *  are done external to this wrapper (in application layer). This interface is only to wrap
 *  the functionalities of different target dependent middleware hardware peripheral.
 *
 *  Currently NOT supporting:
 *  1. Non-blocking, using transfer complete callback function.
 */

#ifndef INC_I2C_H_
#define INC_I2C_H_

#include <stdint.h>

#if 0
typedef struct {
    /* Bit rate or clock rate in Hz. Different target implementation may
     * have pre-defined list of supported bit rate. In this case, the
     * actual bit rate will not be exactly the specified bit rate, but
     * will be one of the pre-defined bit rate that is the closest to the
     * specified bit rate. */
    uint32_t bitRate;
    /* Wait timeout, in number of ticks. Different target implementation
     * may have different tick duration or may not even implement this
     * timeout. */
    uint32_t transferTimeout;
} i2cCfg_t;

/**
 * @brief Initialise the I2C wrapper.
 * @details Must be call before using any I2C function in this wrapper.
 * @returns The number of available hardware I2C peripheral. Returns STATUS_ERROR if
 *      fails.
 */
int32_t i2c_open(void);

/**
 * @brief Configure the given I2C.
 * @param[in] i2cIdx Index for the hardware I2C to be configured. First
 *      index is 0, and must be less than the number returned by i2c_open().
 * @param[in] i2cCfg I2C configuration.
 * @returns STATUS_OK if succeed, STATUS_ERROR otherwise.
 */
int32_t i2c_config(uint8_t i2cIdx, const i2cCfg_t *cfg);

/**
 * @brief Clean up the I2C wrapper.
 * @details Must be called to release all resources on hardware I2C. Once this
 *      function has been called, no function in this wrapper can be called anymore.
 */
void i2c_close(void);

/**
 * @brief Set the I2C slave address for I2C transaction.
 * @param[in] *i2c Index for the hardware I2C to be configured. First
 *      index is 0, and must be less than the number returned by i2c_open().
 * @param[in] addr I2C slave address, excluding the read/write bit.
 * @returns STATUS_OK if succeed, STATUS_ERROR otherwise.
 */
int32_t i2c_select(i2c_t *i2c, uint16_t addr);


#endif

typedef struct {
	/* I2C index, start with 0. */
	uint8_t i2cIdx;
	/* I2C slave address. uint16_t to support for 10-bit addressing. Implementation that only
	 * support 7-bit addressing must typecast to uint8_t in implementation. */
	uint16_t slaveAddr;
} i2c_t;

/**
 * @brief Function to update the value of a single register with a new value.
 * @details Only the bit positions correspond to the bit mask that are set will
 * 		be updated with the new value. The rest will remain its existing value.
 * @param[in] i2c I2C instance for I2C communication.
 * @param[in] regAddr The address of the register of the I2C slave to update.
 * @param[in] mask Bit mask for the bit position to update. Only the bit positions
 * 		with bit = 1 will be updated. The rest will keep their existing values.
 * @param[in] data New value to update to the register.
 * @return STATUS_OK if succeed, STATUS_ERROR otherwise.
 */
int32_t i2c_updateReg(i2c_t *i2c, uint8_t regAddr, uint8_t mask, uint8_t data);

/**
 * @brief Function to read a number of bytes starting at the specified register address.
 * @param[in] i2c I2C instance for I2C communication.
 * @param[in] regStartAddr The address of the start/first (inclusive) register to read
 * 		from the I2C slave.
 * @param[in] buffer Buffer to store read data.
 * @param[in] len Number of bytes to read.
 * @return STATUS_OK if succeed, STATUS_ERROR otherwise.
 */
int32_t i2c_readRegN(i2c_t *i2c, uint8_t regStartAddr, uint8_t* const buffer, uint32_t len);

/**
 * @brief Function to write a number of bytes starting at the specified register address.
 * @param[in] i2c I2C instance for I2C communication.
 * @param[in] regStartAddr The address of the start/first (inclusive) register to write
 * 		to the I2C slave.
 * @param[in] data Data to be written.
 * @param[in] len Number of bytes to write.
 * @return STATUS_OK if succeed, STATUS_ERROR otherwise.
 */
int32_t i2c_writeRegN(i2c_t *i2c, uint8_t regStartAddr, const uint8_t* const data, uint32_t len);

/**
 * @brief Transfer a number of bytes to and from the selected I2C.
 * @details Allowed combinations are:
 *      1. txBuffer != NULL and rxBuffer != NULL: Perform I2C write. Once ALL
 *         I2C write has completed, perform I2C read.
 *      2. txBuffer != NULL and rxBuffer == NULL: Perform I2C write only.
 *      3. txBuffer == NULL and rxBuffer != NULL: Perform I2C read only.
 *      4. txBuffer == NULL and rxBuffer == NULL: Do nothing.
 *
 *      Likewise, the cases for wLen and rLen are the same as follows:
 *      1. wLen == 0 --> txBuffer == NULL
 *      2. wLen > 0  --> txBuffer != NULL
 *      3. rLen == 0 --> rxBuffer == NULL
 *      4. rLen > 0  --> rxBuffer != NULL
 *
 * @param[in] i2c I2C instance for I2C communication.
 * @param[out] rxBuffer Buffer to store data read from I2C. If NULL, I2C
 *      read is not performed.
 * @param[in] rLen Length, in bytes, to read from I2C. If 0, I2C read is not
 *      performed.
 * @param[in] txBuffer Buffer containing data to write to I2C. If NULL, I2C
 *      write is not performed.
 * @param[in] wLen Length, in bytes, to write to I2C. If 0, I2C write is not
 *      performed.
 * @returns STATUS_OK if succeed, STATUS_ERROR otherwise.
 */
int32_t i2c_transfer(i2c_t *i2c, uint8_t *rxBuffer, uint32_t rLen, const uint8_t *txBuffer, uint32_t wLen);

#endif  /* INC_I2C_H_ */
