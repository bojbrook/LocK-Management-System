/*
 * PN532.c
 *
 *  Created on: Mar 6, 2018
 *      Author: Sam
 */

#include "PN532.h"
void Delay(uint8_t delay){
    uint32_t i = 0;
    for(i = 0; i < delay; i++);
}

void SlaveSelect(uint8_t onOrOff){
    if(onOrOff == SLAVESELECTON){
        GPIO_write(CC3220S_LAUNCHXL_PIN_15, 0);
        Delay(500);
        //usleep(1);
    }else if(onOrOff == SLAVESELECTOFF){
        //usleep(1);
        Delay(500);
        GPIO_write(CC3220S_LAUNCHXL_PIN_15, 1);
    }else{
        //error
    }
}

void InitSPI(void){
// 	spiParams.transferMode = SPI_MODE_BLOCKING;
	spiParams.transferMode = SPI_MODE_CALLBACK;
	spiParams.transferTimeout = SPI_WAIT_FOREVER;
	spiParams.transferCallbackFxn = NULL;
	spiParams.mode = SPI_MASTER;
// 	spiParams.bitRate = 100000;         //0.1 MHz
    spiParams.bitRate = 50000;
	spiParams.dataSize = 8;       // 8-bit data size
	spiParams.frameFormat = SPI_POL0_PHA0;
    // spiParams.frameFormat = 5;
	spiParams.custom = NULL;
	SPI_Params_init(&spiParams);  // Initialize SPI parameters
	spi = SPI_open(Board_SPI0, &spiParams);
}

uint8_t SwapEndian(uint8_t data){
       data = (data & 0xF0) >> 4 | (data & 0x0F) << 4;
       data = (data & 0xCC) >> 2 | (data & 0x33) << 2;
       data = (data & 0xAA) >> 1 | (data & 0x55) << 1;
       return data;
//    uint8_t swappedData = 0;
//    swappedData |= (data&0x01) << 8;
//    swappedData |= (data&0x02) << 7;
//    swappedData |= (data&0x04) << 6;
//    swappedData |= (data&0x08) << 5;
//    swappedData |= (data&0x10) << 4;
//    swappedData |= (data&0x20) << 3;
//    swappedData |= (data&0x40) << 2;
//    swappedData |= (data&0x80) << 1;

//    return swappedData;
}

uint8_t WriteSPI(uint8_t *txData, uint8_t dataSize){
    uint8_t i;
    //uint8_t *transmit = (uint8_t*) calloc(dataSize, sizeof(uint8_t));
    uint8_t transmit[dataSize];
    //CC3220S is MSB first, PN532 needs LSB first
    for(i = 0; i < dataSize; i++){
        transmit[i] = SwapEndian(txData[i]);
        //transmit[i] = txData[i];
    }


    spiTransaction.count = dataSize;
    spiTransaction.txBuf = transmit;
    spiTransaction.rxBuf = NULL;
    
    SlaveSelect(SLAVESELECTON);
    transferOK = SPI_transfer(spi, &spiTransaction);
    SlaveSelect(SLAVESELECTOFF);
    
    //free(transmit);
    if (!transferOK) {
        GPIO_write(Board_GPIO_LED0, Board_GPIO_LED_OFF);
        // Error in SPI or transfer already in progress.
        return ERROR;
    }
    return SUCCESS;
}

//txData needs to be all 0s
uint8_t ReadSPI(uint8_t *txData, uint8_t *rxData, uint8_t dataSize){
    uint8_t i;
    //uint8_t *temp = (uint8_t*) calloc(dataSize, sizeof(uint8_t));
    //uint8_t temp[dataSize];
    
    uint8_t transmit[dataSize];
    //CC3220S is MSB first, PN532 needs LSB first
    for(i = 0; i < dataSize; i++){
        transmit[i] = SwapEndian(txData[i]);
        //transmit[i] = txData[i];
    }

    spiTransaction.count = dataSize;
    spiTransaction.txBuf = transmit;
    spiTransaction.rxBuf = rxData;
    
    SlaveSelect(SLAVESELECTON);
    transferOK = SPI_transfer(spi, &spiTransaction);
    SlaveSelect(SLAVESELECTOFF);
    
    if (!transferOK) {
        GPIO_write(Board_GPIO_LED0, Board_GPIO_LED_OFF);
        // Error in SPI or transfer already in progress.
        return ERROR;
    }
    //swap endian
    for(i = 0; i < dataSize; i++){
        rxData[i] = SwapEndian(rxData[i]);
    }
    // for(i = 0; i <dataSize; i++){
    //     rxData[i] = temp[i];
    // }
    //free(temp);
    return SUCCESS;
}
//recheck this
//writeOrRead = SPI_DATAWRITE, 0x01
//writeOrRead = SPI_STATUSREAD, 0x02
//writeOrRead = SPI_DATAREAD, 0x02
//first byte of buffer needs to have command
//after first byte of buffer is the data if needed
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
    // tx[bufferSize + 7] = ~checksum;             //2s bit complement LEN+LCS==00h
    tx[bufferSize + 8] = POSTAMBLE;

    //SlaveSelect(SLAVESELECTON);
    WriteSPI(tx, sizeof(tx));       //transmit everything
    //Slave(SLAVESELECTOFF);

    //free(tx);
}


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


