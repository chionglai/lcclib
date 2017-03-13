/*
 * mfrc522.h
 *
 *  Created on: 06/06/2013
 *      Author: lcc
 *
 * Note:
 * 1. Although the MFRC522 itself support I2C, SPI and UART communication
 *    protocols, the module I purchased only support SPI because the I2C
 *    pin is hardwired to GND (LOW) and EA pin is hardwired to 3.3V (HI).
 *    This configuration allows only the SPI. Refer to datasheet,
 *    section 8.1.2, pg 10 for interfacing using SPI, particularly
 *    section 8.1.2.3
 * 2. For the module that I have:
 * 		SCK:  SPI clock
 * 		MOSI: SPI MOSI
 * 		MISO: SPI MISO
 * 		SDA:  SPI CS
 * 		IRQ:  NC
 * 		RST:  Active low. Internally pull-up, so if reset is not required, no need
 * 			  to connect.
 * 3. MFRC522 only support ISO/IEC 14443 Type A/MIFARE proximity integrated
 *    circuit card (PICC). It does not support ISO/IEC 14443 Type B PICC.
 */


#ifndef MFRC522_H
#define MFRC522_H

#include <stdint.h>

/// Mifare 1K and 4K has max timeout of 10ms. Just in case, timeout has been set to 50ms.
#define TIMEOUT						4999				//< Various timeout, (499+1)*100us = 50ms

#define MFRC522_FIFO_LENGTH			64		//< 64-byte FIFO buffer

/// Define for registers addresses
/// Refer to MFRC522 datasheet, Table 20, section 9.2, pg 35
// Page 0 - Command and status registers
#define MFRC522_REG_COMMAND			0x01
#define MFRC522_REG_COM_IEN			0x02
#define MFRC522_REG_DIV_IEN			0x03
#define MFRC522_REG_COM_IRQ			0x04
#define MFRC522_REG_DIV_IRQ			0x05
#define MFRC522_REG_ERROR			0x06
#define MFRC522_REG_STATUS1			0x07
#define MFRC522_REG_STATUS2			0x08
#define MFRC522_REG_FIFO_DATA		0x09
#define MFRC522_REG_FIFO_LEVEL		0x0A
#define MFRC522_REG_WATER_LEVEL		0x0B
#define MFRC522_REG_CONTROL			0x0C
#define MFRC522_REG_BIT_FRAMING		0x0D
#define MFRC522_REG_COLL			0x0E

// Page 1 - Command registers
#define MFRC522_REG_MODE			0x11
#define MFRC522_REG_TX_MODE			0x12
#define MFRC522_REG_RX_MODE			0x13
#define MFRC522_REG_TX_CONTROL		0x14
#define MFRC522_REG_TX_ASK			0x15
#define MFRC522_REG_TX_SEL			0x16
#define MFRC522_REG_RX_SEL			0x17
#define MFRC522_REG_RX_THRESHOLD	0x18
#define MFRC522_REG_DEMOD			0x19
#define MFRC522_REG_MF_TX			0x1C
#define MFRC522_REG_MF_RX			0x1D
#define MFRC522_REG_SERIAL_SPEED	0x1F

// Page 2 - Configuration registers
#define MFRC522_REG_CRC_MSB			0x21
#define MFRC522_REG_CRC_LSB			0x22
#define MFRC522_REG_MOD_WIDTH		0x24
#define MFRC522_REG_RF_CFG			0x26
#define MFRC522_REG_GSN				0x27
#define MFRC522_REG_CW_GSP			0x28
#define MFRC522_REG_MOD_GSP			0x29
#define MFRC522_REG_TMODE			0x2A
#define MFRC522_REG_TPRESCALER		0x2B
#define MFRC522_REG_TRELOAD_MSB		0x2C
#define MFRC522_REG_TRELOAD_LSB		0x2D
#define MFRC522_REG_TCOUNTER_MSB	0x2E
#define MFRC522_REG_TCOUNTER_LSB	0x2F

// Page 3 - Test registers
#define MFRC522_REG_TEST_SEL1		0x31
#define MFRC522_REG_TEST_SEL2		0x32
#define MFRC522_REG_TEST_PIN_EN		0x33
#define MFRC522_REG_TEST_PIN_VAL	0x34
#define MFRC522_REG_TEST_BUS		0x35
#define MFRC522_REG_AUTO_TEST		0x36
#define MFRC522_REG_VERSION			0x37
#define MFRC522_REG_ANALOG_TEST		0x38
#define MFRC522_REG_TEST_DAC1		0x39
#define MFRC522_REG_TEST_DAC2		0x3A
#define MFRC522_REG_TEST_ADC		0x3B


