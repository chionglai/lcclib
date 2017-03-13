/*
 * picc14443a-3.c
 *
 *  Created on: 05/08/2013
 *      Author: lcc
 */

#include <string.h>
#include "module/mfrc522-rfid/mfrc522.h"
#include "module/mfrc522-rfid/picc.h"
#include "module/mfrc522-rfid/picc14443a3.h"

/* Local function prototype */
/// Common function for increment, decrement and command. This function is only
/// used internally and should not be used. Use mifare_increment() or
/// mifare_decrement() instead;
/// \param[in] cmd The command byte, either MIFARE_INCREMENT or MIFARE_DECREMENT.
/// \param[in] blockAddr The block address where increment/decrement is done.
/// \param[in] data A 4-byte data containing the increment or decrement values.
/// 	For restore, the data is ignore, but still required.
/// 	\ref MF1S503x datasheet section 10.4
/// \return The status of this function call. \ref mifareErrorCode
uint8_t picc14443a3_inc_dec_res(mfrc522_t *mfrc522, uint8_t cmd, uint8_t blockAddr, const uint8_t* const data);


// 3 way to detect UID not complete
//  a. CT byte
//  b. ATQA
//  c. SAK, bit 2
// This function assume only one card present in the RF field and thus
// does not check for collision
// Tested on MIFARE 1K and 4K
uint8_t picc14443a3_activate(mfrc522_t *mfrc522, uint8_t* const uid, uint8_t* const sak, uint16_t* const atqa) {
	uint8_t buffer[7];
	uint8_t tempBuffer;
	uint32_t rxBit;
	uint8_t status;
	uint8_t CLn;
	int32_t uidIdx;
	int32_t i;

	// 1. send REQA or WUPA
	buffer[0] = PCD_WUPA;
	status = mfrc522_transceive(mfrc522, buffer, 7, &rxBit);
	if (status == MFRC522_TXRX_TIMEOUT) {
		return MIFARE_TIMEOUT;
	} else if (status == MFRC522_TXRX_ERROR) {
		uid[0] = MFRC522_REG_ERROR;
		mfrc522_readRegN(mfrc522, uid, 1);

		return MIFARE_UNKNOWN_ERROR;
	}

	// 2. check ATQA[7:6] for UID size. No need, since ATQA may contain collision.
	*atqa = (buffer[1] << 8) + buffer[0];

	// 3. start anti-collision loop
	CLn = PCD_CL1;
	uidIdx = 0;
	do {
		// 3a. send CLn=9x, NVB=20
		buffer[0] = CLn;
		buffer[1] = 0x20;	//< NVB
		status = mfrc522_transceive(mfrc522, buffer, 16, &rxBit);

		// Check for collision code here.
		if (status == MFRC522_TXRX_ERROR) {
			tempBuffer = MFRC522_REG_ERROR;
			mfrc522_readRegN(mfrc522, &tempBuffer, 1);

			if ((tempBuffer & MFRC522_ERR_COLL) != 0) {
				// Collision occur. Anti-collision code here (refer ISO-IEC14443-3, anti-collision and select)
				// 1. Read from MFRC522_REG_COLL
				// 2. Check that bit[5] is cleared
				// 3. let coll = bit[4:0]
				// 4. NVB = 0x20 + coll
				// 5. Move received UID starting at buffer[0] to buffer[2], with number of bytes = rxBit >> 3
				// 6. buffer[0] = CLn, buffer[1] = NVB
				// 7. Transceive buffer with txBit = rxBit + 16
				// 8. Repeat until no collision for a max loop of 32.

				return MIFARE_COLLISION;
			}
		} else if (status == MFRC522_TXRX_TIMEOUT) {
			return MIFARE_TIMEOUT;
		}

		if (buffer[0] == PICC_CT) {
			memcpy(&(uid[uidIdx]), &(buffer[1]), 3);
			uidIdx += 3;
		} else {
			memcpy(&(uid[uidIdx]), buffer, 4);
			uidIdx += 4;
		}

		// 3b. send CL1=93, NVB=70, received UID packet
		for (i = 6; i > 1; i--) {
			buffer[i] = buffer[i-2];
		}
		buffer[0] = CLn;
		buffer[1] = 0x70;

		// enable Tx and Rx CRC
		mfrc522_updateReg(mfrc522, MFRC522_REG_TX_MODE, MFRC522_TX_CRC_EN, MFRC522_TX_CRC_EN);
		mfrc522_updateReg(mfrc522, MFRC522_REG_RX_MODE, MFRC522_RX_CRC_EN, MFRC522_RX_CRC_EN);

		status = mfrc522_transceive(mfrc522, buffer, 56, &rxBit);

		// Check for collision code here.
		if (status == MFRC522_TXRX_ERROR) {
			tempBuffer = MFRC522_REG_ERROR;
			mfrc522_readRegN(mfrc522, &tempBuffer, 1);

			if ((buffer[0] & MFRC522_ERR_COLL) != 0) {
				// Collision should not happen here! If it does,
				// something is wrong.
				return MIFARE_COLLISION;
			}
		} else if (status == MFRC522_TXRX_TIMEOUT) {
			return MIFARE_TIMEOUT;
		}

		// Disable Tx and Rx CRC
		mfrc522_updateReg(mfrc522, MFRC522_REG_TX_MODE, MFRC522_TX_CRC_EN, 0x00);
		mfrc522_updateReg(mfrc522, MFRC522_REG_RX_MODE, MFRC522_RX_CRC_EN, 0x00);

		// 3c. increase cascade level
		CLn += 2;
	} while ((buffer[0] & PICC_SAK_CT) != 0);		// 3d. Check received SAK cascade bit

	// at this point, card successfully activated and will be in READY state
	*sak = buffer[0];

	return MIFARE_OK;
}