uint8_t SendCommandAndACK(uint8_t *commandCodeAndData, uint8_t commandCodeAndDataLength){
    uint8_t spiStatusRead[2] = {SPI_STATUSREAD, 0};
    uint8_t rxData[7] = {0};
    uint8_t zero[0] = {0};
    uint8_t readData[7] = {SPI_DATAREAD, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    uint8_t timer = 0;



    //send command
    WriteFramePN532(SPI_DATAWRITE, commandCodeAndData, commandCodeAndDataLength);       //size 1 byte

    //write read status and wait for ACK
    //ReadSPI(spiStatusRead, rxData, 2);
    while(rxData[1] != SPI_STATUS_READ_READY){      //keep doing this until timeout
        ReadSPI(spiStatusRead, rxData, 2);
        timer++;
        if(timer > WAITTIMEOUT){
            // return ERROR;
            return 6;
        }
    }
    
    ReadSPI(readData, rxData, 7);        //this writes read data
    if(!CheckACK(rxData)){              //checks if correct ACK response
        // return ERROR;
        return 2;
    }
    
    timer = 0;
    //ReadSPI(spiStatusRead, rxData, 2);
    while(rxData[1] != SPI_STATUS_READ_READY){      //keep doing this until timeout
        ReadSPI(spiStatusRead, rxData, 2);
        timer++;
        if(commandCodeAndData[0] == INLISTPASSIVETARGET){
            if(timer > 1000){
                return TIMEOUT;
            }
        }else{
            if(timer > WAITTIMEOUT){
                return TIMEOUT;
                // return ERROR;
                // return 8;
            }
        }
    }
    return SUCCESS;
}

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
        // return test0;
    }
    //read the data
    ReadSPI(readData, rxData, firmwareVersionResponseLength);
    for(i = 0; i < 6; i++){     //search through first 6 to see if packet is correct
        if(rxData[i] == PREAMBLE){
            if(rxData[i+1] == STARTCODE1){
                if(rxData[i+2] == STARTCODE2){
                    if(rxData[i+5] == PN532TOHOST){
                        if(rxData[i+6] == GETFIRMWAREVERSION + 0x01){
                            packetCheck = i;      //pre increment
                            i = 6;
                        }
                    }
                }
            }
        }
    }

    if(packetCheck == 0xFF){        //the packet check didn't work out
        // return 4;
        return ERROR;
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
uint8_t ConfigureSAM(void){
    uint8_t configureSAM[4] = {0};
    uint8_t configureSAMResponseLength = 10;
    uint8_t readData[configureSAMResponseLength];
    uint8_t rxData[configureSAMResponseLength];
    uint8_t i, packetCheck = 0xFF;

    configureSAM[0] = SAMCONFIGURATION;
    configureSAM[1] = 0x01;     //normal mode, SAM not used, default
    configureSAM[2] = 20;       //timeout of 1 second; 20*50ms = 1s
    configureSAM[3] = 0x01;     //default is 0x01, uses interrupt pin; might have to have interrupt pin in future

    for(i = 0; i < configureSAMResponseLength; i++){     //makes all zeros
        readData[i] = 0;
        rxData[i] = 0;
    }
    readData[0] = SPI_DATAREAD;      //sets the first element of array to read data

    if(SendCommandAndACK(configureSAM, 4) != SUCCESS){
        return ERROR;
    }
    ReadSPI(readData, rxData, configureSAMResponseLength);
    for(i = 0; i < 6; i++){     //search through first 6 to see if packet is correct
        if(rxData[i] == PREAMBLE){
            if(rxData[i+1] == STARTCODE1){
                if(rxData[i+2] == STARTCODE2){
                    if(rxData[i+5] == PN532TOHOST){
                        if(rxData[i+6] == SAMCONFIGURATION + 0x01){
                            packetCheck = i;
                        }
                    }
                }
            }
        }
    }


    if(packetCheck == 0xFF){        //the packet check didn't work out
        return ERROR;
    }
    if(rxData[packetCheck+6] == 0x15){      //no errors if returns 0x15
        return SUCCESS;
    }else{
        return ERROR;
    }
}

//page 116
uint32_t DetectPassiveTarget(uint8_t cardType){

    uint8_t detectPassiveTarget[3] = {0};
    uint8_t detectPassiveTargetLength = 22;
    uint8_t readData[detectPassiveTargetLength];
    uint8_t rxData[detectPassiveTargetLength];
    uint8_t i, packetCheck = 0xFF, targetCount = 1;


    //return data
    uint32_t cardData = 0;
    uint16_t sensRes = 0;       //not sure what this is
    uint8_t selRes = 0;         //not sure what this is either


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
    for(i = 0; i < 6; i++){     //search through first 6 to see if packet is correct
        if(rxData[i] == PREAMBLE){
            if(rxData[i+1] == STARTCODE1){
                if(rxData[i+2] == STARTCODE2){
                    if(rxData[i+5] == PN532TOHOST){
                        if(rxData[i+6] == INLISTPASSIVETARGET + 0x01){
                            //this is extra one for only this function
                            if(rxData[i+7] == targetCount){
                                packetCheck = i;
                                i = 6;
                            }
                        }
                    }
                }
            }
        }
    }
    if(packetCheck == 0xFF){        //the packet check didn't work out
        return 4;
    }
    
    
    
    sensRes = rxData[packetCheck+9];
    sensRes = sensRes << 8;
    sensRes |= rxData[packetCheck+10];
    selRes = rxData[packetCheck+11];

    for(i = 0; i < rxData[packetCheck+12]; i++){    //rxData[12] is the ID length; should be at most 4 bytes
        cardData = cardData << 8;                   //first time this happens shouldn't do anything because already all 0
        cardData |= rxData[packetCheck+13+i];
    }

    return cardData;
}