/// Mask and value for CommandReg
#define MFRC522_MASK_CMD			0x0F
#define MFRC522_RCV_OFF				0x20
#define MFRC522_POWER_DOWN			0x10

/// Mask and value for ComIEnReg, ComIrqReg, DivIEnReg and DivIrqReg
#define MFRC522_IRQ_INV				0x80
#define MFRC522_IRQ_PUSH_PULL		0x80

/// redefined IRQ bits for better IRQ handling
typedef enum {
	MFRC522_IRQ_TX				= 0x4000,
	MFRC522_IRQ_RX				= 0x2000,
	MFRC522_IRQ_IDLE			= 0x1000,
	MFRC522_IRQ_HI_ALERT		= 0x0800,
	MFRC522_IRQ_LO_ALERT		= 0x0400,
	MFRC522_IRQ_ERR				= 0x0200,
	MFRC522_IRQ_TIMER			= 0x0100,
	MFRC522_IRQ_MFIN_ACT		= 0x0010,
	MFRC522_IRQ_CRC				= 0x0004,
	MFRC522_IRQ_ALL				= 0x7F14
} mfrc522Irq;

typedef enum {
	MFRC522_IRQ_CMOS_NORMAL			= 0x80,	//< signal on IRQ pin is equals to IRQ bit, CMOS 3-state
	MFRC522_IRQ_CMOS_INVERTED		= 0x88, //< signal on IRQ pin is inverted to IRQ bit, CMOS 3-state
	MFRC522_IRQ_OPEN_DRAIN_NORMAL	= 0x00, //< normal, open drain
	MFRC522_IRQ_OPEN_DRAIN_INVERTED	= 0x08  //< inverted, open drain
} mfrc522IrqMode;

/// Mask and value for ErrorReg
#define MFRC522_ERR_WR				0x80
#define MFRC522_ERR_TEMP			0x40
#define MFRC522_ERR_BUFFER_OVFL		0x10
#define MFRC522_ERR_COLL			0x08
#define MFRC522_ERR_CRC				0x04
#define MFRC522_ERR_PARITY			0x02
#define MFRC522_ERR_PROTOCOL		0x01

/// Mask and value for Status1Reg
#define MFRC522_CRC_OK				0x40
#define MFRC522_CRC_READY			0x20
#define MFRC522_IRQ					0x10
#define MFRC522_TRUNNING			0x08
#define MFRC522_HI_ALERT			0x02
#define MFRC522_LO_ALERT			0x01

/// Mask and value for Status2Reg
#define MFRC522_TEMP_SENS_CLEAR		0x80
#define MFRC522_I2C_FORCE_HS		0x40
#define MFRC522_MF_CRYPTO1_ON		0x08
#define MFRC522_MASK_MODEM_STATE	0x07

/// value for ModemState
typedef enum {
	MFRC522_MODEM_STATE_IDLE 		= 0x00,
	MFRC522_MODEM_STATE_START_WAIT 	= 0x01,
	MFRC522_MODEM_STATE_TX_WAIT		= 0x02,
	MFRC522_MODEM_STATE_TX			= 0x03,
	MFRC522_MODEM_STATE_RX_WAIT		= 0x04,
	MFRC522_MODEM_STATE_DATA_WAIT	= 0x05,
	MFRC522_MODEM_STATE_RX			= 0x06
} mfrc522ModemState;

/// Mask and value for FIFOLevelReg
#define MFRC522_FIFO_FLUSH			0x80
#define MFRC522_MASK_FIFO_LEVEL		0x7F

/// Mask and value for WaterLeveReg
#define MFRC522_MASK_WATER_LEVEL	0x3F

/// Mask and value for ControlReg
#define MFRC522_TSTOP_NOW			0x80
#define MFRC522_TSTART_NOW			0x40
#define MFRC522_MASK_RX_LAST_BIT	0x07	//< If it is 000b, the whole byte is valid

