/*
 * PN532.h
 *
 *  Created on: Mar 6, 2018
 *      Author: Sam
 */
#include "unistd.h"
#include "stdlib.h"


#include "uart_term.h"
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



#define DATAEXCHANGEINDEX               (8)
#define SPI_STATUS_READ_READY           0x01
#define SLAVESELECTON                   0x01
#define SLAVESELECTOFF                  0x00
#define WAITTIMEOUT                     30
#define TIMEOUTTEST                     30
#define PN532FIRMWAREVERSION            (0x32010600)

enum CommandPN532{
    SPI_DATAWRITE = 0x01,
    SPI_STATUSREAD,
    SPI_DATAREAD,
}CommandPN532;

enum ReturnMessage{
    ERROR = 0xFE,
    SUCCESS = 0x01,
    TIMEOUT,
    ACKERROR,
    SPIERROR
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
uint8_t frameCheck(uint8_t *rxdata, uint8_t commandOut);
void Delay(uint8_t delay);
void SlaveSelect(uint8_t onOrOff);
void InitSPI(void);
uint8_t SwapEndian(uint8_t data);
uint8_t WriteSPI(uint8_t *txData, uint8_t dataSize);
uint8_t ReadSPI(uint8_t *txData, uint8_t *rxData, uint8_t dataSize);
void WriteFramePN532(uint8_t writeOrRead, uint8_t *buffer, uint8_t bufferSize);
uint8_t CheckACK(uint8_t *rxData);
uint8_t SendCommandAndACK(uint8_t *commandCodeAndData, uint8_t commandCodeAndDataLength);
uint32_t GetFirmwareVersion(void);
uint8_t ConfigureSAM(void);
uint32_t DetectPassiveTarget(uint8_t cardType);
uint8_t PowerDown(void);
uint32_t TgInitAsTarget(void);
uint8_t InDataExchange(void);
uint8_t TgGetData(uint8_t *getDataBuffer);
uint8_t SetParameters(uint8_t flag);
uint8_t TgSetData(uint8_t *setDataBuffer, uint8_t setDataBufferLength);


#endif /* PN532_H_ */
