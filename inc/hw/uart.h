/*
 * uart.h
 *
 *  Created on: 19 Sep 2016
 *      Author: chiong
 *
 *  UART wrapper for target dependent UART middleware. Hardware initialisation and configuration
 *  are done external to this wrapper (in application layer). This interface is only to wrap
 *  the functionalities of different target dependent middleware hardware peripheral.
 *
 *  Currently NOT supporting:
 *  1. Non-blocking, using transfer complete callback function.
 */

#ifndef INC_UART_H_
#define INC_UART_H_

#include <stdint.h>

#if 0
typedef enum {
    UART_DATA_BIT_5 = 0,
    UART_DATA_BIT_6 = 1,
    UART_DATA_BIT_7 = 2,
    UART_DATA_BIT_8 = 3
} uartDataLength_t;

typedef enum {
    UART_STOP_BIT_1 = 0,
    UART_STOP_BIT_2 = 1
} uartStopBit_t;

typedef enum {
    /* No parity. */
    UART_PARITY_NONE = 0,
    /* Parity bit is even. */
    UART_PARITY_EVEN = 1,
    /* Parity bit is odd. */
    UART_PARITY_ODD  = 2,
    /* Parity bit is always zero. */
    UART_PARITY_ZERO = 3,
    /* Parity bit is always one. */
    UART_PARITY_ONE  = 4,
} uartParity_t;

typedef enum {
    /* Read/write is finished when the byte count is reached. */
    UART_RETURN_FULL    = 0,
    /* For read, read is finished when new line char is encountered. For write
     * a new line char is inserted at end of text. */
    UART_RETURN_NEWLINE = 1
} uartReturnMode_t;

typedef struct {
    uint32_t baudRate;
    uartDataLength_t dataLength;
    uartStopBit_t stopBits;
    uartParity_t parity;
    uartReturnMode_t readReturnMode;
    uartReturnMode_t writeReturnMode;
    /* Wait timeout, in number of ticks. Different target implementation
     * may have different tick duration or may not even implement this
     * timeout. */
    uint32_t transferTimeout;
} uartCfg_t;

/**
 * @brief Initialise the UART wrapper.
 * @details Must be call before using any UART function in this wrapper.
 * @returns The number of available hardware UART peripheral. Returns STATUS_ERROR if
 *      fails.
 */
int32_t uart_open(void);

/**
 * @brief Configure the given UART.
 * @param[in] uartIdx Index for the hardware UART to be configured. First
 *      index is 0, and must be less than the number returned by uart_open().
 * @param[in] uartCfg UART configuration.
 * @returns STATUS_OK if succeed, STATUS_ERROR otherwise.
 */
int32_t uart_config(uint8_t uartIdx, const uartCfg_t *cfg);

/**
 * @brief Clean up the UART wrapper.
 * @details Must be called to release all resources on hardware UART. Once this
 *      function has been called, no function in this wrapper can be called anymore.
 */
void uart_close(void);

#endif

/**
 * @brief Write to UART.
 * @param[in] uartIdx Index for the hardware UART to be configured. First
 *      index is 0, and must be less than the number returned by uart_open().
 * @param[in] buffer Buffer containing data to write to UART.
 * @param[in] len Length, in bytes, of data to write.
 * @returns Number of bytes written to UART, excluding the newline char, if
 *      UART_RETURN_NEWLINE is selected. STATUS_ERROR if error occurs.
 */
int32_t uart_write(uint8_t uartIdx, const uint8_t *buffer, uint32_t len);

/**
 * @brief Read from UART.
 * @details For UART_RETURN_NEWLINE, the newline char will be replaced by a
 *      null delimiter.
 * @param[in] uartIdx Index for the hardware UART to be configured. First
 *      index is 0, and must be less than the number returned by uart_open().
 * @param[in] buffer Buffer to store data read from UART.
 * @param[in] len Length, in bytes, of data to read.
 * @returns Number of bytes read from UART, excluding the newline char, if
 *      UART_RETURN_NEWLINE is selected. STATUS_ERROR if error occurs.
 */
int32_t uart_read(uint8_t uartIdx, uint8_t *buffer, uint32_t len);

#endif  /* INC_UART_H_ */