/// Mask and value for BitFramingReg
#define MFRC522_START_SEND			0x80
#define MFRC522_GET_RX_ALIGN(x)		((x >> 4) & 0x07)	//< macro for getting the RxAlign value
#define MFRC522_SET_RX_ALIGN(x) 	((x & 0x07) << 4)	//< macro for setting the RxAlign value
#define MFRC522_MASK_TX_LAST_BIT	0x07				//< If 000b, the whole byte will be transmitted

/// Mask and value for CollReg
#define MFRC522_VALUES_AFTER_COLL	0x80
#define MFRC522_COLL_POS_NOT_VALID	0x20
#define MFRC522_GET_COLL_POS(x) 	(x & 0x1F)			// macro for getting the bit position of first detected collision

/// Mask and value for ModeReg
#define MFRC522_MSB_FIRST			0x80
#define MFRC522_TX_WAIT_RF			0x20
#define MFRC522_POL_MFIN			0x08
#define MFRC522_MASK_CRC_PRESET		0x03

/// CRC coprocessor preset value for CalcCRC command
typedef enum {
	MFRC522_CRC_PRESET_0000H		= 0x00,
	MFRC522_CRC_PRESET_6363H		= 0x01,
	MFRC522_CRC_PRESET_A671H		= 0x02,
	MFRC522_CRC_PRESET_FFFFH		= 0x03
} mfrc522CRCPreset;

typedef enum {
	MFRC522_CRC_SUCCESS				= 0x00,		//< CRC succeeds, only when using CalcCRC command
	MFRC522_CRC_TIMEOUT				= 0x01,		//< CRC timeout, only when using CalcCRC command
	MFRC522_CRC_FAIL				= 0x02		//< CRC fails for Tx and Rx
} mfrc522CrcError;

/// Mask and value for TxModeReg
#define MFRC522_TX_CRC_EN			0x80
#define MFRC522_MASK_TX_SPEED		0x70	// \ref mfrc522BitRate
#define MFRC522_INV_MOD				0x08

/// Value for bit rate during data transmission, in kilo-baud,
/// for both Tx (in TxModeReg) and Rx (in RxModeReg)
typedef enum {
	MFRC522_BIT_RATE_106KBD			= 0x00,
	MFRC522_BIT_RATE_212KBD			= 0x10,
	MFRC522_BIT_RATE_424KBD			= 0x20,
	MFRC522_BIT_RATE_848KBD			= 0x30
} mfrc522BitRate;

/// Mask and value for RxModeReg
#define MFRC522_RX_CRC_EN			0x80
#define MFRC522_MASK_RX_SPEED		0x70	// \ref mfrc522BitRate
#define MFRC522_RX_NO_ERR			0x08
#define MFRC522_RX_MULTIPLE			0x04

/// Mask and value for TxControlReg
#define MFRC522_INV_TX2_RF_ON		0x80
#define MFRC522_INV_TX1_RF_ON		0x40
#define MFRC522_INV_TX2_RF_OFF		0x20
#define MFRC522_INV_TX1_RF_OFF		0x10
#define MFRC522_TX2_CW				0x08
#define MFRC522_TX2_RF_EN			0x02
#define MFRC522_TX1_RF_EN			0x01

/// Mask and value for TxASKReg
#define MFRC522_FORCE_100ASK		0x40

/// Mask and value for TxSelReg
#define MFRC522_MASK_DRIVER_SEL		0x30
#define MFRC522_MASK_MFOUT_SEL		0x0F

typedef enum {
	MFRC522_DRIVER_SEL_3STATE		= 0x00,
	MFRC522_DRIVER_SEL_MILLER		= 0x10,
	MFRC522_DRIVER_SEL_MFIN			= 0x20,
	MFRC522_DRIVER_SEL_HIGH			= 0x30
} mfrc522DriverSel;

typedef enum {
	MFRC522_MFOUT_SEL_3STATE		= 0x00,
	MFRC522_MFOUT_SEL_LOW			= 0x01,
	MFRC522_MFOUT_SEL_HIGH			= 0x02,
	MFRC522_MFOUT_SEL_TEST_BUS		= 0x03,
	MFRC522_MFOUT_SEL_MILLER		= 0x04,
	MFRC522_MFOUT_SEL_SERIAL		= 0x05,
	MFRC522_MFOUT_SEL_MANCHESTER	= 0x07
} mfrc522MFOutSel;

