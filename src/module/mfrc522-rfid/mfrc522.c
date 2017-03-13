/*
 * mrfc522.c
 *
 *  Created on: 06/06/2013
 *      Author: lcc
 */

#include <stdlib.h>
#include "util/status.h"
#include "util/xtype.h"
#include "hw/spi.h"
#include "module/mfrc522-rfid/mfrc522.h"

struct mfrc522_s {
	/* SPI index used for communication to mfrc522. */
	uint8_t spiIdx;
	/* CS index used for communication to mfrc522. */
	uint8_t csIdx;
};

/*
 * Note:
 *
 * 1. Although the MFRC522 itself support I2C, SPI and UART communication
 *    protocols, the module I purchased only support SPI because the I2C
 *    pin is hardwired to GND (LOW) and EA pin is hardwired to 3.3V (HI).
 *    This configuration allows only the SPI.
 *
 * 2. Other settings such as Rx CRC check, Tx CRC append, Rx speed and Tx
 *    speed need to be set separately since they are dependent on type of
 *    cards and commands.
 */
int32_t mfrc522_create(mfrc522_t **ppSelf, const mfrc522Cfg_t *cfg) {
	mfrc522_t *self;
	uint8_t buffer;

	self = (mfrc522_t*) malloc(sizeof(mfrc522_t));
	if (NULL == self) {
		return STATUS_ERROR_MALLOC;
	}

	self->spiIdx = cfg->spiIdx;
	self->csIdx = cfg->csIdx;

	// reset module so its registers are back to default values
	mfrc522_sendCmd(self, MFRC522_CMD_RESET, MFRC522_CMD_NO_BLOCKING);

	// power up the module
	mfrc522_updateReg(
			self,
			MFRC522_REG_COMMAND,
			MFRC522_POWER_DOWN|MFRC522_MASK_CMD,
			MFRC522_CMD_NOP);

	// wait till module is ready
	do {
		buffer = MFRC522_REG_COMMAND;
		mfrc522_readRegN(self, &buffer, 1);
	} while((buffer & MFRC522_POWER_DOWN) != 0);

	// setting all required register for Tx and Rx
	mfrc522_tx_setEnable(self, 1);	// turn on tx antenna
	mfrc522_rx_setEnable(self, 1);	// turn on receiver circuit
	// force 100% ASK
	mfrc522_updateReg(self, MFRC522_REG_TX_ASK, MFRC522_FORCE_100ASK, MFRC522_FORCE_100ASK);

	// set timer to tick once every 100us
	mfrc522_updateReg(self, MFRC522_REG_DEMOD, MFRC522_TPRESCAL_EVEN, MFRC522_TPRESCAL_EVEN);
	mfrc522_timer_setPrescale(self, 677);

	// set CRC preset to 6363 as required by ISO/IEC1443-3
	mfrc522_crc_setPreset(self, MFRC522_CRC_PRESET_6363H);

	// set optional settings
	mfrc522_updateReg(self, MFRC522_REG_RF_CFG, MFRC522_MASK_RX_GAIN, MFRC522_RX_GAIN_48DB);

	*ppSelf = self;

	return STATUS_OK;
}


void mfrc522_destroy(mfrc522_t **ppSelf) {
	uint8_t buffer;
	mfrc522_t *self = *ppSelf;

	mfrc522_tx_setEnable(self, 0);	// turn on tx antenna
	mfrc522_rx_setEnable(self, 0);	// turn on receiver circuit
	buffer = MFRC522_POWER_DOWN | MFRC522_CMD_NOP;
	mfrc522_writeRegN(self, MFRC522_REG_COMMAND, &buffer, 1);
	if (NULL != self) {
		free(self);
		*ppSelf = NULL;
	}
}


