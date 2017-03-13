/*
 * spi.h
 *
 *  Created on: 19 Sep 2016
 *      Author: chiong
 *
 *  SPI wrapper for target dependent SPI middleware. Hardware initialisation and configuration
 *  are done external to this wrapper (in application layer). This interface is only to wrap
 *  the functionalities of different target dependent middleware hardware peripheral.
 *
 *  Currently NOT supporting:
 *  1. Non-blocking, using transfer complete callback function.
 */

#ifndef INC_SPI_H_
#define INC_SPI_H_

#include <stdint.h>

#if 0
typedef enum {
    SPI_MASTER = 0,
    SPI_SLAVE  = 1
} spiMode_t;

typedef enum {
    SPI_POL0_PHA0 = 0,
    SPI_POL0_PHA1 = 1,
    SPI_POL1_PHA0 = 2,
    SPI_POL1_PHA1 = 3
} spiFrameFormat_t;

typedef enum {
	SPI_BIT_LSBFIRST = 0,
	SPI_BIT_MSBFIRST = 1
} spiBitOrder_t;

typedef struct {
	/* Base clock speed, in Hz. Used to determine clock divider. */
	uint32_t baseClock;
    /* Bit rate or clock rate in Hz. Different target implementation may
     * have pre-defined list of supported bit rate. In this case, the
     * actual bit rate will not be exactly the specified bit rate, but
     * will be one of the pre-defined bit rate that is the closest to the
     * specified bit rate. */
    uint32_t bitRate;
    /* Size, in number of bits for single data element. */
    uint32_t dataSize;
    /* Wait timeout, in number of ticks. Different target implementation
     * may have different tick duration or may not even implement this
     * timeout. */
    uint32_t transferTimeout;
    /* Either SPI master or slave. */
    spiMode_t mode;
    /* Specify the type/mode for data clocking. */
    spiFrameFormat_t frameFormat;
    /* Specify the order for data bit. */
    spiBitOrder_t bitOrder;
} spiCfg_t;

/**
 * @brief Initialise the SPI wrapper.
 * @details Must be call before using any SPI function in this wrapper.
 * @returns The number of available hardware SPI peripheral. Returns STATUS_ERROR if
 *      fails.
 */
int32_t spi_open(void);

/**
 * @brief Configure the given SPI.
 * @details By default, the CS associated with the spiIdx will be the same as the spiIdx
 * 		itself, i.e. SPI0 will use CS0 and SPI1 will use CS1 and the CSx will be active
 * 		LOW. To change this default behaviour, use spi_selectCs().
 * @param[in] spiIdx Index for the hardware SPI to be configured. First index is 0,
 *      and must be less than the number returned by spi_open();
 * @param[in] cfg SPI configuration struct.
 * @returns STATUS_OK if successfully configured, STATUS_ERROR otherwise.
 */
int32_t spi_config(uint8_t spiIdx, const spiCfg_t *cfg);

/**
 * @brief Clean up the SPI wrapper.
 * @details Must be called to release all resources on hardware SPI. Once this
 *      function has been called, no function in this wrapper can be called anymore.
 */
void spi_close(void);

#endif

/**
 * @brief Select the hardware chip select to be associated with the specified SPI index.
 * @details This function allows different hardware controlled CS to be re-routed to
 *      different hardware SPI. For targets that have fixed CS, e.g. CS0 for SPI0 and
 *      CS1 for SPI1, the parameter cs is ignored and the CS is determined from the SPI
 *      index, unless CS is negative.
 * @param[in] spiIdx Index for the hardware SPI to be configured. First index is 0,
 *      and must be less than the number returned by spi_open();
 * @param[in] cs Index for chip select. Start with 0. The actual CS pin that corresponds to
 *      CSx is internally determined in the target specific implementation. Negative cs
 *      means do not use hardware controlled CS and the CS line will be manually controlled
 *      by application, outside spi_transfer().
 * @param[in] active HIGH (1) for active high, LOW (0) for active low.
 * @returns STATUS_OK if successfully configured, STATUS_ERROR otherwise.
 */
int32_t spi_selectCs(uint8_t spiIdx, int8_t cs, uint8_t active);

/**
 * @brief Transfer a number of bytes to and from the selected SPI.
 * @details Allowed combinations of rxBuffer and txBuffer are:
 *      1. txBuffer != NULL and rxBuffer != NULL: Perform byte-by-byte
 *         write-read cycles.
 *      2. txBuffer != NULL and rxBuffer == NULL: Perform write only.
 *         No read.
 * @param[in] spiIdx Index for the hardware SPI to be configured. First
 *      index is 0, and must be less than the number returned by spi_open();
 * @param[out] rxBuffer Buffer to store the received byte from MISO line.
 *      If NULL, read from MISO will not be performed, i.e. only write to SPI.
 * @param[in] txBuffer Buffer containing data to be sent to MOSI line.
 *      Must not be NULL.
 * @param[in] length Size, in bytes, for both rxBuffer and txBuffer, i.e. the
 *      number of byte to send and receive.
 * @returns STATUS_OK if successfully configured, STATUS_ERROR otherwise.
 */
int32_t spi_transfer(uint8_t spiIdx, uint8_t *rxBuffer, const uint8_t *txBuffer, uint32_t length);

#endif /* INC_SPI_H_ */
