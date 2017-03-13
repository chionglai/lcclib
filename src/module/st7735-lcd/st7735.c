/*
 * st7735.c
 *
 *  Created on: 8 Mar 2016
 *      Author: lcc
 */

#include <stdlib.h>
#include "util/bit.h"
#include "hw/spi.h"
#include "hw/gpio.h"
#include "portable/fxn.h"
#include "module/st7735-lcd/st7735.h"

#define DATA_BUFFER_SIZE    16
#define DELAY				0x80

struct st7735_s {
	uint8_t spiIdx;
    uint8_t csPin;
    uint8_t dcxPin;
    st7735_color_t colorFmt;
};

/* Initialisation data */
static const uint8_t dataB[] = {	/* Initialisation commands for ST7735B panel */
	13,								/* Total of 9 commands */
	ST7735_CMD_SWRESET, DELAY,		/* SW reset, no args, with delay */
	50,								/* 50 ms delay before next command */
	ST7735_CMD_SLPOUT, DELAY,		/* Get out of sleep mode */
	150,
	ST7735_CMD_FRMCTR1, 3,			/* Frame rate control, 3 args */
	0x00,							/* Fastest refresh rate */
	0x06,							/* 6 lines front porch */
	0x03,							/* 3 lines back porch */
	ST7735_CMD_FRMCTR2, 3,			/* Frame rate control for idle mode, 3 args */
	0x00,							/* Same settings */
	0x06,
	0x03,
	ST7735_CMD_FRMCTR3, 6,
	0x00,							/* Same settings */
	0x06,
	0x03,
	0x00,							/* Same settings */
	0x06,
	0x03,
	ST7735_CMD_INVCTR, 1,			/** Do not invert display */
	0x00,
	ST7735_CMD_INVOFF, 0,
	ST7735_CMD_MADCTL , 1,			//  5: Memory access ctrl (directions), 1 arg:
	0x08,							//     Row addr/col addr, bottom to top refresh
	ST7735_CMD_DISSET5, 2,			//  6: Display settings #5, 2 args, no delay:
	0x15,							//     1 clk cycle nonoverlap, 2 cycle gate
									//     rise, 3 cycle osc equalize
	0x02,                   		//     Fix on VTL

/*
 *  // Power control for 5V use case
	ST7735_CMD_PWCTR2 , 1,			//  9: Power control, 1 arg, no delay:
	0x05,							//     VGH = 14.7V, VGL = -7.35V
	ST7735_CMD_PWCTR3 , 2,			// 10: Power control, 2 args, no delay:
	0x01,							//     Opamp current small
	0x02,							//     Boost frequency
	ST7735_CMD_VMCTR1 , 2 + DELAY,	// 11: Power control, 2 args + delay:
	0x3C,							//     VCOMH = 4V
	0x38,							//     VCOML = -1.1V
	10,								//     10 ms delay
	ST7735_CMD_PWCTR6 , 2, 			// 12: Power control, 2 args, no delay:
	0x11, 0x15,
*/
	ST7735_CMD_GAMCTRP1, 16,		// 13: Magical unicorn dust, 16 args, no delay:
	0x09, 0x16, 0x09, 0x20,			//     (seriously though, not sure what
	0x21, 0x1B, 0x13, 0x19,			//      these config values represent)
	0x17, 0x15, 0x1E, 0x2B,
	0x04, 0x05, 0x02, 0x0E,
	ST7735_CMD_GAMCTRN1, 16 + DELAY,// 14: Sparkles and rainbows, 16 args + delay:
	0x0B, 0x14, 0x08, 0x1E,			//     (ditto)
	0x22, 0x1D, 0x18, 0x1E,
	0x1B, 0x1A, 0x24, 0x2B,
	0x06, 0x06, 0x02, 0x0F,
	10,								//     10 ms delay
	ST7735_CMD_NORON, DELAY,		/* Normal display on */
	10,
	ST7735_CMD_DISPON, DELAY,		/* Turn on main screen */
	120
};