uint8_t mfrc522_authMifare(mfrc522_t *self, mfrc522MifareKeyType type, uint8_t blockAddr, const uint8_t* const key, const uint8_t* const serial) {
	uint16_t status;
	uint8_t buffer;
	uint8_t keyType;

	// flush FIFO
	mfrc522_fifo_flush(self);

	// put all data to FIFO, in order
	keyType = (uint8_t) type;
	mfrc522_fifo_write(self, &keyType, 1);	// Authentication key type: key A or key B
	mfrc522_fifo_write(self, &blockAddr, 1);	// Block address on Mifare card to be accessed for R/W
	mfrc522_fifo_write(self, key, 6);		// User supplied 6-byte sector key
	mfrc522_fifo_write(self, serial, 4);	// 4-byte serial number of Mifare card

	// stop timer and set its reload value
	mfrc522_timer_stop(self);
	mfrc522_timer_loadValue(self, TIMEOUT);

	// disable, clear and enable IRQ_IDLE, IRQ_ERR and IRQ_TIMER
	mfrc522_irq_disable(self, MFRC522_IRQ_IDLE|MFRC522_IRQ_TIMER);
	mfrc522_irq_clear(self, MFRC522_IRQ_IDLE|MFRC522_IRQ_TIMER);
	mfrc522_irq_enable(self, MFRC522_IRQ_IDLE|MFRC522_IRQ_TIMER);

	// set timer to auto start. For some unknown reason, timer auto start
	// not working. Maybe because sending of Mifare authentication data fails
	// and does not trigger the timer start
//	mfrc522_updateReg(self, MFRC522_REG_TMODE, MFRC522_TAUTO, MFRC522_TAUTO);

	// clear MF_CRYPTO1_ON
	mfrc522_updateReg(self, MFRC522_REG_STATUS2, MFRC522_MF_CRYPTO1_ON, 0x00);

	// start Mifare authentication using command MFAuthent
	mfrc522_sendCmd(self, MFRC522_CMD_MF_AUTH, MFRC522_CMD_NO_BLOCKING);

	// start timer manually instead
	mfrc522_timer_start(self);

	// wait for Timer or Idle irq
	do {
		status = mfrc522_irq_status(self);
	} while ((status & (MFRC522_IRQ_IDLE|MFRC522_IRQ_TIMER)) == 0);

	// stop timer
	mfrc522_timer_stop(self);

	// set timer back to manual
	mfrc522_updateReg(self, MFRC522_REG_TMODE, MFRC522_TAUTO, 0x00);

	if ((status & MFRC522_IRQ_TIMER) != 0) {	// timeout occur
		// send Idle command to stop MFAuthent command
		mfrc522_sendCmd(self, MFRC522_CMD_IDLE, MFRC522_CMD_NO_BLOCKING);

		// read Status2Reg, for debug only
		buffer = MFRC522_REG_STATUS2;
		mfrc522_readRegN(self, &buffer, 1);

		// read ErrorReg
		buffer = MFRC522_REG_ERROR;
		mfrc522_readRegN(self, &buffer, 1);

		if ((buffer & MFRC522_ERR_PROTOCOL) != 0) {
			return MFRC522_MIFARE_PROTOCOL_ERROR;
		} else {
			return MFRC522_MIFARE_AUTH_TIMEOUT;
		}
	} else {	// if MFAuthent command successfully/automatically terminated without error and timeout
	 	// read Status2Reg
	 	buffer = MFRC522_REG_STATUS2;
	 	mfrc522_readRegN(self, &buffer, 1);

	 	// if MFCryto1On == 1, Mifare successfully authenticated
		if ((buffer & MFRC522_MF_CRYPTO1_ON) != 0) {
	 		return MFRC522_MIFARE_AUTH_SUCCESS;
		} else {
	 		// will never reach this point, since MFAuthent command only
			// terminates when Mifare card is authenticated AND MFCrypto1On
			// bit is set to 1
			return MFRC522_MIFARE_UNKNOWN_ERROR;
		}
	}
	// if Idle irq (success)
	// 		read Status2Reg
	//		if MFCrypto1On == 1, return Success
	//		else return Fail
	// else (timeout)
	//		send Idle command,
	//		read Status2reg?? read ErrorReg
	//		ProtocolErr == 1, return Protocol error
	//		BufferOvflo error
	//		CRCError
	//		Parity error
	//		collision error
	//		else return timeout
}