/// Mask and value for RxSelReg
#define MFRC522_MASK_UART_SEL		0xC0
#define MFRC522_RX_WAIT(x)			(x & 0x3F)		// macro for getting and setting RxWait

typedef enum {
	MFRC522_UART_SEL_LOW			= 0x00,
	MFRC522_UART_SEL_MANCHESTER		= 0x40,
	MFRC522_UART_SEL_INT_ANALOG		= 0x80,
	MFRC522_UART_SEL_NRZ			= 0xC0
} mfrc522UARTSel;

/// Mask and value for RxThresholdReg
#define MFRC522_GET_MIN_LEVEL(x)	((x >> 4) & 0x0F)	// macro for getting MinLevel
#define MFRC522_SET_MIN_LEVEL(x)	((x & 0x0F) << 4)	// macro for setting MinLeve
#define MFRC522_COLL_LEVEL(x)		(x & 0x07)			// macro for getting and setting CollLevel

/// Mask and value for DemodReg
#define MFRC522_MASK_IQ_CHAN		0xE0
#define MFRC522_TPRESCAL_EVEN		0x10
#define MFRC522_MASK_TAU_RCV		0x0C
#define MFRC522_MASK_TAU_SYNC		0x03

// Value for IQ channel selection
typedef enum {
	MFRC522_IQ_FIXI					= 0x20,
	MFRC522_IQ_FIXQ					= 0x60,
	MFRC522_IQ_STRONG				= 0x00,
	MFRC522_IQ_STRONG_FREEZE		= 0x40
} mfrc522IQ;

/// Mask and value for MfTxReg
#define MFRC522_TX_WAIT(x) 			(x & 0x03)		// macro for getting and setting TxWait

/// Mask and value for MfRxReg
#define MFRC522_PARITY_DISABLE		0x10

/// Value for SerialSpeedReg
/// Refer datasheet section 8.1.3.2, table 10, pg 12
typedef enum {
	MFRC522_SERIAL_SPEED_7200		= 0xFA,
	MFRC522_SERIAL_SPEED_9600		= 0xEB,
	MFRC522_SERIAL_SPEED_14400		= 0xDA,
	MFRC522_SERIAL_SPEED_19200		= 0xCB,
	MFRC522_SERIAL_SPEED_38400		= 0xAB,
	MFRC522_SERIAL_SPEED_57600		= 0x9A,
	MFRC522_SERIAL_SPEED_115200		= 0x7A,
	MFRC522_SERIAL_SPEED_128000		= 0x74,
	MFRC522_SERIAL_SPEED_230400		= 0x5A,
	MFRC522_SERIAL_SPEED_460800		= 0x3A,
	MFRC522_SERIAL_SPEED_921600		= 0x1C,
	MFRC522_SERIAL_SPEED_1228800	= 0x15
} mfrc522SerialSpeed;

/// Mask and value for RFCfgReg
#define MFRC522_MASK_RX_GAIN		0x70

typedef enum {
	MFRC522_RX_GAIN_18DB			= 0x20,
	MFRC522_RX_GAIN_23DB			= 0x30,
	MFRC522_RX_GAIN_33DB			= 0x40,
	MFRC522_RX_GAIN_38DB			= 0x50,
	MFRC522_RX_GAIN_43DB			= 0x60,
	MFRC522_RX_GAIN_48DB			= 0x70
} mfrc522RxGain;

/// Mask and value for CWGsNReg
#define MFRC522_GET_CW_GSN(x)		((x >> 4) & 0x0F)
#define MFRC522_SET_CW_GSN(x)		((x & 0x0F) << 4)
#define MFRC522_MOD_GSN(x)			(x & 0x0F)

/// Mask and value for CWGsPReg
#define MFRC522_CW_GSP(x)			(x & 0x3F)

/// Mask and value for ModGsPReg
#define MFRC522_MOD_GSP(x)			(x & 0x3F)

/// Mask and value for TModeReg
#define MFRC522_TAUTO				0x80
#define MFRC522_MASK_TGATED			0x60
#define MFRC522_TAUTO_RESTART		0x10
#define MFRC522_MASK_TPRESCALER		0x0FFF

typedef enum {
	MFRC522_TGATED_NON			= 0x00,
	MFRC522_TGATED_MFIN			= 0x20,
	MFRC522_TGATED_AUX1			= 0x40
} mfrc522TGated;