uint8_t picc14443a3_deactivate(mfrc522_t *mfrc522) {
	uint8_t buffer[2];
	uint32_t rxBit;

	// enable Tx CRC
	mfrc522_updateReg(mfrc522, MFRC522_REG_TX_MODE, MFRC522_TX_CRC_EN, MFRC522_TX_CRC_EN);

	buffer[0] = PCD_HLTA >> 8;
	buffer[1] = PCD_HLTA & 0xFF;
	mfrc522_transceive(mfrc522, buffer, 16, &rxBit);

	// disable Tx CRC
	mfrc522_updateReg(mfrc522, MFRC522_REG_TX_MODE, MFRC522_TX_CRC_EN, 0x00);

	// clear MFCrypto1On bit
	mfrc522_updateReg(mfrc522, MFRC522_REG_STATUS2, MFRC522_MF_CRYPTO1_ON, 0x00);

	// PICC should not respond to HALT command. If responded, it should
	// be treated as NAK. \ref ISO/IEC14443-3
	if (rxBit > 0) {
		return MIFARE_NAK;
	}

	return MIFARE_OK;
}


/// The SAK response for different type of MIFARE card
/// MIFARE Mini		- 0b xxx0 1x01
/// MIFARE 1K		- 0b xxx0 1x00
/// MIFARE 4K		- 0b xxx1 1x0x
/// MIFARE UL		- 0b xx00 0x0x
/// MIFARE ISO		- 0b xx10 0x0x
/// MIFARE Plus 2K	- 0b xxx1 0x00
/// MIFARE Plus 4K	- 0b xxx1 0x01
///
/// Tested on MIFARE 1K and 4K
uint8_t picc14443a3_getType(uint8_t sak) {
	// LSB is bit 0, not bit 1 as in the AN10834 document
	if ((sak & 0x02) != 0) {	// bit1 == 1
		return PCD_RFU;		// 0b xxxx xx1x
	} else if ((sak & 0x08) != 0) { // bit1 == 0, bit3 == 1
		if ((sak & 0x10) != 0) {	// bit4 == 1
			return MIFARE_TYPE_4K;	// 0b xxx1 1x0x
		} else if ((sak & 0x01) != 0) {	// bit0 == 1
			return MIFARE_TYPE_MINI;	// 0b xxx0 1x01
		} else {
			return MIFARE_TYPE_1K;		// 0b xxx0 1x00
		}
	} else if ((sak & 0x10) != 0) { // bit4 == 1
		if ((sak & 0x01) != 0) {	// bit0 == 1
			return MIFARE_TYPE_PLUS_4K;	// 0b xxx1 0x01
		} else {
			return MIFARE_TYPE_PLUS_2K;	// 0b xxx1 0x00
		}
	} else if ((sak & 0x20) != 0) {	// bit5 == 1
		return MIFARE_TYPE_ISO_IEC14443_4;	// 0b xx10 0x0x
	} else {
		return MIFARE_TYPE_ULTRALIGHT;		// 0b xx00 0x0x
	}
}


