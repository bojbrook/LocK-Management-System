/*
 * PN532.c
 *
 *  Created on: Mar 6, 2018
 *      Author: Sam
 */

#include "PN532.h"


/*
 * manual delay
 */
void Delay(uint8_t delay){
    uint32_t i = 0;
    for(i = 0; i < delay; i++);
}



/*
 * Bit bang slave select with GPIO, hardware SPI SS is not compatible with PN532's nonstandard SPI
 */
void SlaveSelect(uint8_t onOrOff){
    if(onOrOff == SLAVESELECTON){
        GPIO_write(CC3220S_LAUNCHXL_PIN_15, 0);
        usleep(1000);       //1000 microseconds
    }else if(onOrOff == SLAVESELECTOFF){
        usleep(100);
        GPIO_write(CC3220S_LAUNCHXL_PIN_15, 1);
    }else{
        //error
    }
}



/*
 * initializes SPI
 */
void InitSPI(void){
    spiParams.transferMode = SPI_MODE_CALLBACK;
    spiParams.transferTimeout = SPI_WAIT_FOREVER;
    spiParams.transferCallbackFxn = NULL;
    spiParams.mode = SPI_MASTER;
//  spiParams.bitRate = 100000;                     //0.1 MHz
    spiParams.bitRate = 50000;                      //50 kHz
    spiParams.dataSize = 8;                         // 8-bit data size
    spiParams.frameFormat = SPI_POL0_PHA0;
    spiParams.custom = NULL;
    SPI_Params_init(&spiParams);                    // Initialize SPI parameters
    spi = SPI_open(Board_SPI0, &spiParams);
}


/*
 * swaps endianness
 */
uint8_t SwapEndian(uint8_t data){
       data = (data & 0xF0) >> 4 | (data & 0x0F) << 4;
       data = (data & 0xCC) >> 2 | (data & 0x33) << 2;
       data = (data & 0xAA) >> 1 | (data & 0x55) << 1;
       return data;
}


/*
 * writes to sensor over SPI
 */
uint8_t WriteSPI(uint8_t *txData, uint8_t dataSize){
    uint8_t i;
    uint8_t transmit[dataSize];
    //CC3220S is MSB first, PN532 needs LSB first
    for(i = 0; i < dataSize; i++){
        transmit[i] = SwapEndian(txData[i]);
    }


    spiTransaction.count = dataSize;
    spiTransaction.txBuf = transmit;
    spiTransaction.rxBuf = NULL;
    
    SlaveSelect(SLAVESELECTON);
    transferOK = SPI_transfer(spi, &spiTransaction);
    SlaveSelect(SLAVESELECTOFF);
    
    if (!transferOK) {
        GPIO_write(Board_GPIO_LED0, Board_GPIO_LED_OFF);
        // Error in SPI or transfer already in progress.
        return ERROR;
    }
    return SUCCESS;
}



/*
 * This function reads from the PN532. Must send x number of bytes in order to
 * read x number of bytes
 */
//txData needs to be all 0s
uint8_t ReadSPI(uint8_t *txData, uint8_t *rxData, uint8_t dataSize){
    uint8_t i;
    uint8_t *transmit = (uint8_t*) malloc(sizeof(uint8_t)*dataSize);
    //CC3220S is MSB first, PN532 needs LSB first
    for(i = 0; i < dataSize; i++){
        transmit[i] = SwapEndian(txData[i]);
    }

    spiTransaction.count = dataSize;
    spiTransaction.txBuf = transmit;
    spiTransaction.rxBuf = rxData;
    
    SlaveSelect(SLAVESELECTON);
    transferOK = SPI_transfer(spi, &spiTransaction);
    SlaveSelect(SLAVESELECTOFF);
    free(transmit);
    
    if (!transferOK) {
        GPIO_write(Board_GPIO_LED0, Board_GPIO_LED_OFF);
        // Error in SPI or transfer already in progress.
        return ERROR;
    }
    //swap endian
    for(i = 0; i < dataSize; i++){
        rxData[i] = SwapEndian(rxData[i]);
    }
    return SUCCESS;
}


/*
 * This function creates the frame for sending to PN532
 */