/// Mask and value for TestSel1Reg
#define MFRC522_TST_BUS_BIT_SEL(x)	(x & 0x03)

/// Mask and value for TestSel2Reg
#define MFRC522_TST_BUS_FLIP		0x80
#define MFRC522_PRBS9				0x40
#define MFRC522_PRBS15				0x20
#define MFRC522_TEST_BUS_SEL(x)		(x & 0x1F)	// macro for getting and setting TestBusSel

/// Mask and value for TestPinEnReg
#define MFRC522_RS232_LINE_EN		0x80
#define MFRC522_GET_TEST_PIN_EN(x)	((x >> 1) & 0x3F)
#define MFRC522_SET_TEST_PIN_EN(x)	((x & 0x3F) << 1)

/// Mask and value for TestPinValueReg
#define MFRC522_USE_IO					0x80
#define MFRC522_GET_TEST_PIN_VALUE(x)	((x >> 1) & 0x3F)
#define MFRC522_SET_TEST_PIN_VALUE(x)	((x & 0x3F) << 1)

/// Mask and value for AutoTestReg
#define MFRC522_AMP_RCV				0x40
#define MFRC522_MASK_SELF_TEST		0x0F

typedef enum {
	MFRC522_NORMAL 		= 0x00,		//< Put into normal mode
	MFRC522_SELF_TEST 	= 0x09		//< Put into digital self test mode
} mfrc522SelfTest;

/// Mask and value for AnalogTestReg
#define MFRC522_GET_ANALOG_SEL_AUX1(x)	((x >> 4) & 0x0F)
#define MFRC522_SET_ANALOG_SEL_AUX1(x)	((x & 0x0F) << 4)
#define MFRC522_GET_ANALOG_SEL_AUX2(x)	(x & 0x0F)
#define MFRC522_SET_ANALOG_SEL_AUX2(x)	(x & 0x0F)

typedef enum {
	MFRC522_ANALOG_SEL_AUX_3STATE		= 0x00,
	MFRC522_ANALOG_SEL_AUX_DAC_OUT		= 0x01,
	MFRC522_ANALOG_SEL_AUX_CORR			= 0x02,
	MFRC522_ANALOG_SEL_AUX_MIN_LEVEL	= 0x04,
	MFRC522_ANALOG_SEL_AUX_ADC_I		= 0x05,
	MFRC522_ANALOG_SEL_AUX_ADC_Q		= 0x06,
	MFRC522_ANALOG_SEL_AUX_HIGH			= 0x0A,
	MFRC522_ANALOG_SEL_AUX_LOW			= 0x0B
} mfrc522AnalogSelAux;

/// Mask and value for TestDAC1Reg
#define MFRC522_MASK_TEST_DAC		0x3F

/// Mask and value for TestADCReg
#define MFRC522_GET_ADC_I(x)		((x >> 4) & 0x0F)
#define MFRC522_GET_ADC_Q(x)		(x & 0x0F)

/// Define for MFRC commands
/// Refer MFRC522 datasheet section 10.3, pg 69
#define MFRC522_CMD_IDLE			0x00
#define MFRC522_CMD_MEM				0x01
#define MFRC522_CMD_RND_ID			0x02
#define MFRC522_CMD_CALC_CRC		0x03
#define MFRC522_CMD_TX				0x04
#define MFRC522_CMD_NOP				0x07
#define MFRC522_CMD_RX				0x08
#define MFRC522_CMD_TXRX			0x0C
#define MFRC522_CMD_MF_AUTH			0x0E
#define MFRC522_CMD_RESET			0x0F

typedef enum {
	MFRC522_CMD_NO_BLOCKING 	= 0x00,	//< no blocking on command execution
	MFRC522_CMD_BLOCKING		= 0x01	//< flag to wait till command finish execution before returning to calling function
} mfrc522CmdBlock;

typedef enum {
	MFRC522_TXRX_OK	= 0x00,
	MFRC522_TXRX_TIMEOUT,
	MFRC522_TXRX_ERROR
} mfrc522TransceiveStatus;

typedef enum {
	MFRC522_MIFARE_AUTH_SUCCESS = 0x00,			//< Mifare authenticated
	MFRC522_MIFARE_AUTH_TIMEOUT,				//< Mifare authentication timeout/fails, i.e. incorrect key
	MFRC522_MIFARE_PROTOCOL_ERROR,				//< Mifare authentication protocol error
	MFRC522_MIFARE_UNKNOWN_ERROR				//<
} mfrc522MifareAuthStatus;
#define MFRC522_MIFARE_AUTH_FAIL	MFRC522_MIFARE_AUTH_TIMEOUT

