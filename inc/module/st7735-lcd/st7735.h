/*
 * st7735.h
 *
 *  Created on: 8 Mar 2016
 *      Author: lcc
 *
 * For 1.8inch TFT LCD driven by ST7735. Although the pins are labeled as SDA, SCL, CS, it
 * does not follow any standard I2C and SPI convention.
 * 1. There is no I2C slave addressing,
 * 2. The SDA line acts as both (host -> LCD) and (LCD -> host),
 * 3. Chip select CS is active low for both command and data,
 * 4. For 4-line mode, AO line is LO for command, and HI for data,
 * 5. For my module, it is hardware configured to use 4-line serial bus, i.e.
 *      a. SCK = SPI clock
 *      b. SDA = MOSI line
 *      c. CS  = normal CS line
 *      d. AO  = DCX line
 *      e. RESET = Reset line, active low
 * 5. I think the module that I have is ST7735B
 */

#ifndef __ST7735_H_INCLUDED
#define __ST7735_H_INCLUDED

#include <stdint.h>
#include "hw/spi.h"
#include "portable/fxn.h"

/* System function commands */
#define ST7735_CMD_NOP          0x00    /*< No operation */
#define ST7735_CMD_SWRESET      0x01    /*< Software reset */
#define ST7735_CMD_RDDID        0x04    /*< Read display ID */
#define ST7735_CMD_RDDST        0x09    /*< Read display status */
#define ST7735_CMD_RDDPM        0x0A    /*< Read display power */
#define ST7735_CMD_RDDMADCTL    0x0B    /*< Read display */
#define ST7735_CMD_RDDCOLMOD    0x0C    /*< Read display pixel */
#define ST7735_CMD_RDDIM        0x0D    /*< Read display image */
#define ST7735_CMD_RDDSM        0x0E    /*< Read display signal */
#define ST7735_CMD_SLPIN        0x10    /*< Sleep in & booster off */
#define ST7735_CMD_SLPOUT       0x11    /*< Sleep out & booster off */
#define ST7735_CMD_PTLON        0x12    /*< Partial mode on */
#define ST7735_CMD_NORON        0x13    /*< Partial off (Normal */
#define ST7735_CMD_INVOFF       0x20    /*< Display inversion off */
#define ST7735_CMD_INVON        0x21    /*< Display inversion on */
#define ST7735_CMD_GAMSET       0x26    /*< Gamma curve select */
#define ST7735_CMD_DISPOFF      0x28    /*< Display off */
#define ST7735_CMD_DISPON       0x29    /*< Display on */
#define ST7735_CMD_CASET        0x2A    /*< Column address set */
#define ST7735_CMD_RASET        0x2B    /*< Row address set */
#define ST7735_CMD_RAMWR        0x2C    /*< Memory write */
#define ST7735_CMD_RGBSET       0x2D    /*< LUT for 4k, 65k, 262k colour */
#define ST7735_CMD_RAMRD        0x2E    /*< Memory read */
#define ST7735_CMD_PTLAR        0x30    /*< Partial start/end address set */
#define ST7735_CMD_TEOFF        0x34    /*< Tearing effect line off */
#define ST7735_CMD_TEON         0x35    /*< Tearing effect mode set & on */
#define ST7735_CMD_MADCTL       0x36    /*< Memory data access control */
#define ST7735_CMD_IDMOFF       0x38    /*< Idle mode off */
#define ST7735_CMD_IDMON        0x39    /*< Idle mode on */
#define ST7735_CMD_COLMOD       0x3A    /*< Interface pixel format */
#define ST7735_CMD_RDID1        0xDA    /*< Read ID1 */
#define ST7735_CMD_RDID2        0xDB    /*< Read ID2 */
#define ST7735_CMD_RDID3        0xDC    /*< Read ID3 */