uint8_t picc14443a3_read(mfrc522_t *mfrc522, uint8_t blockAddr, uint8_t* const data) {
	uint32_t rxBit;
	uint8_t status;
	uint8_t errorCode;

	// load command and block address to buffer
	data[0] = PCD_READ;
	data[1] = blockAddr;

	// enable automatic CRC append on Tx
	mfrc522_updateReg(mfrc522, MFRC522_REG_TX_MODE, MFRC522_TX_CRC_EN, MFRC522_TX_CRC_EN);

	// enable CRC check on Rx data
	mfrc522_updateReg(mfrc522, MFRC522_REG_RX_MODE, MFRC522_RX_CRC_EN, MFRC522_RX_CRC_EN);

	// send command and receive data
	status = mfrc522_transceive(mfrc522, data, 16, &rxBit);

	// disable CRC on Tx and Rx
	mfrc522_updateReg(mfrc522, MFRC522_REG_TX_MODE, MFRC522_TX_CRC_EN, 0x00);
	mfrc522_updateReg(mfrc522, MFRC522_REG_RX_MODE, MFRC522_RX_CRC_EN, 0x00);

	switch (status) {
	case MFRC522_TXRX_OK:
		if ((rxBit >> 3) == MIFARE_LENGTH_DATA_BLOCK) {
			errorCode = MIFARE_OK;
		} else {
			errorCode = MIFARE_UNKNOWN_ERROR;
		}
		break;

	case MFRC522_TXRX_TIMEOUT:
		errorCode = MIFARE_TIMEOUT;
		break;

	default:
		// read ErrorReg, reuse variable status
		status = MFRC522_REG_ERROR;
		mfrc522_readRegN(mfrc522, &status, 1);

		// if CRC error
		//   if rxBit == 4, NAK
		//   if rxBit == (16+2)*8, CRC error
		//   else, unknown error
		// else, unknown error
		errorCode = MIFARE_UNKNOWN_ERROR;
		if ((status & MFRC522_ERR_CRC) != 0) {
			if (rxBit == MIFARE_LENGTH_ACK) {
				errorCode = MIFARE_NAK;
			} else if ((rxBit >> 3) == (MIFARE_LENGTH_DATA_BLOCK + MIFARE_LENGTH_CRC)) {
				errorCode = MIFARE_CRC_ERROR;
			}
		}
		break;
	}

	return errorCode;
}


uint8_t picc14443a3_write(mfrc522_t *mfrc522, uint8_t blockAddr, const uint8_t* const data) {
	uint32_t rxBit;
	uint8_t buffer[MIFARE_LENGTH_DATA_BLOCK];
	uint8_t status;
	uint8_t errorCode;

	// load command and block address to buffer
	buffer[0] = PCD_WRITE;
	buffer[1] = blockAddr;

	// enable automatic CRC append on Tx
	mfrc522_updateReg(mfrc522, MFRC522_REG_TX_MODE, MFRC522_TX_CRC_EN, MFRC522_TX_CRC_EN);

	// part 1: send write command.
	status = mfrc522_transceive(mfrc522, buffer, 16, &rxBit);

	buffer[0] = buffer[0] & 0x0F;

	if (status == MFRC522_TXRX_TIMEOUT) {
		errorCode = MIFARE_TIMEOUT;
		goto exit;

	// Don't know why rxBit == 12 when ACK is only 4-bit long. buffer[0]
	// correctly contains the ACK packet, buffer[1] = 0x00. So this checking
	//is removed
//	} else if (rxBit != 4) {
//		errorCode = MIFARE_UNKNOWN_ERROR;
//		goto exit;

	} else if (buffer[0] != PICC_ACK) {
		errorCode = MIFARE_NAK;
		goto exit;
	} else if (status != MFRC522_TXRX_OK) {
		errorCode = MIFARE_UNKNOWN_ERROR;
		goto exit;
	}

	// part2: send write data
	memcpy(buffer, data, MIFARE_LENGTH_DATA_BLOCK);
	status = mfrc522_transceive(mfrc522, buffer, MIFARE_LENGTH_DATA_BLOCK*8, &rxBit);

	buffer[0] &= 0x0F;

	if (status == MFRC522_TXRX_TIMEOUT) {
		// status != OK
		errorCode = MIFARE_TIMEOUT;

	// Don't know why rxBit == 12 when ACK is only 4-bit long. buffer[0]
	// correctly contains the ACK packet, buffer[1] = 0x00. So this checking
	//is removed
//	} else if (rxBit != 4) {
//		errorCode = MIFARE_UNKNOWN_ERROR;

	} else if (buffer[0] != PICC_ACK) {
		// status == OK && rxBit == 4, buffer != ACK
		errorCode = MIFARE_NAK;
	} else if (status != MFRC522_TXRX_OK) {
		errorCode = MIFARE_UNKNOWN_ERROR;
	} else {
		// status == OK && rxBit == 4, buffer == ACK
		errorCode = MIFARE_OK;
	}

exit:
	// disable CRC on Tx and Rx
	mfrc522_updateReg(mfrc522, MFRC522_REG_TX_MODE, MFRC522_TX_CRC_EN, 0x00);

	return errorCode;
}


