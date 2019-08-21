#include "MFRC522.h"

void InitMFRC(SPI_Handle spi, *SPI_Transaction spiTransaction){
	WriteMFRC(COMMANDREG, SOFTRESET, spi, &spiTransaction);		//first action
	
	//timer automatically starts, non-gated
	WriteMFRC(TMODEREG, 0x8D, spi, &spiTransaction);   		//0x8D = 0b1000 1101
	
	//Part of TRELOADREGL and RELOADREGH
	WriteMFRC(TPRESCALERREG, 0x3E, spi, &spiTransaction);		//0x3E = 0b0011 1110
	//16-bit timer reload valuel just decimal 30
	WriteMFRC(TRELOADREGL, 30, spi, &spiTransaction);
	WriteMFRC(TRELOADREGH, 0, spi, &spiTransaction);
	//transmission modulation setting; 100% ASK
	WriteMFRC(TXAUTOREG, 0x40, spi, &spiTransaction);			//0x40 = 0b0100 0000
	//transmitter starts only in RF field, MFIN is active HIGH, CRC=0x6363
	WriteMFRC(MODEREG, 0x3D, spi, &spiTransaction);			//0x3D = 0b0011 1101
	
	AntennaOn(PI_Handle spi, *SPI_Transaction spiTransaction);
}

void WriteMFRC(uint8_t address, char value, SPI_Handle spi, *SPI_Transaction spiTransaction){
	uint8_t tx[2] = {0};			//transfer buffer initialized to all zero
	// 0x7E is binary 0b0111 1110
	// first bit defines read or write
	// last bit is always logic 0
	tx[0] = (address << 1) & 0x7E;
	tx[1] = value;
	
	spiTransaction->count = sizeof(tx);		//should be 2
	spiTransaction->txBuf = tx;
	spiTransaction->rxBuf = NULL;
	
	transferOK = SPI_transfer(spi, &spiTransaction);
	if (!transferOK) {
		GPIO_write(Board_GPIO_LED0, Board_GPIO_LED_OFF);
		// Error in SPI or transfer already in progress.
	}
}

uint8_t ReadMFRC(uint8_t address, SPI_Handle spi, *SPI_Transaction spiTransaction){
	uint8_t tx[2] = {0};
	uint8_t rx[2] = {0};
	
	//0x80 is binary 0b1000 1101
	tx[0] = (address<<1) | 0x80;
	
	spiTransaction->count = sizeof(tx);		//should be 2
	spiTransaction->txBuf = tx;
	spiTransaction->rxBuf = rx;
	
	transferOK = SPI_transfer(spi, &spiTransaction);
	if (!transferOK) {
		GPIO_write(Board_GPIO_LED0, Board_GPIO_LED_OFF);
		// Error in SPI or transfer already in progress.
	}
	
	return rx[1];
}


void AntennaOn(SPI_Handle spi, *SPI_Transaction spiTransaction){
	uint8_t antennaState = ReadMFRC(TXCONTROLREG);
	//sets the bit so antenna is on
	WriteMFRC(TXCONTROLREG, SetBitMFRC(antennaState,0x03, spi, &spiTransaction), spi, &spiTransaction);
}

void AntennaOff(SPI_Handle spi, *SPI_Transaction spiTransaction){
	uint8_t antennaState = ReadMFRC(TXCONTROLREG);
	//clears bit so antenna is off
	WriteMFRC(TXCONTROLREG, ClearBitMFRC(antennaState,0x03, spi, &spiTransaction), spi, &spiTransaction);
}


void CloseMFRC(SPI_Handle spi, *SPI_Transaction spiTransaction){
	WriteMFRC(COMMANDREG, IDLE, spi, &spiTransaction);
}

void SetBitMFRC(uint8_t address, uint8_t mask, SPI_Handle spi, *SPI_Transaction spiTransaction){
	uint8_t read = ReadMFRC(address, spi, &spiTransaction);
	read = SETBIT(read, mask);
	WriteMFRC(address, read, spi, &spiTransaction);
}

void ClearBitMFRC(uint8_t address, uint8_t mask, SPI_Handle spi, *SPI_Transaction spiTransaction){
	uint8_t read = ReadMFRC(address, spi, &spiTransaction);
	read = CLEARBIT(read, mask);
	WriteMFRC(address, read, spi, &spiTransaction);
}