#if 0 // unused
    /* Power control, using default values */
    st7735_writeCommand(inst, ST7735_CMD_PWCTR1);
    buffer[0] = 0x82;
    buffer[1] = 0x02;
    buffer[2] = 0x84;
    st7735_writeData(inst, buffer, 3);
    st7735_writeCommand(inst, ST7735_CMD_PWCTR2);
    buffer[0] = 0xC5;
    st7735_writeData(inst, buffer, 1);
    st7735_writeCommand(inst, ST7735_CMD_PWCTR3);
    buffer[0] = 0x0A;
    buffer[1] = 0x00;
    st7735_writeData(inst, buffer, 2);
    st7735_writeCommand(inst, ST7735_CMD_PWCTR4);
    buffer[0] = 0x8A;
    buffer[1] = 0x2E;
    st7735_writeData(inst, buffer, 2);
    st7735_writeCommand(inst, ST7735_CMD_PWCTR5);
    buffer[0] = 0x8A;
    buffer[1] = 0xAA;
    st7735_writeData(inst, buffer, 2);
#endif

/* Internal functions. */
/**
 * @brief Initialise ST7735 LCD panel.
 * @param[in] inst ST7735 handle.
 * @param[in] data Array of initialisation command + data.
 */
st7735_status_t st7735_initModel(st7735_t *inst, const uint8_t *data, fxnDelay_t delayFxn);


st7735_status_t st7735_create(st7735_t **inst, const st7735Cfg_t *cfg) {
    st7735_t *pInst;
	st7735_status_t status = ST7735_STATUS_ERROR;

    pInst = (st7735_t*) malloc(sizeof(st7735_t));
    if (NULL == pInst) {
    	return ST7735_STATUS_ERROR;
    }
    *inst = pInst;
    pInst->spiIdx = cfg->spiIdx;
    pInst->csPin = cfg->csPin;
    pInst->dcxPin = cfg->dcxPin;

    switch(cfg->model) {
        case ST7735_MODEL_B:
            status = st7735_initModel(pInst, dataB, cfg->delayMs);
            break;

        case ST7735_MODEL_R:
            /* Not supported yet */
            break;

        default:
            /* Do nothing */
            break;
    }
    return status;
}


st7735_status_t st7735_initModel(st7735_t *inst, const uint8_t *data, fxnDelay_t delayFxn) {
	uint8_t ncmd;
	uint8_t narg;
	uint8_t delay;
	uint8_t hasDelay;
	const uint8_t *p;

	p = data;
	ncmd = *p++;

	while(ncmd--) {
		st7735_writeCommand(inst, *p++);
		hasDelay = *p & DELAY;
		narg = *p++ & ~DELAY;
		st7735_writeData(inst, p, narg);
		p += narg;

		if (hasDelay && delayFxn) {
			delay = *p++;
			delayFxn(delay);
		}
	}

    st7735_panel_setOrientation(inst, ST7735_PANEL_ORIENT_NORMAL);
    st7735_panel_setColorFmt(inst, ST7735_PANEL_COLOR_18_BIT);

    return ST7735_STATUS_OK;
}


st7735_status_t st7735_writeCommand(const st7735_t *inst, const uint8_t cmd) {
    /* Set DCX to LO for command. */
    gpio_write(inst->dcxPin, LOW);

    spi_selectCs(inst->spiIdx, inst->csPin, LOW);
    spi_transfer(inst->spiIdx, NULL, &cmd, 1);
    return ST7735_STATUS_OK;
}


st7735_status_t st7735_writeData(const st7735_t *inst, const uint8_t* data, const uint32_t count) {
    /* Set DCX to HI for data. */
    gpio_write(inst->dcxPin, HIGH);

    spi_selectCs(inst->spiIdx, inst->csPin, LOW);
    spi_transfer(inst->spiIdx, NULL, data, count);
    return ST7735_STATUS_OK;
}