typedef enum {
	MFRC522_MIFARE_KEY_A			= 0x60,		//< validation using key A
	MFRC522_MIFARE_KEY_B			= 0x61		//< validation using key B
} mfrc522MifareKeyType;

typedef struct {
	/* SPI index used for communication to mfrc522. */
	uint8_t spiIdx;
	/* CS index used for communication to mfrc522. */
	uint8_t csIdx;
} mfrc522Cfg_t;

typedef struct mfrc522_s mfrc522_t;

/// macro to start MFRC522 timer unit
#define mfrc522_timer_start(self) 	mfrc522_updateReg(self, MFRC522_REG_CONTROL, MFRC522_TSTART_NOW, MFRC522_TSTART_NOW)

/// macro to stop MFRC522 timer unit
#define mfrc522_timer_stop(self) 	mfrc522_updateReg(self, MFRC522_REG_CONTROL, MFRC522_TSTOP_NOW, MFRC522_TSTOP_NOW)

/// macro to flush FIFO buffer
#define mfrc522_fifo_flush(self)	mfrc522_updateReg(self, MFRC522_REG_FIFO_LEVEL, MFRC522_FIFO_FLUSH, MFRC522_FIFO_FLUSH)

/// macro to convert from 2-byte array to 16-bit integer
#define uint8ToUint16(X)			((X[0] << 8) | X[1])