void WriteFramePN532(uint8_t writeOrRead, uint8_t *buffer, uint8_t bufferSize){
    //page 9 of application note
    uint8_t i = 0, checksum = 0;
    uint8_t tx[bufferSize + 9];
    uint8_t TFI;                //byte 5 of frame
    if(writeOrRead == SPI_DATAWRITE){
        TFI = HOSTTOPN532;      //0xD4
    }else if (writeOrRead == SPI_STATUSREAD || writeOrRead == SPI_DATAREAD){
        TFI = PN532TOHOST;      //0xD5
    }else{
        //error
    }

    tx[0] = writeOrRead;
    tx[1] = PREAMBLE;
    tx[2] = STARTCODE1;
    tx[3] = STARTCODE2;
    tx[4] = bufferSize + 1;                 //+1 for TFI
    tx[5] = ~tx[4] + 1;                     //2s bit complement LEN+LCS==00h
    tx[6] = TFI;
    checksum += tx[6];
    for(i = 0; i < bufferSize; i++){        //the actual data
        tx[i+7] = buffer[i];                //buffer[0] should be command code
        checksum += buffer[i];
    }
    tx[bufferSize + 7] = ~checksum + 1;     //2s bit complement LEN+LCS==00h
    tx[bufferSize + 8] = POSTAMBLE;

    WriteSPI(tx, sizeof(tx));       //transmit everything
}



/*
 * checks to see if return frame is correct, also returns index at which return buffer starts
 */
uint8_t frameCheck(uint8_t *rxData, uint8_t commandOut){
    //returns index to start at
    int i = ERROR;
    for(i = 0; i < 10; i++){
        if(rxData[i] == PREAMBLE){
            if(rxData[i+1] == STARTCODE1){
                if(rxData[i+2] == STARTCODE2){
                    if(rxData[i+5] == PN532TOHOST){
                        if(rxData[i+6] == commandOut){
                            return i;   //index to start at
                        }
                    }
                }
            }
        }
    }
    return ERROR;
}


/*
 * Checks the acknowledgement from the PN532
 */
uint8_t CheckACK(uint8_t *rxData){
    if(rxData[1] == 0x00
        & rxData[2] == 0x00
        & rxData[3] == 0xFF
        & rxData[4] == 0x00
        & rxData[5] == 0xFF
        & rxData[6] == 0x00){
            return SUCCESS;
        }else{
            return ERROR;
        }
}



/*
 * Sends command and checks acknowledgement
 */