uint8_t mfrc522_transceive(mfrc522_t *self, uint8_t* const data, uint32_t txBitLen, uint32_t* const rxBitLen) {
	uint8_t temp;
	uint16_t irqStatus;
	uint16_t irqFlag;
	int32_t rxIdx;
	uint8_t status;

	// set Irq monitor, i.e. which Irq to monitor
	irqFlag = MFRC522_IRQ_RX | MFRC522_IRQ_IDLE | MFRC522_IRQ_ERR | MFRC522_IRQ_TIMER;

	// set TxLastBits
	temp = txBitLen & MFRC522_MASK_TX_LAST_BIT;
	mfrc522_updateReg(
			self,
			MFRC522_REG_BIT_FRAMING,
			MFRC522_MASK_TX_LAST_BIT,
			temp);

	// set command to Idle
	mfrc522_sendCmd(self, MFRC522_CMD_IDLE, MFRC522_CMD_NO_BLOCKING);

	// flush fifo buffer
	mfrc522_fifo_flush(self);

	// put tx data to fifo
	if (temp == 0)
		mfrc522_fifo_write(self, data, (txBitLen >> 3));
	else
		mfrc522_fifo_write(self, data, (txBitLen >> 3) + 1);

	// load timer value
	mfrc522_timer_loadValue(self, TIMEOUT);

	// clear irq.
	mfrc522_irq_clear(self, irqFlag);

	// turn on TX antenna. Cannot turn on and off TX antenna only when needed.
	// It will break the communication between MFRC522 and card.
//	mfrc522_tx_setEnable(self, 1);

	// set command to transceive and power up receiver circuit
	mfrc522_updateReg(self, MFRC522_REG_COMMAND, MFRC522_MASK_CMD, MFRC522_CMD_TXRX);

	// set StartSend bit
	mfrc522_updateReg(self, MFRC522_REG_BIT_FRAMING, MFRC522_START_SEND, MFRC522_START_SEND);

	// start timer
	mfrc522_timer_start(self);

	// reset Rx bit count
	rxIdx = 0;

	// wait till data transfer completes, or error occurs
	do {
		// get ModemState
		temp = MFRC522_REG_STATUS2;
		mfrc522_readRegN(self, &temp, 1);
		temp &= MFRC522_MASK_MODEM_STATE;

		switch(temp) {
		case MFRC522_MODEM_STATE_RX:
/*
			// NOTE: Not a good idea as it may cause fifo buffer to change and affect
			// transmission

			// while MFRC522 is still receiving data, put Rx data from fifo to array.
			// this is to avoid fifo overflow

			//read FIFOLevel
			temp = MFRC522_REG_FIFO_LEVEL;
			mfrc522_readRegN(&temp, 1);
			temp &= MFRC522_MASK_FIFO_LEVEL;	// just in case

			// only put Rx data into array if there is a complete byte
			if (temp > 1) {
				temp--;
				mfrc522_fifo_read(&(data[rxIdx]), temp);
				rxIdx += temp;
			}
*/
			// intentionally no break at end of this case
		case MFRC522_MODEM_STATE_TX:
			// reset timer
			mfrc522_timer_loadValue(self, TIMEOUT);
			break;
		}

		// read Irq status
		irqStatus = mfrc522_irq_status(self);
	} while ((irqStatus & irqFlag) == 0);	// wait for RxIrq, IdleIrq, ErrIrq or TimerIrq

	// stop timer
	mfrc522_timer_stop(self);

	// clear StartSend
	mfrc522_updateReg(self, MFRC522_REG_BIT_FRAMING, MFRC522_START_SEND, 0x00);

	// turn off TX antenna
//	mfrc522_tx_setEnable(self, 0);

	if ((irqStatus & MFRC522_IRQ_ERR) != 0) {	// errors occurs
		// read Error register
		temp = MFRC522_REG_ERROR;
		mfrc522_readRegN(self, &temp, 1);

		data[0] = temp;
		status = MFRC522_TXRX_ERROR;
/*
		if ((temp & MFRC522_ERR_BUFFER_OVFL) != 0) {
			status = MFRC522_TXRX_FIFO_OVERFLOW;
		} else if ((temp & MFRC522_ERR_CRC) != 0) {
			status = MFRC522_TXRX_CRC_ERROR;
		} else if ((temp & MFRC522_ERR_COLL) != 0) {
			status = MFRC522_TXRX_COLL_ERROR;
		} else if ((temp & MFRC522_ERR_PARITY) != 0) {
			status = MFRC522_TXRX_PARITY_ERROR;
		} else {
			status = MFRC522_TXRX_UNKNOWN_ERROR;
		}
*/
	} else if ((irqStatus & MFRC522_IRQ_TIMER) != 0) {	// Tx/Rx timeout
		status = MFRC522_TXRX_TIMEOUT;
	} else {	// Tx/Rx success
		// put remaining data from fifo to array
		// read FIFOLevel
		temp = MFRC522_REG_FIFO_LEVEL;
		mfrc522_readRegN(self, &temp, 1);
		temp &= MFRC522_MASK_FIFO_LEVEL;

		mfrc522_fifo_read(self, &(data[rxIdx]), temp);
		rxIdx += temp;

		*rxBitLen = rxIdx << 3;	// multiply by 8

		// read RxLastBits
		temp = MFRC522_REG_CONTROL;
		mfrc522_readRegN(self, &temp, 1);
		temp &= MFRC522_MASK_RX_LAST_BIT;

		if (temp != 0) {
			*rxBitLen = *rxBitLen + temp;
			mfrc522_fifo_read(self, &(data[rxIdx]), 1);	// read last non-full byte
		}

		status = MFRC522_TXRX_OK;
	}

	// set command back to Idle. It is set here because command execution will reset ErrorReg
	mfrc522_updateReg(self, MFRC522_REG_COMMAND, MFRC522_MASK_CMD, MFRC522_CMD_IDLE);

	return status;
}