#ifdef __cplusplus
extern "C" {
#endif


/// Initialise MFRC522 and setup all the communication protocol using bcm2835 library.
/// \param[out] self Pointer to store the created and initialised mfrc522 instance.
/// \param[in] cfg Configuration to create a new instance of mfrc522.
/// \return Returns STATUS_OK if success, STATUS_ERROR otherwise.
int32_t mfrc522_create(mfrc522_t **ppSelf, const mfrc522Cfg_t *cfg);

/// To power down MFRC522 module and releasing the resources.
/// \param[in/out] self Pointer containing the instance to be destroyed. Once
///		destroyed, *self will be set to NULL and must not be used.
void mfrc522_destroy(mfrc522_t **ppSelf);

/// Manage standard Mifare authentication to Mifare Mini, Mifare 1k and Mifare 4k.
/// \param[in/out] self Instance of mfrc522 to use.
/// \param[in] type Key validation type, either key A or key B \ref mfrc522MifareKeyType.
/// \param[in] blockAddr Block address for the memory being accessed in Mifare card.
/// 	Refer Mifare card datasheet for more details.
/// \param[in] key A 6-byte array specifying the sector key for authentication.
/// \param[in] serial A 4-byte array specifying the card serial number. This is the first
///		4 bytes of the card UID, obtained by calling mifare_activate().
/// \return The status of Mifare authentication \ref mfrc522MifareAuthStatus
uint8_t mfrc522_authMifare(mfrc522_t *self, mfrc522MifareKeyType type, uint8_t blockAddr, const uint8_t* const key, const uint8_t* const serial);

/// To transmit data to card, followed by receiving data/responds from card.
/// Received data/respond will be stored in the data buffer, which originally
/// contains transmit data.
/// \param[in/out] self Instance of mfrc522 to use.
/// \param[in/out] data A byte array containing data to be transmitted. Upon return
/// 	of this function, the array may contain any data/respond received from a
/// 	card, if any. Data/respond is only valid if MFRC522_TXRX_SUCCESS is returned.
///		When MFRC522_TXRX_ERROR is returned, data[0] contains the value of register ErrorReg.
/// \param[in] txBitLen The length of transmit data, in bits.
/// \param[out] rxBitLen The length of receive data, in bits. Value is only valid
///		if MFRC522_TXRX_SUCCESS is returned.
/// \return The status of this transceive function. \ref mfrc522TransceiveStatus.
uint8_t mfrc522_transceive(mfrc522_t *self, uint8_t* const data, uint32_t txBitLen, uint32_t* const rxBitLen);

/// Get the version value stored in the VersionReg
/// \param[in/out] self Instance of mfrc522 to use.
/// \return A byte indicating chip type and version. bit[7:4] = 9 for MFRC522, bit[3:0] = 1 for version 1.0, 2 for version 2.0.
uint8_t mfrc522_getVersion(mfrc522_t *self);

/// Perform self test for MFRC522.
/// \param[in/out] self Instance of mfrc522 to use.
/// \param[out] A char/byte array of length 64. Refer MFRC552 datasheet, section 16.1.1, pg. 81 for the expected values.
void mfrc522_selfTest(mfrc522_t *self, uint8_t* const buffer);

/// To generate 10-bytes random ID using mfrc522 Generate RandomID command
/// \param[in/out] self Instance of mfrc522 to use.
/// \param[out] An array of 10-bytes containing the generated random ID.
void mfrc522_generateRandomID(mfrc522_t *self, uint8_t* const id);

/// To enable/disable the TX antenna.
/// \param[in/out] self Instance of mfrc522 to use.
/// \param[in] enable A value of 1 to enable, other value to disable.
void mfrc522_tx_setEnable(mfrc522_t *self, uint8_t enable);

/// To enable/disable the RX analogue circuit.
/// \param[in/out] self Instance of mfrc522 to use.
/// \param[in] enable A value of 1 to enable, other value to disable.
void mfrc522_rx_setEnable(mfrc522_t *self, uint8_t enable);

/// To power down MFRC522 module
/// \param[in/out] self Instance of mfrc522 to use.
void mfrc522_powerDown(mfrc522_t *self);

/// To set the preset value for the CRC co-processor.
/// \param[in/out] self Instance of mfrc522 to use.
/// \param[in] preset The CRC preset value \ref mfrc522CRCPreset.
void mfrc522_crc_setPreset(mfrc522_t *self, mfrc522CRCPreset preset);

/// To calculate CRC of the specified data stream using MFRC522 CRC co-processor
/// \param[in/out] self Instance of mfrc522 to use.
/// \param[in] data An array of bytes containing data whose CRC is to be calculated.
/// \param[in] len The length of data array.
/// \param[out] crc The calculated 2-byte CRC. Only valid if this function returns
///		MFRC522_CRC_READY.
/// \return The status code for CRC calculations \ref mfrc522CrcError.
uint8_t mfrc522_crc_calc(mfrc522_t *self, const uint8_t* const data, uint32_t len, uint8_t* const crc);


/// To set the timer prescale value. Timer period is calculated by:
/// tperiod = (2*prescale + 1)/(13.56MHz) if TPrescalEven in DemodReg is clear
/// tperiod = (2*prescale + 2)/(13.56MHz) if TPrescalEven in DemodReg is set
/// \param[in/out] self Instance of mfrc522 to use.
/// \param[in] prescale The timer prescale value. Only the 10 LSB bits are used.
void mfrc522_timer_setPrescale(mfrc522_t *self, uint16_t prescale);

/// To set the timer reload value. When the timer is started, the timer will
/// start decrementing from this reload value towards zero.
/// \param[in/out] self Instance of mfrc522 to use.
/// \param[in] value The 16-bit timer reload value.
void mfrc522_timer_loadValue(mfrc522_t *self, uint16_t value);

/// To get the current timer value. This function is not to get the timer reload
/// value, but to get the timer's current counting value.
/// \param[in/out] self Instance of mfrc522 to use.
/// \return The timer's current counting value in 16-bit unsigned integer
uint16_t mfrc522_timer_getValue(mfrc522_t *self);


/// To set the mode for the IRQ pin of MFRC522 module. WIll not affect
/// the bits in IRQ registers.
/// \param[in/out] self Instance of mfrc522 to use.
/// \param[in] mode The mode to set IRQ pin to be normal or inverted,
/// 	and standard CMOS or open drain. \ref mfrc522IrqMode
void mfrc522_irq_setMode(mfrc522_t *self, mfrc522IrqMode mode);

/// To enable the specified IRQ to be propagated to the IRQ pin
/// \param[in/out] self Instance of mfrc522 to use.
/// \param[in] irq The selected IRQ to be propagated to the IRQ pin.
/// 	Bit 1 enable the selected IRQ, bit 0 makes the selected IRQ
///		remains unchanged.
void mfrc522_irq_enable(mfrc522_t *self, uint16_t irq);

/// To disable the specified IRQ from being propagated to the IRQ pin
/// \param[in/out] self Instance of mfrc522 to use.
/// \param[in] irq The selected IRQ NOT to be propagated to the IRQ pin.
/// 	Bit 1 disable the selected IRQ, bit 0 makes the selected IRQ
///		remains unchanged.
void mfrc522_irq_disable(mfrc522_t *self, uint16_t irq);

/// To clear the selected IRQ in the IRQ status bit.
/// \param[in/out] self Instance of mfrc522 to use.
/// \param[in] irq The selected IRQ status bit will be cleared.
/// 	Bit 1 clears the selected IRQ, bit 0 makes the selected IRQ
///		remains unchanged.
void mfrc522_irq_clear(mfrc522_t *self, uint16_t irq);

/// To set the selected IRQ in the IRQ status bit. This function is
/// only provided for completeness and should not be used under normally.
/// \param[in/out] self Instance of mfrc522 to use.
/// \param[in] irq The selected IRQ status bit will be set.
/// 	Bit 1 sets the selected IRQ, bit 0 makes the selected IRQ
///		remains unchanged.
void mfrc522_irq_set(mfrc522_t *self, uint16_t irq);

/// To get the IRQ status for both REG_COM_IRQ and REG_DIV_IRQ, and convert
/// them to 16-bit word to be compatible with the IRQ enum \ref mfrc522Irq.
/// \param[in/out] self Instance of mfrc522 to use.
/// \return The value in REG_COM_IRQ | REG_DIV_IRQ.
uint16_t mfrc522_irq_status(mfrc522_t *self);


/// To send command to MFRC522 module using SPI.
/// \param[in/out] self Instance of mfrc522 to use.
/// \param[in] command A command to be sent.
/// \param[in] block A flag to block the function from returning until
/// 	the command finish execution. \ref mfrc522CmdBlock.
void mfrc522_sendCmd(mfrc522_t *self, uint8_t command, mfrc522CmdBlock block);

/// To read from the FIFO buffer on MFRC522 module.
/// \param[in/out] self Instance of mfrc522 to use.
/// \param[out] buffer A char array to stored the data read.
/// \param[in] len The number of bytes to read from FIFO buffer.
/// \return The number of byte read. STATUS_ERROR is read failed.
int32_t mfrc522_fifo_read(mfrc522_t *self, uint8_t* const buffer, uint32_t len);

/// To write to the FIFO buffer on MFRC522 module.
/// \param[in/out] self Instance of mfrc522 to use.
/// \param[in] buffer A char array containing data to be written.
/// \param[in] len The number of bytes to write to FIFO buffer.
/// \return The number of byte written. STATUS_ERROR is read failed.
int32_t mfrc522_fifo_write(mfrc522_t *self, const uint8_t* buffer, uint32_t len);


/// To read the value of a number of registers on MFRC522 module.
/// \param[in/out] self Instance of mfrc522 to use.
/// \param[in/out] buffer A char array containing the list of register addresses to be read.
///	    The value of the register read will be stored back to the buffer in order.
///	    It is possible to read from the same register address by providing same register
///     address in the array.
/// \param[in] len The number of registers to be read.
/// \return The number of byte read. STATUS_ERROR is read failed.
int32_t mfrc522_readRegN(mfrc522_t *self, uint8_t* const buffer, uint32_t len);

/// To update masked bit(s) of the selected register address with the given value.
/// \param[in/out] self Instance of mfrc522 to use.
/// \param[in] regAddr The register address to write value to.
/// \param[in] mask The mask specifying which bit to be updated with the given data.
/// \param[in] data The data byte to write to the specified register address.
/// \return STATUS_OK if succeed, STATUS_ERROR otherwise.
int32_t mfrc522_updateReg(mfrc522_t *self, uint8_t regAddr, uint8_t mask, uint8_t data);

/// To write a number of values to the same register address on MFRC522 module.
/// \param[in/out] self Instance of mfrc522 to use.
/// \param[in] regAddr The register address to which multiple bytes are written.
/// \param[in] data A char array containing a list of data to be written to the given register
///            addresses. It is possible to write same data by providing same data value
///            in the array.
/// \param[in] len The number of data to be written.
/// \return The number of byte written. STATUS_ERROR is read failed.
int32_t mfrc522_writeRegN(mfrc522_t *self, uint8_t regAddr, const uint8_t* const data, uint32_t len);


#ifdef __cplusplus
}
#endif

#endif
