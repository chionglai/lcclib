/*
 * picc.h
 *
 *  Created on: 05/08/2013
 *      Author: lcc
 *
 *  Note:
 *  1. This module implements the functionalities for PICC as defined by
 *     ISO/IEC 14443-4, which extends the ISO/IEC 14443-3 with additional
 *     features.
 *
 * PICC = proximity integrated-circuit card
 */

#ifndef PICC_H_
#define PICC_H_

/// Commands for ISO/IEC 14443A-3
#define PCD_REQA					0x26	//< Note: This command is only 7-bit long
#define PCD_WUPA					0x52	//< Note: this command is only 7-bit long
#define PCD_HLTA					0x5000	//< Note: This command is 2-byte long
#define PCD_AUTH_KEYA				0x60
#define PCD_AUTH_KEYB				0x61
#define PCD_READ					0x30
#define PCD_WRITE					0xA0
#define PCD_DECREMENT				0xC0
#define PCD_INCREMENT				0xC1
#define PCD_RESTORE					0xC2
#define PCD_TRANSFER				0xB0
#define PCD_CL1						0x93	//< Cascade-level 1
#define PCD_CL2						0x95	//< Cascade-level 2
#define PCD_CL3						0x97	//< Cascade-level 2
#define PCD_RFU						0xFF	//< reserved for future use

// Commands for ISO/IEC 14443A-4
#define PCD_RATS					0xE0
#define PCD_PPS						0xD0	//< Lower 4-bit is CID and need to be formed at run-time

// Response from PICC
#define PICC_ACK					0x0A	//< 4-bit only
#define PICC_CT						0x88	//< Cascade tag Type A, signify UID not complete yet. \ref AN10927 Handling UID
#define PICC_SAK_CT					0x04


#endif /* PICC_H_ */
