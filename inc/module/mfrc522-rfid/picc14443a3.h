/*
 * picc14443a3.h
 *
 *  Created on: 05/08/2013
 *      Author: lcc
 *
 *  Note:
 *  1. This module implements the functionalities for MIFARE
 *     classic as defined by ISO/IEC 14443-3 Type A/MIFARE.
 *  2. It does not implement functionalities for ISO/IEC 14443
 *     Type B PICC, since MFRC522 module does not support it.
 *  3. All functions only works for a single card presence, unless
 *     stated otherwise
 *  4. Data transfer speed for MIFARE Classic is 106 kBaud.
 *  5. The process of accessing the card is (in order):
 *     a. Send REQA or WUPA to poll/wake-up card
 *     b. Do anti-collision loop to select a card
 *     c. Authenticate with card for a particular sector
 *     d. Read/write and do other command on that sector
 *     e. Send HALT command to make the card sleep once completed
 *  6. If want to access other sector, need to send HALT command
 *     to card and clear MFCrypto1On bit of MFRC522 module to
 *     disable encryption.
 *  7. Read, write, increment, decrement, restore and transfer functions
 *     depends on the memory access condition for each sector, and are
 *     determined by the access bits. \ref MF1S503x datasheet, all
 *     section 8.7.
 *  8. For increment, decrement, restore and transfer functions to work,
 *     the block (which those functions need to run) need to be first
 *     setup (using write command) to have proper data format.
 *     \ref MF1S503x datasheet, section 8.6.2.1. Otherwise, NAK will be
 *     received.
 *
 * PICC = proximity integrated-circuit card
 */

#ifndef PICC14443A3_H_
#define PICC14443A3_H_

#include <stdint.h>
#include "module/mfrc522-rfid/mfrc522.h"

#define NUM_RETRY					10		//< Number of retries before flagged error/timeout

#define MIFARE_LENGTH_CRC			2		//< in number of bytes
#define MIFARE_LENGTH_DATA_BLOCK	16		//< in number of bytes
#define MIFARE_LENGTH_ACK			4		//< in number of bits

// This is the intepretation of bit[7:6] of received ATQA response from PICC from WUPA or REQA command.
typedef enum {
	MIFARE_CL_1_OR_UID_4  = 0,		//< Cascade level 1, or 4-byte UID
	MIFARE_CL_2_OR_UID_7  = 1,		//< Cascade level 2, or 7-byte UID
	MIFARE_CL_3_OR_UID_10 = 2		//< Cascade level 3, or 10-byte UID
} mifareUidSize_t;

typedef enum {
	MIFARE_TYPE_MINI = 0,
	MIFARE_TYPE_1K,
	MIFARE_TYPE_4K,
	MIFARE_TYPE_ULTRALIGHT,
	MIFARE_TYPE_PLUS_2K,
	MIFARE_TYPE_PLUS_4K,
	MIFARE_TYPE_ISO_IEC14443_4,
} mifareType;


typedef enum {
	MIFARE_OK = 0,
	MIFARE_TIMEOUT,
	MIFARE_NAK,
	MIFARE_CRC_ERROR,
	MIFARE_COLLISION,
	MIFARE_UNKNOWN_ERROR
} mifareErrorCode;


#ifdef __cplusplus
extern "C" {
#endif


/// To activate a MIFARE card. Support MIFARE Mini, MIFARE 1K, MIFARE 4K, MIFARE UL,
/// MIFARE Plus 2K, MIFARE Plus 4K, and ISO/IEC1443-3 compliance card. This function
/// performs the necessary REQA
/// \param[out] uid A byte array containing the activated card UID. The array size
/// 	must be 10-byte to support up to 10-byte UID.
/// \param[out] sak The SAK byte received at the end of anti-collision loop. It can
/// 	be used to identify the type of card being activated.
/// 	\ref AN10834 MIFARE PICC selection, Fig. 3.
/// \param[out] atqa The received 16-bit ATQA response from PICC, containing the UID
///		size and bit frame anti-collision.
/// \return The status of this function call. If UNKNOWN_ERROR is returned, the
/// 	value of ErrorReg is returned in uid[0]. \ref mifareErrorCode
uint8_t picc14443a3_activate(mfrc522_t *mfrc522, uint8_t* const uid, uint8_t* const sak, uint16_t* const atqa);


/// To deactivate the currently activated card. It is recommended that all card must
/// be deactivated once we are done with it.
/// \return The status of this function call, either OK or NAK. \ref mifareErrorCode
uint8_t picc14443a3_deactivate(mfrc522_t *mfrc522);

/// To determine which type of card is activated from the received SAK byte.
/// \ref AN10834 MIFARE PICC selection, Fig. 3.
/// \param[in] sak The received SAK byte after anti-collision loop.
/// \return The type of card activated. \ref mifareType
uint8_t picc14443a3_getType(uint8_t sak);

/// To read a block of 16-byte data from card.
/// \param[in] blockAddr The block address to read from. The range is 0x00 till 0x3F.
/// \param[out] data The block of 16-byte data read. Size of data must be 16-byte.
/// \return The status of this function call, either OK, NAK or TIMEOUT.
/// 	\ref mifareErrorCode
uint8_t picc14443a3_read(mfrc522_t *mfrc522, uint8_t blockAddr, uint8_t* const data);

/// To write a block of 16-byte data to card.
/// \param[in] blockAddr The block address to write to. The range is 0x00 till 0x3F.
/// \param[in] data The block of 16-byte data read. Size of data must be 16-byte.
/// \return The status of this function call, either OK, NAK or TIMEOUT.
/// 	\ref mifareErrorCode
uint8_t picc14443a3_write(mfrc522_t *mfrc522, uint8_t blockAddr, const uint8_t* const data);

/// To add/increment the value of the addressed block with the value in data.
/// The result will be stored in a volatile memory and requires a mifare_transfer()
/// call to write them to a new block address on card.
/// \param[in] blockAddr The block address whose value is to be added.
/// \param[in] data A 4-byte data.
/// \return The status of this function call, either OK, NAK or TIMEOUT.
/// 	\ref mifareErrorCode
uint8_t picc14443a3_increment(mfrc522_t *mfrc522, uint8_t blockAddr, const uint8_t* const data);

/// To subtract/decrement the value of the addressed block with the value in data.
/// The result will be stored in a volatile memory and requires a mifare_transfer()
/// call to write them to a new block address on card.
/// \param[in] blockAddr The block address whose value is to be subtracted.
/// \param[in] data A 4-byte data.
/// \return The status of this function call, either OK, NAK or TIMEOUT.
/// 	\ref mifareErrorCode
uint8_t picc14443a3_decrement(mfrc522_t *mfrc522, uint8_t blockAddr, const uint8_t* const data);

/// To copy the value of the addressed block to a volatile memory. It
/// requires a mifare_transfer() call to write them to a new block
/// address on card.
/// \param[in] blockAddr The block address whose value is to be copied.
/// \return The status of this function call, either OK, NAK or TIMEOUT.
/// 	\ref mifareErrorCode
uint8_t picc14443a3_restore(mfrc522_t *mfrc522, uint8_t blockAddr);

/// To transfer the value stored in the volatile memory to a destination
/// block address on card.
/// \param[in] blockAddr The destination block address.
/// \return The status of this function call, either OK, NAK or TIMEOUT.
/// 	\ref mifareErrorCode
uint8_t picc14443a3_transfer(mfrc522_t *mfrc522, uint8_t blockAddr);


#ifdef __cplusplus
}
#endif

#endif /* PICC14443A3_H_ */