uint8_t mfrc522_getVersion(mfrc522_t *self) {
	uint8_t buffer;

	buffer = MFRC522_REG_VERSION;
	mfrc522_readRegN(self, &buffer, 1);

	return buffer;
}


/// The step to perform self test is available in the datasheet,
/// section 16.1.1, pg 81
void mfrc522_selfTest(mfrc522_t *self, uint8_t* const buffer) {
	int32_t i;
	uint8_t prevCmd;
	uint8_t prevTest;
	uint8_t temp;

	// 0. Get current COMMAND register value to be restored later
	prevCmd = MFRC522_REG_COMMAND;
	mfrc522_readRegN(self, &prevCmd, 1);
	
	// 1. Perform soft reset using SoftReset command.
	temp = MFRC522_CMD_RESET;
	mfrc522_writeRegN(self, MFRC522_REG_COMMAND, &temp, 1);

	// 2. Clear internal buffer by writing 25 bytes of 00h using MEM command
	for (i = 0; i < 64; i++) {
		buffer[i] = 0;
	}
	mfrc522_fifo_write(self, buffer, 25);
	temp = MFRC522_CMD_MEM;
	mfrc522_writeRegN(self, MFRC522_REG_COMMAND, &temp, 1);

	// 3a. read AutoTestReg value to be restored later
	prevTest = MFRC522_REG_AUTO_TEST;
	mfrc522_readRegN(self, &prevTest, 1);

	// 3b. Write 09h to AutoTestReg to enable self test
	temp = MFRC522_SELF_TEST;
	mfrc522_writeRegN(self, MFRC522_REG_AUTO_TEST, &temp, 1);

	// 4. Write 00h to FIFO buffer
	mfrc522_fifo_write(self, buffer, 1);

	// 5. Use CalcCRC to start self test
	temp = MFRC522_CMD_CALC_CRC;
	mfrc522_writeRegN(self, MFRC522_REG_COMMAND, &temp, 1);

	// 6. Wait for self test to finish
	do {
		temp = MFRC522_REG_COMMAND;
		mfrc522_readRegN(self, &temp, 1);
	} while ((temp & MFRC522_MASK_CMD) == MFRC522_CMD_CALC_CRC);

	// 7. Once finished, read all 64 bytes from FIFO buffer. Refer to datasheet for their values
	mfrc522_fifo_read(self, buffer, 64);

	// 8. Restore register values and exit self test mode
	prevCmd &= ~MFRC522_MASK_CMD;	//< IDLE command is implicitly applied here
	mfrc522_writeRegN(self, MFRC522_REG_COMMAND, &prevCmd, 1);
	prevTest &= ~MFRC522_MASK_SELF_TEST;	//< Exiting self test 0h is implicitly applied here
	mfrc522_writeRegN(self, MFRC522_REG_AUTO_TEST, &prevTest, 1);
}