/* Panel function commands */
#define ST7735_CMD_FRMCTR1      0xB1    /*< Frame rate control in normal mode (full colour) */
#define ST7735_CMD_FRMCTR2      0xB2    /*< In idle mode (8-colours) */
#define ST7735_CMD_FRMCTR3      0xB3    /*< In partial mode + full colour mode */
#define ST7735_CMD_INVCTR       0xB4    /*< Display inversion control */
#define ST7735_CMD_DISSET5      0xB6    /*< Display function setting */
#define ST7735_CMD_PWCTR1       0xC0    /*< Power control setting */
#define ST7735_CMD_PWCTR2       0xC1
#define ST7735_CMD_PWCTR3       0xC2    /*< In normal mode */
#define ST7735_CMD_PWCTR4       0xC3    /*< In idle mode (8-colours) */
#define ST7735_CMD_PWCTR5       0xC4    /*< In partial mode + full colour mode */
#define ST7735_CMD_VMCTR1       0xC5    /*< VCOM control 1 */
#define ST7735_CMD_VMOFCTR      0xC7    /*< Set VCOM offset control */
#define ST7735_CMD_WRID2        0xD1    /*< Set LCM version code */
#define ST7735_CMD_WRID3        0xD2    /*< Customer project code */
#define ST7735_CMD_PWCTR6       0xFC    /*< Power control in partial mode + idle mode */
#define ST7735_CMD_NVCTR1       0xD9    /*< EEPROM control status */
#define ST7735_CMD_NVCTR2       0xDE    /*< EEPROM read command */
#define ST7735_CMD_NVCTR3       0xDF    /*< EEPROM write command */
#define ST7735_CMD_GAMCTRP1     0xE0    /*< Gamma adjustment (+ polarity) */
#define ST7735_CMD_GAMCTRN1     0xE1    /*< Gamma adjustment (- polarity) */
#define ST7735_CMD_EXTCTRL      0xF0    /*< Extension command control */
#define ST7735_CMD_VCOM4L       0xFF    /*< Vcom 4 level control */

/* Bit position for panel layout */
/* Horizontal refresh order. 0: left to right, 1: right to left */
#define ST7735_PANEL_REFRESH_ORDER_HOR_pos  (2)
/* RGB order. 0: RGB, 1: BGR */
#define ST7735_PANEL_RGB_ORDER_pos          (3)
/* Vertical refresh order. 0: top to bottom, 1: bottom to top */
#define ST7735_PANEL_REFRESH_ORDER_VER_pos  (4)
/* X and Y position changed. 0: Normal, 1: Flipped on Y=X axis. */
#define ST7735_PANEL_EXCHANGE_XY_pos        (5)
/* Mirror X. 0: normal, 1: Flipped on x-axis */
#define ST7735_PANEL_MIRROR_X_pos           (6)
/* Mirror Y. 0: normal, 1: Flipped on y-axis */
#define ST7735_PANEL_MIRROR_Y_pos           (7)

#define ST7735_PANEL_ORIENT_NORMAL      0x00

/* These are the width (horizontal) and height (vertical) when using normal
 * orientation. If bit ST7735_PANEL_EXCHANGE_XY_pos (MV in datasheet) is set,
 * the horizontal and vertical need to be swapped.
 */
#define ST7735_WIDTH         	128
#define ST7735_HEIGHT         	160

#define ST7735_DEFAULT_GAMMA_P  \
    {0x0F, 0x0F, 0x1A, 0x0F, 0x18, 0x2F, 0x28, 0x20, \
     0x22, 0x1F, 0x1B, 0x23, 0x37, 0x00, 0x07, 0x02, \
     0x10}

#define ST7735_DEFAULT_GAMMA_N  \
    {0x0F, 0x1B, 0x0F, 0x17, 0x33, 0x2C, 0x29, 0x2E,    \
     0x30, 0x30, 0x39, 0x3F, 0x00, 0x07, 0x03, 0x10}

/**
 * @brief Macro to convert RGB data to 18-bit color format with
 * 		5-bit R, 6-bit G and 5-bit B.
 * @param[in] r 8-bit unsigned int for red.
 * @param[in] g 8-bit unsigned int for green.
 * @param[in] b 8-bit unsigned int for blue.
 * @returns Packed RGB data in 18-bit 5-6-5 color format.
 */
#define ST7735_rgb565(r, g, b)	((((r) & 0xF8) << 8) | (((g) & 0xFC) << 3) | ((b) >> 3))


/* Refer COLMOD command in datasheet */
typedef enum {
    ST7735_PANEL_COLOR_12_BIT = 0x03,
    ST7735_PANEL_COLOR_16_BIT = 0x05,
    ST7735_PANEL_COLOR_18_BIT = 0x06,
} st7735_color_t;

typedef enum {
    ST7735_STATUS_OK = 0,
    ST7735_STATUS_ERROR
} st7735_status_t;

