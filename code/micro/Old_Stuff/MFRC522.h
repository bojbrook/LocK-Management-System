#include <unistd.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <ti/drivers/SPI.h>
#include "Board.h"

/*MFRC522 Registers 
* Defined on page 36 of datasheet
*
*/
//Command and Status Registers
#define     COMMANDREG            0x01   
#define     COMMIENREG            0x02   
#define     DIVLENREG             0x03   
#define     COMMIRQREG            0x04   
#define     DIVIRQREG             0x05
#define     ERRORREG              0x06   
#define     STATUS1REG            0x07   
#define     STATUS2REG            0x08   
#define     FIFODATAREG           0x09
#define     FIFOLEVELREG          0x0A
#define     WATERLEVELREG         0x0B
#define     CONTROLREG            0x0C
#define     BITFRAMINGREG         0x0D
#define     COLLREG               0x0E
//Command Registers
#define     MODEREG               0x11
#define     TXMODEREG             0x12
#define     RXMODEREG             0x13
#define     TXCONTROLREG          0x14
#define     TXAUTOREG             0x15
#define     TXSELREG              0x16
#define     RXSELREG              0x17
#define     RXTHRESHOLDREG        0x18
#define     DEMODREG              0x19
#define     MIFARETX              0x1C
#define     MIFARERX              0x1D
#define     SERIALSPEEDREG        0x1F
//Configuration Registers
#define     CRCRESULTREGM         0x21
#define     CRCRESULTREGL         0x22
#define     MODWIDTHREG           0x24
#define     RFCFGREG              0x26
#define     GSNREG                0x27
#define     CWGSCFGREG            0x28
#define     MODGSCFGREG           0x29
#define     TMODEREG              0x2A
#define     TPRESCALERREG         0x2B
#define     TRELOADREGH           0x2C
#define     TRELOADREGL           0x2D
#define     TCOUNTERVALUEREGH     0x2E
#define     TCOUNTERVALUEREGL     0x2F
//Test Registers  
#define     TESTSEL1REG           0x31
#define     TESTSEL2REG           0x32
#define     TESTPINENREG          0x33
#define     TESTPINVALUEREG       0x34
#define     TESTBUSREG            0x35
#define     AUTOTESTREG           0x36
#define     VERSIONREG            0x37
#define     ANALOGTESTREG         0x38
#define     TESTDAC1REG           0x39 
#define     TESTDAC2REG           0x3A  
#define     TESTADCREG            0x3B

//Command Overview
#define     IDLE					0x00	//0b0000
#define     MEM           			0x01	//0b0001
#define     RANDOMID				0x02	//0b0010
#define     CALCCRC         		0x03	//0b0011
#define     TRANSMIT           		0x04 	//0b0100
#define     NOCMDCHANGE           	0x07  	//0b0111
#define     RECEIVE            		0x08	//0b1000
#define     TRANSCEIVE            	0x0C	//0b1100
#define     MFAUTHENT           	0x0E	//0b1110
#define     SOFTRESET            	0x0F	//0b1111


#define		SETBIT(data,mask)		(data|mask)
#define		CLEARBIT(data,mask)		(data&~mask)
#define		TOGGLEBIT(data,mask)	(data^mask)
#define		READBIT(data,mask)		(data&mask)

#define		MAXLENGTH				16

typedef enum{
	RETURNFAIL = -1,
	RETURNTIMEOUT,
	RETURNOK,
}returnStatusMFRC;


void InitMFRC(SPI_Handle spi, *SPI_Transaction spiTransaction);
void WriteMFRC(uint8_t address, char value, SPI_Handle spi, *SPI_Transaction spiTransaction);
uint8_t ReadMFRC(uint8_t address, SPI_Handle spi, *SPI_Transaction spiTransaction);
void AntennaOn(SPI_Handle spi, *SPI_Transaction spiTransaction);
void AntennaOff(SPI_Handle spi, *SPI_Transaction spiTransaction);
void CloseMFRC(SPI_Handle spi, *SPI_Transaction spiTransaction);
void SetBitMFRC(uint8_t address, uint8_t mask, SPI_Handle spi, *SPI_Transaction spiTransaction);
void ClearBitMFRC(uint8_t address, uint8_t mask, SPI_Handle spi, *SPI_Transaction spiTransaction);
uint8_t MFRCToCard(uint8_t command, uint8_t *cardData, uint8_t cardLength, uint8_t *returnData, uint8_t *returnLength, SPI_Handle spi, *SPI_Transaction spiTransaction);
uint8_t CardFind(uint8_t *tagType, SPI_Handle spi, *SPI_Transaction spiTransaction);
uint8_t CardSerialNumber(uint8_t *serialNumber, SPI_Handle spi, *SPI_Transaction spiTransaction);