uint8_t SendCommandAndACK(uint8_t *commandCodeAndData, uint8_t commandCodeAndDataLength){
    uint8_t spiStatusRead[2] = {SPI_STATUSREAD, 0};
    uint8_t rxData[7] = {0, 0, 0, 0, 0, 0, 0};
    uint8_t readData[7] = {SPI_DATAREAD, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    uint8_t timer = 0;

    //send command
    WriteFramePN532(SPI_DATAWRITE, commandCodeAndData, commandCodeAndDataLength);       //size 1 byte

    //write read status and wait for ACK
    ReadSPI(spiStatusRead, rxData, 2);
    while(rxData[1] != SPI_STATUS_READ_READY){      //keep doing this until timeout
        ReadSPI(spiStatusRead, rxData, 2);
        timer++;
        if(timer > WAITTIMEOUT){
            return ERROR;
        }
    }
    
    ReadSPI(readData, rxData, 7);        //this writes read data
    if(!CheckACK(rxData)){              //checks if correct ACK response
        return ERROR;
    }
    
    timer = 0;
    while(rxData[1] != SPI_STATUS_READ_READY){      //keep doing this until timeout
        ReadSPI(spiStatusRead, rxData, 2);
        timer++;
        if(commandCodeAndData[0] == INLISTPASSIVETARGET){
            if(timer > TIMEOUTTEST){
                return TIMEOUT;
            }
            usleep(100);
        }else{
            if(timer > WAITTIMEOUT){
                return 8;
            }
            usleep(100);
        }
    }
    return SUCCESS;
}



/*
 * gets firmware version for PN532, also sensor init command for some reason
 */
uint32_t GetFirmwareVersion(void){
    uint8_t getFirmwareVersion[1] = {GETFIRMWAREVERSION};
    uint8_t firmwareVersionResponseLength = 14;
    uint8_t readData[firmwareVersionResponseLength];
    uint8_t rxData[firmwareVersionResponseLength];
    uint8_t i, packetCheck = 0xFF;

    //return data
    uint32_t firmwareVersion = 0;
    for(i = 0; i < firmwareVersionResponseLength; i++){     //makes all zeros
        readData[i] = 0;
        rxData[i] = 0;
    }
    readData[0] = SPI_DATAREAD;       //sets the last element of array to read data

    uint8_t test0 = SendCommandAndACK(getFirmwareVersion, 1);
    if(test0 != SUCCESS){
        return ERROR;

    }
    //read the data
    ReadSPI(readData, rxData, firmwareVersionResponseLength);
    uint8_t startIndex = frameCheck(rxData, GETFIRMWAREVERSION + 0x01);
    if(startIndex == ERROR){
        return SUCCESS;
    }else{
        packetCheck = startIndex;
    }

    firmwareVersion |= rxData[packetCheck + 6];
    firmwareVersion = firmwareVersion << 8;
    firmwareVersion |= rxData[packetCheck + 7];
    firmwareVersion = firmwareVersion << 8;
    firmwareVersion |= rxData[packetCheck + 8];
    firmwareVersion = firmwareVersion << 8;
    firmwareVersion |= rxData[packetCheck + 9];
    firmwareVersion = firmwareVersion << 8;

    return firmwareVersion;
}


//page 89
/*
 * Configures Security Access Module
 */
uint8_t ConfigureSAM(void){
    uint8_t configureSAM[4] = {0, 0, 0, 0};
    uint8_t configureSAMResponseLength = 10;
    uint8_t readData[configureSAMResponseLength];
    uint8_t rxData[configureSAMResponseLength];
    uint8_t i, packetCheck = 0xFF;

    configureSAM[0] = SAMCONFIGURATION;
    configureSAM[1] = 0x01;     //normal mode, SAM not used, default
    configureSAM[2] = 40;       //timeout of 1 second; 40*50ms = 2s
    configureSAM[3] = 0x01;     //default is 0x01, uses interrupt pin; might have to have interrupt pin in future

    for(i = 0; i < configureSAMResponseLength; i++){     //makes all zeros
        readData[i] = 0;
        rxData[i] = 0;
    }
    readData[0] = SPI_DATAREAD;      //sets the first element of array to read data

    uint8_t test = SendCommandAndACK(configureSAM, 4);
    if(test != SUCCESS){
//        return test;
        return ERROR;
    }
    ReadSPI(readData, rxData, configureSAMResponseLength);
    uint8_t startIndex = frameCheck(rxData, SAMCONFIGURATION + 0x01);
    if(startIndex == ERROR){
        return 37;
    }else{
        packetCheck = startIndex;
    }
    if(rxData[packetCheck+6] == 0x15){      //no errors if returns 0x15
        return SUCCESS;
    }else{
        return ERROR;
    }
}



//page 116
/*
 * Detects Passive Target, i.e. card, passive phone in HCE
 */
uint32_t DetectPassiveTarget(uint8_t cardType){
    uint8_t detectPassiveTarget[3] = {0, 0, 0};
    uint8_t detectPassiveTargetLength = 22;
    uint8_t readData[detectPassiveTargetLength];
    uint8_t rxData[detectPassiveTargetLength];
    uint8_t i, packetCheck = 0xFF, targetCount = 1;


    //return data
    uint32_t cardData = 0;
    uint16_t sensRes = 0;       //not sure what this is


    detectPassiveTarget[0] = INLISTPASSIVETARGET;
    detectPassiveTarget[1] = targetCount;                       //how many cards to read, max is 2 but we shouldn't ever read more than 1
    detectPassiveTarget[2] = cardType;                          //depends on the type of card being read, defines baud rate as well
    for(i = 0; i < detectPassiveTargetLength; i++){     //makes all zeros
        readData[i] = 0;
        rxData[i] = 0;
    }
    readData[0] = SPI_DATAREAD;       //sets the first element of array to read data

    uint8_t tester = SendCommandAndACK(detectPassiveTarget, 3);
    if(tester != SUCCESS){
        return tester;
    }
    ReadSPI(readData, rxData, detectPassiveTargetLength);
    uint8_t startIndex = frameCheck(rxData, INLISTPASSIVETARGET + 0x01);
    if(startIndex == ERROR){   //didn't work out
        return 4;
    }else{
        packetCheck = startIndex;
    }

    sensRes = rxData[packetCheck+9];
    sensRes = sensRes << 8;
    sensRes |= rxData[packetCheck+10];

    //this for loop is for ID
    cardData = 0;
    for(i = 0; i < rxData[packetCheck+12]; i++){    //rxData[12] is the ID length; should be at most 4 bytes
        cardData = cardData << 8;                   //first time this happens shouldn't do anything because already all 0
        cardData |= rxData[packetCheck+13+i];
    }
    return cardData;
}



/*
 * Exchanges data while looking for passive cards
 */
uint8_t InDataExchange(void){
    uint8_t inDataExchange[13] = {INDATAEXCHANGE,
                                    0x01,       //number of targets??
                                    //start of data out
                                    0x00,       //CLAss?
                                    0xA4,       //INS
                                    0x04,       //P1
                                    0x00,       //P2
                                    0x05,       //length
                                    0xF2, 0x22, 0x22, 0x22, 0x22,       //AID on android app
                                    0x00
    };

    /*SENDCOMMANDANDACK()*/
    int timer;
    uint8_t spiStatusRead[2] = {SPI_STATUSREAD, 0};
    uint8_t rxData[7] = {0, 0, 0, 0, 0, 0, 0};
    uint8_t readData[7] = {SPI_DATAREAD, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    //send command
    WriteFramePN532(SPI_DATAWRITE, inDataExchange, 12);       //size 1 byte

    //write read status and wait for ACK
    ReadSPI(spiStatusRead, rxData, 2);
    while(rxData[1] != SPI_STATUS_READ_READY){      //keep doing this until timeout
        ReadSPI(spiStatusRead, rxData, 2);
        timer++;
        if(timer > WAITTIMEOUT){
            return TIMEOUT;
        }
    }
    ReadSPI(readData, rxData, 7);        //this writes read data
    if(!CheckACK(rxData)){              //checks if correct ACK response
         return ERROR;
    }
    /*END OF SENDCOMMANDANDACK()*/
    return SUCCESS;
}



/*
 * initialize sensor as passive target
 */
uint32_t TgInitAsTarget(void){
    uint8_t initTarget[38] = {TGINITASTARGET,       //first command code
                          0x00,                 // the mode
                          0x08, 0x00,           //sens_res, not sure what this is
                          0x12, 0x34, 0x56,     //NFCID1t
                          0x60,                 //DEP- data exchange protocol, 0x60 is both
                          //read application note page 54
                          0x01, 0xFE, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7,
                          0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7,
                          0xFF, 0xFF,
                          //NFCID3t
                          0xAA, 0x99, 0x88, 0x77, 0x66, 0x55, 0x44, 0x33, 0x22, 0x11,
                          0x00,                 //general bytes
                          0x02                 //historical bytes??
    };

    /*SENDCOMMANDANDACK()*/
    int timer;
    uint8_t spiStatusRead[2] = {SPI_STATUSREAD, 0};
    uint8_t rxData[7] = {0, 0, 0, 0, 0, 0, 0};
    uint8_t readData[7] = {SPI_DATAREAD, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    //send command
    WriteFramePN532(SPI_DATAWRITE, initTarget, 38);       //size 1 byte
    //write read status and wait for ACK
    while(rxData[1] != SPI_STATUS_READ_READY){      //keep doing this until timeout
        ReadSPI(spiStatusRead, rxData, 2);
        timer++;
        if(timer > WAITTIMEOUT){
            return TIMEOUT;
        }
    }
    ReadSPI(readData, rxData, 7);        //this writes read data
    if(!CheckACK(rxData)){              //checks if correct ACK response
         return ERROR;
    }
    /*END OF SENDCOMMANDANDACK()*/
    return SUCCESS;
}



/*
 * get data while in passive mode
 */
uint8_t TgGetData(uint8_t *getDataBuffer){
    uint8_t tgGetData[1] = {TGGETDATA};
    uint8_t tgGetDataLength = 30;
    uint8_t readData[tgGetDataLength];
    uint8_t i;


    for(i = 0; i < tgGetDataLength; i++){     //makes all zeros
        readData[i] = 0;
    }
    readData[0] = SPI_DATAREAD;       //sets the first element of array to read data

    uint8_t tester = SendCommandAndACK(tgGetData, 1);
    if(tester != SUCCESS){
        return tester;
    }
    ReadSPI(readData, getDataBuffer, tgGetDataLength);
    if(frameCheck(getDataBuffer, TGGETDATA + 0x01) == ERROR){   //didn't work out
        return 4;
    }else{
        return SUCCESS;
    }
}

/*
 * sends data while in passive mode
 */
uint8_t TgSetData(uint8_t *setDataBuffer, uint8_t setDataBufferLength){
    uint8_t tgSetData[1+setDataBufferLength];
    uint8_t rxData[7] = {0, 0, 0, 0, 0, 0, 0};
    uint8_t readData[7] = {SPI_DATAREAD, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    uint8_t i;
    tgSetData[0] = TGGETDATA;
    for(i = 0; i < setDataBufferLength; i++){
        tgSetData[i+1] = setDataBuffer[i];
    }


    rxData[0] = SPI_DATAREAD;       //sets the first element of array to read data

    uint8_t tester = SendCommandAndACK(tgSetData, 1+setDataBufferLength);
    if(tester != SUCCESS){
        return tester;
    }
    ReadSPI(rxData, readData, 7);
    if(frameCheck(readData, TGSETDATA + 0x01) == ERROR){   //didn't work out
        return 4;
    }else{
        return SUCCESS;
    }
}



//page98
/*
 * power downs pn532
 */
uint8_t PowerDown(void){
    uint8_t powerDown[3] = {0, 0, 0};
    uint8_t powerDownLength = 11;
    uint8_t readData[powerDownLength];
    uint8_t rxData[powerDownLength];
    uint8_t i;


    powerDown[0] = POWERDOWN;
    powerDown[1] = 0x08;                       //RF Level Detector
    powerDown[2] = 0x01;                       //generate IRQ
    for(i = 0; i < powerDownLength; i++){     //makes all zeros
        readData[i] = 0;
        rxData[i] = 0;
    }
    readData[0] = SPI_DATAREAD;       //sets the first element of array to read data

    uint8_t tester = SendCommandAndACK(powerDown, 3);
    if(tester != SUCCESS){
        return tester;
    }
    ReadSPI(readData, rxData, powerDownLength);
    if(frameCheck(rxData, POWERDOWN + 0x01)){
        return SUCCESS;
    }else{
        return ERROR;
    }
}


/*
 * Writes to register
 */
uint8_t SetParameters(uint8_t flag){
    uint8_t setParameters[2] = {SETPARAMETERS, flag};

    /*SENDCOMMANDANDACK()*/
    int timer;
    uint8_t spiStatusRead[2] = {SPI_STATUSREAD, 0};
    uint8_t rxData[7] = {0, 0, 0, 0, 0, 0, 0};
    uint8_t readData[7] = {SPI_DATAREAD, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    //send command
    WriteFramePN532(SPI_DATAWRITE, setParameters, 2);       //size 1 byte

    //write read status and wait for ACK
    ReadSPI(spiStatusRead, rxData, 2);
    while(rxData[1] != SPI_STATUS_READ_READY){      //keep doing this until timeout
        ReadSPI(spiStatusRead, rxData, 2);
        timer++;
        if(timer > WAITTIMEOUT){
            return TIMEOUT;
        }
    }
    ReadSPI(readData, rxData, 7);        //this writes read data
    if(!CheckACK(rxData)){              //checks if correct ACK response
         return ERROR;
    }
    /*END OF SENDCOMMANDANDACK()*/
    return SUCCESS;
}