void mfrc522_generateRandomID(mfrc522_t *self, uint8_t* const id) {
	uint8_t buffer[25];

	// Stop any active command
	mfrc522_sendCmd(self, MFRC522_CMD_IDLE, MFRC522_CMD_NO_BLOCKING);

	mfrc522_sendCmd(self, MFRC522_CMD_RND_ID, MFRC522_CMD_BLOCKING);

	// clear FIFO buffer by flushing the pointer
	mfrc522_fifo_flush(self);

	// copy all 25-byte from internal buffer to FIFO buffer using MEM command
	mfrc522_sendCmd(self, MFRC522_CMD_MEM, MFRC522_CMD_BLOCKING);

	// read out all 25-bytes of internal memory. Seems like the generated
	// 10-bytes random ID are not located on the first 10-bytes of 25-bytes
	// internal memory but distributed among the 25-bytes
	mfrc522_fifo_read(self, buffer, 25);

	id[0] = buffer[2];
	id[1] = buffer[3];
	id[2] = buffer[4];
	id[3] = buffer[8];
	id[4] = buffer[9];
	id[5] = buffer[10];
	id[6] = buffer[11];
	id[7] = buffer[12];
	id[8] = buffer[13];
	id[9] = buffer[24];
}


void mfrc522_tx_setEnable(mfrc522_t *self, uint8_t enable) {
	if (enable == 1) {
		mfrc522_updateReg(
				self,
				MFRC522_REG_TX_CONTROL,
				MFRC522_TX2_RF_EN|MFRC522_TX1_RF_EN,
				MFRC522_TX2_RF_EN|MFRC522_TX1_RF_EN);
	} else {
		mfrc522_updateReg(
				self,
				MFRC522_REG_TX_CONTROL,
				MFRC522_TX2_RF_EN|MFRC522_TX1_RF_EN,
				0x00);
	}
}


void mfrc522_rx_setEnable(mfrc522_t *self, uint8_t enable) {
	uint8_t buffer;
	if (enable == 1) {
		buffer = MFRC522_CMD_NOP;
	} else {
		buffer = MFRC522_RCV_OFF | MFRC522_CMD_NOP;
	}

	mfrc522_updateReg(self, MFRC522_REG_COMMAND, MFRC522_RCV_OFF|MFRC522_MASK_CMD, buffer);
}


void mfrc522_powerDown(mfrc522_t *self) {
	mfrc522_updateReg(
		self,
		MFRC522_REG_COMMAND,
		MFRC522_POWER_DOWN|MFRC522_MASK_CMD,
		MFRC522_POWER_DOWN|MFRC522_CMD_NOP);
}


void mfrc522_crc_setPreset(mfrc522_t *self, mfrc522CRCPreset preset) {
	mfrc522_updateReg(self, MFRC522_REG_MODE, MFRC522_MASK_CRC_PRESET, (uint8_t) preset);
}


uint8_t mfrc522_crc_calc(mfrc522_t *self, const uint8_t* const data, uint32_t len, uint8_t* const crc) {
	uint8_t buffer[2];
	uint16_t irq;

	// Stop any active command
	mfrc522_sendCmd(self, MFRC522_CMD_IDLE, MFRC522_CMD_NO_BLOCKING);

	// stop timer and put counter to timer. I am using the timer for timeout
	mfrc522_timer_stop(self);
	mfrc522_timer_loadValue(self, TIMEOUT);

	// clear CRC IRQ. Since I am using the timer for timeout, clear
	// the timer IRQ as well.
	mfrc522_irq_clear(self, MFRC522_IRQ_CRC | MFRC522_IRQ_TIMER);

	// flush FIFO
	mfrc522_fifo_flush(self);

	// put data into FIFO
	mfrc522_fifo_write(self, data, len);

	// start CalcCRC command
	mfrc522_sendCmd(self, MFRC522_CMD_CALC_CRC, MFRC522_CMD_NO_BLOCKING);

	// start timeout timer
	mfrc522_timer_start(self);

	// wait for CRC finished or timeout
	do {
		irq = mfrc522_irq_status(self);
	} while ((irq & (MFRC522_IRQ_TIMER | MFRC522_IRQ_CRC)) == 0);

	// set command back to Idle
	mfrc522_sendCmd(self, MFRC522_CMD_IDLE, MFRC522_CMD_NO_BLOCKING);

	// if success
	if ((irq & MFRC522_IRQ_CRC) != 0) {
		mfrc522_timer_stop(self);

		buffer[0] = MFRC522_REG_CRC_MSB;
		buffer[1] = MFRC522_REG_CRC_LSB;

		mfrc522_readRegN(self, buffer, 2);

		crc[0] = buffer[0];
		crc[1] = buffer[1];

		return MFRC522_CRC_SUCCESS;
	} else { 	// else, timeout
		return MFRC522_CRC_TIMEOUT;
	}
}