// almost same procedure as mifare_write()
uint8_t picc14443a3_inc_dec_res(mfrc522_t *mfrc522, uint8_t cmd, uint8_t blockAddr, const uint8_t* const data) {
	uint32_t rxBit;
	uint8_t buffer[4];
	uint8_t status;
	uint8_t errorCode;

	// load command and block address to buffer
	buffer[0] = cmd;
	buffer[1] = blockAddr;

	// enable automatic CRC append on Tx
	mfrc522_updateReg(mfrc522, MFRC522_REG_TX_MODE, MFRC522_TX_CRC_EN, MFRC522_TX_CRC_EN);

	// part 1: send increment command.
	status = mfrc522_transceive(mfrc522, buffer, 16, &rxBit);

	buffer[0] = buffer[0] & 0x0F;

	if (status == MFRC522_TXRX_TIMEOUT) {
		errorCode = MIFARE_TIMEOUT;
		goto exit;

	// Don't know why rxBit == 12 when ACK is only 4-bit long. buffer[0]
	// correctly contains the ACK packet, buffer[1] = 0x00. So this checking
	//is removed
//	} else if (rxBit != 4) {
//		errorCode = MIFARE_UNKNOWN_ERROR;
//		goto exit;

	} else if (buffer[0] != PICC_ACK) {
		errorCode = MIFARE_NAK;
		goto exit;
	} else if (status != MFRC522_TXRX_OK) {
		errorCode = MIFARE_UNKNOWN_ERROR;
		goto exit;
	}

	// part2: send write data
	memcpy(buffer, data, 4);
	status = mfrc522_transceive(mfrc522, buffer, 32, &rxBit);

	buffer[0] &= 0x0F;

	// on part 2, there is no ACK and timeout must be used to signify OK.
	// \ref MF1S503x datasheet
	if (status == MFRC522_TXRX_TIMEOUT) {
		// status != OK
		errorCode = MIFARE_OK;

	// Don't know why rxBit == 12 when ACK is only 4-bit long. buffer[0]
	// correctly contains the ACK packet, buffer[1] = 0x00. So this checking
	//is removed
//	} else if (rxBit != 4) {
//		errorCode = MIFARE_UNKNOWN_ERROR;

	} else if (buffer[0] != PICC_ACK) {
		// status == OK && rxBit == 4, buffer != ACK
		errorCode = MIFARE_NAK;
	} else {
		errorCode = MIFARE_UNKNOWN_ERROR;
	}

exit:
	// disable CRC on Tx and Rx
	mfrc522_updateReg(mfrc522, MFRC522_REG_TX_MODE, MFRC522_TX_CRC_EN, 0x00);

	return errorCode;

}


uint8_t picc14443a3_increment(mfrc522_t *mfrc522, uint8_t blockAddr, const uint8_t* const data) {
	return picc14443a3_inc_dec_res(mfrc522, PCD_INCREMENT, blockAddr, data);
}


uint8_t picc14443a3_decrement(mfrc522_t *mfrc522, uint8_t blockAddr, const uint8_t* const data) {
	return picc14443a3_inc_dec_res(mfrc522, PCD_DECREMENT, blockAddr, data);
}


uint8_t picc14443a3_restore(mfrc522_t *mfrc522, uint8_t blockAddr) {
	uint8_t dummy[4] = {0, 0, 0, 0};
	return picc14443a3_inc_dec_res(mfrc522, PCD_RESTORE, blockAddr, dummy);
}


uint8_t picc14443a3_transfer(mfrc522_t *mfrc522, uint8_t blockAddr) {
	uint32_t rxBit;
	uint8_t buffer[2];
	uint8_t status;
	uint8_t errorCode;

	// load command and block address to buffer
	buffer[0] = PCD_TRANSFER;
	buffer[1] = blockAddr;

	// enable automatic CRC append on Tx
	mfrc522_updateReg(mfrc522, MFRC522_REG_TX_MODE, MFRC522_TX_CRC_EN, MFRC522_TX_CRC_EN);

	// send transfer command
	status = mfrc522_transceive(mfrc522, buffer, 16, &rxBit);

	buffer[0] &= 0x0F;

	// response checking same as write command
	if (status == MFRC522_TXRX_TIMEOUT) {
		// status != OK
		errorCode = MIFARE_TIMEOUT;

	// Don't know why rxBit == 12 when ACK is only 4-bit long. buffer[0]
	// correctly contains the ACK packet, buffer[1] = 0x00. So this checking
	//is removed
//	} else if (rxBit != 4) {
//		errorCode = MIFARE_UNKNOWN_ERROR;

	} else if (buffer[0] != PICC_ACK) {
		// status == OK && rxBit == 4, buffer != ACK
		errorCode = MIFARE_NAK;
	} else if (status != MFRC522_TXRX_OK) {
		errorCode = MIFARE_UNKNOWN_ERROR;
	} else {
		// status == OK && rxBit == 4, buffer == ACK
		errorCode = MIFARE_OK;
	}

	// disable CRC on Tx
	mfrc522_updateReg(mfrc522, MFRC522_REG_TX_MODE, MFRC522_TX_CRC_EN, 0x00);

	return errorCode;
}

