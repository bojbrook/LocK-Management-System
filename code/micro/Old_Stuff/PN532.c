#include "PN532.h"

uint8_t SwapEndian(uint8_t data){
	uint8_t swappedData = 0;
	swappedData |= (data&0x01) << 8;
	swappedData |= (data&0x02) << 7;
	swappedData |= (data&0x04) << 6;
	swappedData |= (data&0x08) << 5;
	swappedData |= (data&0x10) << 4;
	swappedData |= (data&0x20) << 3;
	swappedData |= (data&0x40) << 2;
	swappedData |= (data&0x80) << 1;
	
	return swappedData;
}

uint8_t WriteSPI(uint8_t *txData, uint8_t dataSize){
	uint8_t i;
	//uint8_t *transmit = (uint8_t*) calloc(dataSize, sizeof(uint8_t));
	uint8_t transmit[dataSize];
	//CC3220S is MSB first, PN532 needs LSB first
	for(i = 0; i < dataSize; i++){
		transmit[i] = SwapEndian(txData[i]);
	}
	
	spiTransaction->count = dataSize;
	spiTransaction->txBuf = transmit;
	spiTransaction->rxBuf = NULL;
	transferOK = SPI_transfer(spi, &spiTransaction);
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
	uint8_t temp[dataSize];
	
	spiTransaction->count = dataSize;
	spiTransaction->txBuf = txData;
	spiTransaction->rxBuf = rxData;
	transferOK = SPI_transfer(spi, &spiTransaction);
	if (!transferOK) {
		GPIO_write(Board_GPIO_LED0, Board_GPIO_LED_OFF);
		// Error in SPI or transfer already in progress.
		return ERROR;
	}
	//swap endian
	for(i = 0; i < dataSize; i++){
		temp[i] = SwapEndian(rxData[i]);
	}
	for(i = 0; i <dataSize; i++){
		rxData[i] = temp[i];
	}
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
	//uint8_t *tx = (uint8_t*) calloc(bufferSize+9, sizeof(uint8_t));
	uint8_t tx[bufferSize + 9];
	uint8_t TFI;				//byte 5 of frame
	if(command == SPI_DATAWRITE){
		TFI = HOSTTOPN532;		//0xD4
	}else if (command == SPI_STATUSREAD || command == SPI_DATAREAD){
		TFI = PN532TOHOST;		//0xD5
	}else{
		//error
	}
	
	tx[0] = command;
	
	tx[1] = PREAMBLE;
	tx[2] = STARTCODE1;
	tx[3] = STARTCODE2;
	tx[4] = bufferSize + 1;					//+1 for TFI
	tx[5] = ~tx[4] + 1;						//2s bit complement LEN+LCS==00h
	tx[6] = TFI;
	checksum += tx[6];
	for(i = 0; i < bufferSize; i++){		//the actual data
		tx[i+7] = buffer[i];				//buffer[0] should be command code
		checksum += buffer[i];
	}
	tx[bufferSize + 7] = ~checksum + 1;		//2s bit complement LEN+LCS==00h
	tx[bufferSize + 8] = POSTAMBLE;
	
	//SlaveSelect(SLAVESELECTON);
	WriteSPI(tx, sizeof(tx));		//transmit everything
	//Slave(SLAVESELECTOFF);
	
	//free(tx);
}


uint8_t CheckACK(uint8_t *rxData){
	if(rxData[0] == 0x00
		& rxData[1] == 0x00
		& rxData[2] == 0xFF
		& rxData[3] == 0x00
		& rxData[4] == 0xFF
		& rxData[5] == 0x00){
			return SUCCESS;
		}else{
			return ERROR;
		}
}


uint8_t SendCommandAndACK(uint8_t *commandCodeAndData, uint8_t commandCodeAndDataLength){
	uint8_t spiStatusRead[1] = SPI_STATUSREAD;
	uint8_t rxData[6] = {0};
	uint8_t zero[1] = {0};
	uint8_t readData[6] = {0x00, 0x00, 0x00, 0x00, 0x00, SPI_DATAREAD};
	uint8_t timer = 0;
	
	
	//send command
	WriteFramePN532(SPI_DATAWRITE, commandCodeAndData, commandCodeAndDataLength);		//size 1 byte
	
	//wait for ACK
	ReadSPI(spiStatusRead, rxData, 1);
	while(rxData[0] != SPI_STATUS_READ_READY){
		ReadSPI(zero, rxData, 1);
		timer++;
		if(timer > TIMEOUT){
			return ERROR;
		}
	}
	ReadSPI(zeroBuffer, rxData, 6);
	if(!CheckACK(rxData)){
		return ERROR;
	}
	while(rxData[0] != SPI_STATUS_READ_READY){
		ReadSPI(zero, rxData, 1);
		timer++;
		if(timer > TIMEOUT){
			return ERROR;
		}
	}
	return SUCCESS;
}