void mfrc522_timer_setPrescale(mfrc522_t *self, uint16_t prescale) {
	uint8_t temp;

	mfrc522_updateReg(self, MFRC522_REG_TMODE, MFRC522_MASK_TPRESCALER >> 8, prescale >> 8);

	temp = prescale & 0xFF;
	mfrc522_writeRegN(self, MFRC522_REG_TPRESCALER, &temp, 1);
}


void mfrc522_timer_loadValue(mfrc522_t *self, uint16_t value) {
	uint8_t temp[2];

	temp[0] = (uint8_t)((value >> 8) & 0xFF);
	temp[1] = (uint8_t)(value & 0xFF);

	mfrc522_writeRegN(self, MFRC522_REG_TRELOAD_MSB, temp, 1);
	mfrc522_writeRegN(self, MFRC522_REG_TRELOAD_LSB, &(temp[1]), 1);
}


uint16_t mfrc522_timer_getValue(mfrc522_t *self) {
	uint16_t value;
	uint8_t buffer[2];

	buffer[0] = MFRC522_REG_TCOUNTER_MSB;
	buffer[1] = MFRC522_REG_TCOUNTER_LSB;
	mfrc522_readRegN(self, buffer, 2);

	value = (buffer[0] << 8) | buffer[1];

	return value;
}


void mfrc522_irq_setMode(mfrc522_t *self, mfrc522IrqMode mode) {
	uint8_t irqMode = (uint8_t) mode;
	mfrc522_updateReg(self, MFRC522_REG_COM_IEN, MFRC522_IRQ_INV, (irqMode << 4));
	mfrc522_updateReg(self, MFRC522_REG_DIV_IEN, MFRC522_IRQ_PUSH_PULL, irqMode);
}


void mfrc522_irq_enable(mfrc522_t *self, uint16_t irq) {
	uint8_t value;

	value = (irq & MFRC522_IRQ_ALL) >> 8;
	mfrc522_updateReg(self, MFRC522_REG_COM_IEN, value, value);

	value = (uint8_t)(irq & MFRC522_IRQ_ALL);
	mfrc522_updateReg(self, MFRC522_REG_DIV_IEN, value, value);
}


void mfrc522_irq_disable(mfrc522_t *self, uint16_t irq) {
	uint8_t value;

	value = (irq & MFRC522_IRQ_ALL) >> 8;
	mfrc522_updateReg(self, MFRC522_REG_COM_IEN, value, 0x00);

	value = (uint8_t)(irq & MFRC522_IRQ_ALL);
	mfrc522_updateReg(self, MFRC522_REG_DIV_IEN, value, 0x00);
}


void mfrc522_irq_clear(mfrc522_t *self, uint16_t irq) {
	uint8_t value;

	value = (uint8_t) ((irq & MFRC522_IRQ_ALL) >> 8);
	mfrc522_writeRegN(self, MFRC522_REG_COM_IRQ, &value, 1);

	value = (uint8_t)(irq & MFRC522_IRQ_ALL);
	mfrc522_writeRegN(self, MFRC522_REG_DIV_IRQ, &value, 1);
}