st7735_status_t st7735_setAddress(const st7735_t *inst, uint16_t xStart, uint16_t yStart, uint16_t xEnd, uint16_t yEnd) {
    st7735_status_t status;
    uint8_t buffer[4];

    status = st7735_writeCommand(inst, ST7735_CMD_CASET);
    buffer[0] = (xStart >> 8) & 0xFF;
    buffer[1] = xStart & 0xFF;
    buffer[2] = (xEnd >> 8) & 0xFF;
    buffer[3] = xEnd & 0xFF;
    status = st7735_writeData(inst, buffer, 4);

    status = st7735_writeCommand(inst, ST7735_CMD_RASET);
    buffer[0] = (yStart >> 8) & 0xFF;
    buffer[1] = yStart & 0xFF;
    buffer[2] = (yEnd >> 8) & 0xFF;
    buffer[3] = yEnd & 0xFF;
    status = st7735_writeData(inst, buffer, 4);

    return status;
}


st7735_status_t st7735_destroy(st7735_t **inst) {
    st7735_t *p;

    p = *inst;
    if (NULL != p) {
    	free(p);
    	*inst = NULL;
    }

    return ST7735_STATUS_OK;
}


st7735_status_t st7735_panel_setOrientation(st7735_t *inst, const uint8_t newOrient) {
    // decide to get rid of error checking since it blows up the code
    st7735_writeCommand(inst, ST7735_CMD_MADCTL);
    st7735_writeData(inst, (const uint8_t*)(&newOrient), 1);

    return ST7735_STATUS_OK;
}


st7735_status_t st7735_panel_setColorFmt(st7735_t *inst, const st7735_color_t newFmt) {
    st7735_status_t status;

    inst->colorFmt = newFmt;
    status = st7735_writeCommand(inst, ST7735_CMD_COLMOD);

    if (status == ST7735_STATUS_OK) {
        status = st7735_writeData(inst, (const uint8_t*)&newFmt, 1);
    }

    return status;
}


st7735_status_t st7735_panel_pushColorArray(const st7735_t *inst, uint32_t *color, uint32_t count) {
	uint8_t buffer[3];
	uint32_t i;
	uint16_t color16;
	st7735_status_t status;

	status = st7735_writeCommand(inst, ST7735_CMD_RAMWR);
	if (ST7735_STATUS_OK == status) {
		switch(inst->colorFmt) {
		case ST7735_PANEL_COLOR_12_BIT:
			/* Not supported yet */
			break;
		case ST7735_PANEL_COLOR_16_BIT:
			for (i = 0; i < count; i++) {
				color16 = ST7735_rgb565((color[i] >> 16) & 0xFF, (color[i] >> 8) & 0xFF, color[i] & 0xFF);
				buffer[0] = color16 >> 8;
				buffer[1] = color16 & 0xFF;
				status = st7735_writeData(inst, buffer, 2);
			}
			break;

		case ST7735_PANEL_COLOR_18_BIT:
			for (i = 0; i < count; i++) {
				buffer[0] = (color[i] >> 16) & 0xFF;
				buffer[1] = (color[i] >>  8) & 0xFF;
				buffer[2] = (color[i]      ) & 0xFF;
				status = st7735_writeData(inst, buffer, 3);
			}
			break;
		default:
			/* Do nothing */
			break;
		}
	}

	return status;
}


st7735_status_t st7735_panel_pushColor(const st7735_t *inst, uint32_t color, uint32_t times) {
    st7735_status_t status = ST7735_STATUS_ERROR;
    uint8_t buffer[3];
    uint32_t i;
    uint16_t color16;

    status = st7735_writeCommand(inst, ST7735_CMD_RAMWR);
    switch(inst->colorFmt) {
        case ST7735_PANEL_COLOR_12_BIT:
            /* Not supported yet */
            break;
        case ST7735_PANEL_COLOR_16_BIT:
            color16 = ST7735_rgb565((color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF);
            buffer[0] = color16 >> 8;
            buffer[1] = color16 & 0xFF;
            for (i = 0; i < times; i++) {
            	status = st7735_writeData(inst, buffer, 2);
            }
            break;

        case ST7735_PANEL_COLOR_18_BIT:
            buffer[0] = (color >> 16) & 0xFF;
            buffer[1] = (color >>  8) & 0xFF;
            buffer[2] = (color      ) & 0xFF;
            for (i = 0; i < times; i++) {
                status = st7735_writeData(inst, buffer, 3);
            }
            break;
        default:
            /* Do nothing */
            break;
    }

    return status;
}