uint8_t MFRCToCard(uint8_t command, uint8_t *cardData, uint8_t cardLength, uint8_t *returnData, uint8_t *returnLength, SPI_Handle spi, *SPI_Transaction spiTransaction){
	uint8_t irqEn, irqWait, status = RETURNFAIL;
	
	//see what command is wanted from #define
	switch(command){
		case TRANSCEIVE:
			irqWait = 0x08|0x10;	//binary 0b0011 0000
			irqEn = 0xF3;			//binary 0b1111 0011
			break;
		case MFAUTHENT:
			irqWait = 0x08;			//binary 0b0001 0000
			irqEn = 0xF3;			//binary 0b1111 0011
			break;
		default:
			//wrong
			break;
	}
	
	WriteMFRC(COMMIENREG, irqEn|0x80, spi, &spiTransaction);
	ClearBitMFRC(COMMIRQREG, 0x80, spi, &spiTransaction);
	SetBitMFRC(FIFOLEVELREG, 0x80, spi, &spiTransaction);
	WriteMFRC(COMMANDREG, IDLE, spi, &spiTransaction);
	
	//writes the data to be sent
	uint8_t i;
	for(i = 0; i < cardLength; i++){
		WriteMFRC(FIFODATAREG, cardData[i], spi, &spiTransaction);
	}
	WriteMFRC(COMMANDREG, command, spi, &spiTransaction);
	
	/**************************************************/
	//start transmission
	SetBitMFRC(BITFRAMINGREG, 0x80, spi, &spiTransaction);
	//wait 25ms
	for(i = 2000; i > 0; i--){
		uint8_t read = ReadMFRC(COMIRQREG, spi, &spiTransaction);
		//conditional statements to leave for loop early
		//first is if time out, second is if the receiver gets the data
		if( read&0x01 || read&irqWait ){
			break;
		}
	}
	//stop transmission
	ClearBitMFRC(BITFRAMINGREG, 0x80, spi, &spiTransaction);
	/**************************************************/
	if(i > 0){
		if(READBIT(read, 0x01)){
			return RETURNTIMEOUT;
		}else if(!READBIT(readMFRC(ErrorReg), 0x1B, spi, &spiTransaction)){	//binary 0b0001 1011, page41
			return RETURNFAIL;
		}else{
			uint8_t lengthFIFO = readMFRC(FIFOLEVELREG, spi, &spiTransaction);
			if(lengthFIFO == 0){
				lengthFIFO = 1;
			}
			if(lengthFIFO > MAXLENGTH){
				lengthFIFO = MAXLENGTH;
			}
			
			uint8_t lastValidBits = readMFRC(CONTROLREG, spi, &spiTransaction) & 0x07;	//binary 0b0000 0111, pg 45
			if(lastValidBits){
				*numberBits = 8*(lengthFIFO-1)+lastValidBits;
			}else{
				*numberBits = 8*(lengthFIFO);
			}
			
			//get back data from FIFO
			for(i = 0; i < lengthFIFO; i++){
				returnData[i] = readMFRC(FIFODATAREG, spi, &spiTransaction);
			}
			*returnLength = lengthFIFO;
			status = RETURNOK;
		}
	}
	return status;
}


uint8_t CardFind(uint8_t *tagType, uint8_t *returnData, uint8_t *returnLength, SPI_Handle spi, *SPI_Transaction spiTransaction){
	uint8_t status, cardLength = 1;
	
	tagType[0] = 0x52;		//0b0101 0010, bitwise OR of all possible types of cards miFare
	WriteMFRC(BITFRAMINGREG, 0x07);		//0b0000 0111
	
	status = MFRCToCard(TRANSCEIVE, tagType, cardLength, returnData, returnLength, spi, &spiTransaction);
	if(returnLength != 16){
		return RETURNFAIL
	}else if(status != RETURNOK){
		return status;
	}else{
		return RETURNOK;
	}
}

//returns 4 byte of card serial number, 5th byte is check
uint8_t CardSerialNumber(uint8_t *serialNumber, SPI_Handle spi, *SPI_Transaction spiTransaction){
	uint8_t returnLength, status, cardLength = 2, serialNumberCheck = 0;
	uint8_t i;
	
	WriteMFRC(BITFRAMINGREG, 0x00);			//page 46,  all bits of last byte will be transmitted [2:0]
	status = MFRCToCard(TRANSCEIVE, serialNumber, cardLength, serialNumber, &returnLength, spi, spiTransaction);
	
	//status == 1
	if(status == RETURNOK){
		if(returnLength == 5){
			for(i = 0; i < 4; i++){
				*(serialNumber+i) = serialNumber[i];
				serialNumberCheck ^= serialNumber[i];
			}
			if(serialNumberCheck != serialNumber[i]){
				status = RETURNFAIL;
			}
		}else{
			status = RETURNFAIL;
		}
	}else{
		status = RETURNFAIL;
	}
	return status;
}