uint32_t GetFirmwareVersion(void){
	uint8_t getFirmwareVersion[1] = {0};
	uint8_t firmwareVersionResponseLength = 14;
	uint8_t readData[firmwareVersionResponseLength] = {0};
	uint8_t rxData[firmwareVersionResponseLength] = {0};
	uint8_t i, packetCheck = 0xFF;
	
	//return data
	uint32_t firmwareVersion = 0;
	
	getFirmwareVersion[0] = GETFIRMWAREVERSION;
	readData[firmwareVersionResponseLength-1] = SPI_DATAREAD;		//sets the last element of array to read data
	
	if(!SendCommandAndACK(getFirmwareVersion, 1)){
		free(readData);
		free(rxData);
		return ERROR;
	}
	ReadSPI(readData, rxData, firmwareVersionResponseLength);
	for(i = 0; i < 6; i++){		//search through first 6 to see if packet is correct
		if(rxData[i] == PREAMBLE){
			if(rxData[i+1] == STARTCODE1){
				if(rxData[i+2] == STARTCODE2){
					if(rxData[i+5] == PN532TOHOST){
						if(rxData[i+6] == GETFIRMWAREVERSION + 0x01){
							packetCheck = i;
						}
					}
				}
			}
		}
	}
	if(packetCheck == 0xFF){		//the packet check didn't work out
		free(readData);
		free(rxData);
		return ERROR;
	}
	firmwareVersion = rxData[packetCheck + 6];
	firmwareVersion = firmwareVersion << 8;
	firmwareVersion = rxData[packetCheck + 7];
	firmwareVersion = firmwareVersion << 8;
	firmwareVersion = rxData[packetCheck + 8];
	firmwareVersion = firmwareVersion << 8;
	firmwareVersion = rxData[packetCheck + 9];
	firmwareVersion = firmwareVersion << 8;
	
	free(readData);
	free(rxData);
	return firmwareVersion;
}


//page 89
uint8_t ConfigureSAM(void){
	uint8_t configureSAM[4] = {0};
	uint8_t configureSAMResponseLength = 10;
	uint8_t readData[configureSAMResponseLength] = {0};
	uint8_t rxData[configureSAMResponseLength] = {0};
	uint8_t i, packetCheck = 0xFF;
	
	configureSAM[0] = SAMCONFIGURATION;
	configureSAM[1] = 0x01;		//normal mode, SAM not used, default
	configureSAM[2] = 20;		//timeout of 1 second; 20*50ms = 1s
	configureSAM[3] = 0x01;		//default is 0x01, uses interrupt pin; might have to have interrupt pin in future
	
	readData[configureSAMResponseLength-1] = SPI_DATAREAD;		//sets the last element of array to read data
	
	if(!SendCommandAndACK(configureSAM, 4)){
		free(readData);
		free(rxData);
		return ERROR;
	}
	ReadSPI(readData, rxData, configureSAMResponseLength);
	for(i = 0; i < 6; i++){		//search through first 6 to see if packet is correct
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
	
	
	free(readData);
	free(rxData);
	if(packetCheck == 0xFF){		//the packet check didn't work out
		return ERROR;
	}
	if(rxData[packetCheck+5] == 0x15){		//no errors if returns 0x15
		return SUCCESS;
	}else{
		return ERROR;
	}
}

//page 116
uint32_t DetectPassiveTarget(uint8_t cardType){
	
	uint8_t detectPassiveTarget[3] = {0};
	uint8_t detectPassiveTargetLength = 22;
	// uint8_t *readData = (uint8_t*) calloc(detectPassiveTargetLength, sizeof(uint8_t));
	// uint8_t *rxData = (uint8_t*) calloc(detectPassiveTargetLength, sizeof(uint8_t));
	uint8_t readData[detectPassiveTargetLength] = {0};
	uint8_t rxData[detectPassiveTargetLength] = {0};
	uint8_t i, packetCheck = 0xFF, targetCount = 1;	
	
	//return data
	uint32_t cardData = 0;
	uint16_t sensRes = 0;		//not sure what this is
	uint8_t selRes = 0;			//not sure what this is either
	
	
	detectPassiveTarget[0] = INLISTPASSIVETARGET;
	detectPassiveTarget[1] = targetCount;						//how many cards to read, max is 2 but we shouldn't ever read more than 1
	detectPassiveTarget[2] = cardType;							//depends on the type of card being read, defines baud rate as well
	readData[detectPassiveTargetLength-1] = SPI_DATAREAD;		//sets the last element of array to read data
	
	if(!SendCommandAndACK(detectPassiveTarget, 3)){
		// free(readData);
		// free(rxData);
		return ERROR;
	}
	ReadSPI(readData, rxData, detectPassiveTargetLength);
	for(i = 0; i < 6; i++){		//search through first 6 to see if packet is correct
		if(rxData[i] == PREAMBLE){
			if(rxData[i+1] == STARTCODE1){
				if(rxData[i+2] == STARTCODE2){
					if(rxData[i+5] == PN532TOHOST){
						if(rxData[i+6] == INLISTPASSIVETARGET + 0x01){
							//this is extra one for only this function
							if(rxData[i+7] == targetCount){
								packetCheck = i;
							}
						}
					}
				}	
			}
		}
	}
	if(packetCheck == 0xFF){		//the packet check didn't work out
		// free(readData);
		// free(rxData);
		return ERROR;
	}
	sensRes = rxData[packetCheck+9];
	sensRes = sensRes << 8;
	sensRes |= rxData[packetCheck+10];
	selRes = rxData[packetCheck+11];
	
	for(i = 0; i < rxData[packetCheck+12]; i++){	//rxData[12] is the ID length; should be at most 4 bytes
		cardData = cardData << 8;					//first time this happens shouldn't do anything because already all 0
		cardData = rxData[packetCheck+13+i];
	}
	
	return cardData;
}