void mfrc522_irq_set(mfrc522_t *self, uint16_t irq) {
	uint8_t value;

	value = ((irq & MFRC522_IRQ_ALL) >> 8) | 0x80;
	mfrc522_writeRegN(self, MFRC522_REG_COM_IRQ, &value, 1);

	value = (uint8_t)(irq & MFRC522_IRQ_ALL) | 0x80;
	mfrc522_writeRegN(self, MFRC522_REG_DIV_IRQ, &value, 1);
}


uint16_t mfrc522_irq_status(mfrc522_t *self) {
	uint8_t buffer[2];

	buffer[0] = MFRC522_REG_COM_IRQ;
	buffer[1] = MFRC522_REG_DIV_IRQ;
	mfrc522_readRegN(self, buffer, 2);

	return uint8ToUint16(buffer);
}


void mfrc522_sendCmd(mfrc522_t *self, uint8_t command, mfrc522CmdBlock block) {
	uint16_t irq;

	// clear command finish IRQ status
	if (block == MFRC522_CMD_BLOCKING) {
		// disable idle IRQ, clear and enable it for blocking mode
		mfrc522_irq_disable(self, MFRC522_IRQ_IDLE);
		mfrc522_irq_clear(self, MFRC522_IRQ_IDLE);
		mfrc522_irq_enable(self, MFRC522_IRQ_IDLE);
	}

	mfrc522_updateReg(self, MFRC522_REG_COMMAND, MFRC522_MASK_CMD, command);

	// wait till command finished
	if (block == MFRC522_CMD_BLOCKING) {
		do {
			irq = mfrc522_irq_status(self);
		}while ((irq & MFRC522_IRQ_IDLE) == 0);

		mfrc522_irq_disable(self, MFRC522_IRQ_IDLE);
		mfrc522_irq_clear(self, MFRC522_IRQ_IDLE);
	}
}


int32_t mfrc522_fifo_read(mfrc522_t *self, uint8_t* const buffer, uint32_t len) {
	int32_t i;

	for (i = 0; i < len; i++) {
		buffer[i] = MFRC522_REG_FIFO_DATA;
	}

	return mfrc522_readRegN(self, buffer, len);
}


int32_t mfrc522_fifo_write(mfrc522_t *self, const uint8_t* const buffer, uint32_t len) {
	return mfrc522_writeRegN(self, MFRC522_REG_FIFO_DATA, buffer, len);
}


int32_t mfrc522_readRegN(mfrc522_t *self, uint8_t* const buffer, uint32_t len) {
	uint8_t *buf;
	int32_t i;

	buf = (uint8_t*) malloc(len + 1);
	if (NULL == buf) {
		return STATUS_ERROR;
	}

	for (i = 0; i < len; i++) {
		buf[i] = ((buffer[i] << 1) & 0x7E) | 0x80;
	}
	buf[len] = 0x00;

	spi_selectCs(self->spiIdx, self->csIdx, LOW);
	spi_transfer(self->spiIdx, buf, buf, len+1);

	for (i = 0; i < len; i++) {
		buffer[i] = buf[i+1];
	}

	free(buf);
	return len;
}


int32_t mfrc522_updateReg(mfrc522_t *self, uint8_t regAddr, uint8_t mask, uint8_t data) {
	uint8_t current;

	// read current register value
	current = regAddr;
	if (mfrc522_readRegN(self, &current, 1) == STATUS_ERROR) {
		return STATUS_ERROR;
	}

	// update current data with new data
	current = (current & ~mask) | (mask & data);
	if (mfrc522_writeRegN(self, regAddr, &current, 1) == STATUS_ERROR) {
		return STATUS_ERROR;
	} else {
		return STATUS_OK;
	}
}


int32_t mfrc522_writeRegN(mfrc522_t *self, uint8_t regAddr, const uint8_t* const data, uint32_t len) {
	uint8_t *buffer;
	int32_t i;

	buffer = (uint8_t*) malloc(len + 1);
	if (NULL == buffer) {
		return STATUS_ERROR;
	}

	buffer[0] = (regAddr << 1) & 0x7E;
	for (i = 0; i < len; i++) {
		buffer[i+1] = data[i];
	}

	spi_selectCs(self->spiIdx, self->csIdx, LOW);
	spi_transfer(self->spiIdx, NULL, buffer, len+1);

	free(buffer);
	return len;
}