typedef enum {
    ST7735_MODEL_B,
    ST7735_MODEL_R
} st7735_model_t;

typedef struct st7735_s st7735_t;

typedef struct {
	/* The model of ST7735 chip, see st7735_model_t. */
	st7735_model_t model;
	/* SPI index to be used. */
	uint8_t spiIdx;
	/* Chip select index to be used. */
    uint8_t csPin;
    /* GPIO pin to be used as data/command select line. */
    uint8_t dcxPin;
    /* Delay function to be used for the delay between some LCD init
     * instruction on SPI bus. The delay is necessary for some instruction
     * during startup. If NULL, there will be no delay and the LCD may not
     * be initialised properly. */
    fxnDelay_t delayMs;
} st7735Cfg_t;


#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialise ST7735 module.
 * @param[out] inst Pointer to st7735 instance.
 * @param[in] model The model of ST7735 chip, see st7735_model_t.
 * @param[in] cfg ST7735 configuration to create a new instance.
 * @return ST7355_STATUS_OK if success, ST7735_STATUS_ERROR otherwise.
 */
st7735_status_t st7735_create(st7735_t **inst, const st7735Cfg_t *cfg);


/**
 * @brief Write a single command to ST7735 module.
 * @param[in] inst Pointer to st7735 instance.
 * @param[in] cmd 1-byte ST7735 command.
 * @return ST7355_STATUS_OK if success, ST7735_STATUS_ERROR otherwise.
 */
st7735_status_t st7735_writeCommand(const st7735_t *inst, const uint8_t cmd);


/**
 * @brief Write data to ST7735 module.
 * @param[in] inst Pointer to st7735 instance.
 * @param[in] data Buffer containing data bytes to be written.
 * @param[in] count Number of data bytes to be written.
 * @return ST7355_STATUS_OK if success, ST7735_STATUS_ERROR otherwise.
 */
st7735_status_t st7735_writeData(const st7735_t *inst, const uint8_t* data, const uint32_t count);


/**
 * @brief Set the start and end address before any RGB data write.
 * @param[in] inst Pointer to st7735 instance.
 * @param[in] xStart Start column address.
 * @param[in] yStart Start row address.
 * @param[in] xEnd End column address.
 * @param[in] yEnd End row address.
 * @return ST7355_STATUS_OK if success, ST7735_STATUS_ERROR otherwise.
 */
st7735_status_t st7735_setAddress(const st7735_t *inst, uint16_t xStart, uint16_t yStart, uint16_t xEnd, uint16_t yEnd);


/**
 * @brief Set panel orientation settings.
 * @param[inout] inst Pointer to st7735 instance.
 * @param[in] newOrient New panel orientation with the appropriate bit set/clear,
 *      according to ST7735_PANEL_*_pos.
 * @return ST7355_STATUS_OK if success, ST7735_STATUS_ERROR otherwise.
 */
st7735_status_t st7735_panel_setOrientation(st7735_t *inst, const uint8_t newOrient);


/**
 * @brief Set panel color format settings.
 * @param[inout] inst Pointer to st7735 instance.
 * @param[in] newFmt New color format, see st7735_color_t.
 * @return ST7355_STATUS_OK if success, ST7735_STATUS_ERROR otherwise.
 */
st7735_status_t st7735_panel_setColorFmt(st7735_t *inst, const st7735_color_t newFmt);


/**
 * @brief Push the same color for a number of times to ST7735.
 * @param[in] inst Pointer to instance handler.
 * @param[in] color 24-bit RGB color.
 * @param[in] times Number of times/pixels to draw the color.
 */
st7735_status_t st7735_panel_pushColor(const st7735_t *inst, uint32_t color, uint32_t times);


/**
 * @brief Push an array of colors to ST7735.
 * @param[in] inst Pointer to instance handler.
 * @param[in] color Array of 24-bit RGB color.
 * @param[in] count Number of colors in array color.
 */
st7735_status_t st7735_panel_pushColorArray(const st7735_t *inst, uint32_t *color, uint32_t count);


/**
 * @brief Close ST7735 and all associated peripheral.
 * @param[in] ST7735 handle to be closed.
 * @return ST7355_STATUS_OK if success, ST7735_STATUS_ERROR otherwise.
 */
st7735_status_t st7735_destroy(st7735_t **inst);


#ifdef __cplusplus
}
#endif

#endif /* __ST7735_H_INCLUDED */
