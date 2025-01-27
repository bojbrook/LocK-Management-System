/*
 * PN532.h
 *
 *  Created on: Mar 6, 2018
 *      Author: Sam
 */
#include "unistd.h"
#include "Board.h"
#include <ti/drivers/SPI.h>
#include <ti/drivers/GPIO.h>

#ifndef PN532_H_
#define PN532_H_


#define PREAMBLE                0x00
#define STARTCODE1              0x00
#define STARTCODE2              0xFF
#define POSTAMBLE               0x00

#define HOSTTOPN532             0xD4
#define PN532TOHOST             0xD5

//page 193 of User Manual and page 25 of application note, Commands
// PN532 Commands
#define DIAGNOSE                0x00
#define GETFIRMWAREVERSION      0x02
#define GETGENERALSTATUS        0x04
#define READREGISTER            0x06
#define WRITEREGISTER           0x08
#define READGPIO                0x0C
#define WRITEGPIO               0x0E
#define SETSERIALBAUDRATE       0x10
#define SETPARAMETERS           0x12
#define SAMCONFIGURATION        0x14
#define POWERDOWN               0x16

//RF
#define RFCONFIGURATION         0x32
#define RFREGULATIONTEST        0x58

//Initiator
#define INJUMPFORDEP            0x56
#define INJUMPFORPSL            0x46
#define INLISTPASSIVETARGET     0x4A
#define INATR                   0x50
#define INPSL                   0x4E
#define INDATAEXCHANGE          0x40
#define INCOMMUNICATETHRU       0x42
#define INDESELECT              0x44
#define INRELEASE               0x52
#define INSELECT                0x54
#define INAUTOPOLL              0x60

//target
#define TGINITASTARGET          0x8C
#define TGSETGENERALBYTES       0x92
#define TGGETDATA               0x86
#define TGSETDATA               0x8E
#define TGMETADATA              0x94
#define TGGETINITIATORCOMMAND   0x88
#define TGRESPONSETOINITIATOR   0x90
#define TGGETTARGETSTATUS       0x8A


//page 115, baud rate and modulation
#define ISOIEC14443A            0x00        //106kbps
#define FELICA212               0x01        //212kbps
#define FELICA424               0x02        //424kbps
#define ISOIEC144433B           0x03        //106kbps
#define JEWELTAG                0x04        //106kbps, max one target


// // Mifare Commands
// #define MIFARE_CMD_AUTH_A                   (0x60)
// #define MIFARE_CMD_AUTH_B                   (0x61)
// #define MIFARE_CMD_READ                     (0x30)
// #define MIFARE_CMD_WRITE                    (0xA0)
// #define MIFARE_CMD_TRANSFER                 (0xB0)
// #define MIFARE_CMD_DECREMENT                (0xC0)
// #define MIFARE_CMD_INCREMENT                (0xC1)
// #define MIFARE_CMD_STORE                    (0xC2)
// #define MIFARE_ULTRALIGHT_CMD_WRITE         (0xA2)
// #define PN532_MIFARE_READ                   0x30
// #define PN532_MIFARE_WRITE                  0xA0
// #define PN532_AUTH_WITH_KEYA                0x60
// #define PN532_AUTH_WITH_KEYB                0x61
// #define PN532_WAKEUP                        0x55


#define SPI_STATUS_READ_READY           0x01
#define SLAVESELECTON                   0x01
#define SLAVESELECTOFF                  0x00

#define WAITTIMEOUT                     1000       //3 seconds

enum CommandPN532{
    SPI_DATAWRITE = 0x01,
    SPI_STATUSREAD,
    SPI_DATAREAD,
}CommandPN532;

enum ReturnMessage{
    ERROR,
    SUCCESS,
    TIMEOUT
}ReturnMessage;


#define MSGSIZE 20
//global variables
SPI_Handle      spi;
SPI_Params      spiParams;
SPI_Transaction spiTransaction;
bool            transferOK;

/////////////////////////////////////////////////////////////
/*
SendCommandAndACK() should send command and wait for the ACK
Other functions listed below the comment will actually wait for response
and then read the data from the response
*/
///////////////////////////////////////////////////////////
void Delay(uint8_t delay);
void SlaveSelect(uint8_t onOrOff);
void InitSPI(void);
//SwapEndian() should only be called by WriteSPI() and ReadSPI()
uint8_t SwapEndian(uint8_t data);
uint8_t WriteSPI(uint8_t *txData, uint8_t dataSize);
uint8_t ReadSPI(uint8_t *txData, uint8_t *rxData, uint8_t dataSize);

//WriteFramePn532() is called by SendCommandAndACK()
void WriteFramePN532(uint8_t writeOrRead, uint8_t *buffer, uint8_t bufferSize);
//CheckACK() is only called by SendCommandAndACK() and checks predefined ACK response
uint8_t CheckACK(uint8_t *rxData);
uint8_t SendCommandAndACK(uint8_t *commandCodeAndData, uint8_t commandCodeAndDataLength);

//should only need to ever call these functions
uint32_t GetFirmwareVersion(void);
uint8_t ConfigureSAM(void);
uint32_t DetectPassiveTarget(uint8_t cardType);





#endif /* PN532_H_ */
