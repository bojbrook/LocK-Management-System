#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <ti/drivers/net/wifi/simplelink.h>

#include <uart_term.h>


#define MAXCRUZIDLENGTH         (15+1)      //plus one for new line
#define LINES    (16)
#define MAXCACHESIZE            LINES*MAXCRUZIDLENGTH
#define MAXFILENAMESIZE         20

typedef struct Cache{
    _u32 token[1];                                                     //token for reading
    int fileHandle;
    uint32_t fileSize;
    uint8_t readFileStatus;
    uint8_t fileName[MAXFILENAMESIZE];                                 //name of the file
    uint8_t fileData[LINES][MAXCRUZIDLENGTH];                       //stores every byte of file data
    uint8_t lineIndex;                                            //number of cruzids in the file
}Cache;

Cache *CacheInit(char *filename);
void fileRead(Cache *cache);
void filePrepend(Cache *cache, char *inputData);
void cacheClose(Cache *cache);


//Cache *CacheInit(char *fileName);
//uint32_t CacheInfo(Cache *cache, char *fileName);
//void CacheClose(Cache *cache);
//uint8_t *CacheRead(Cache *cache, uint32_t dataLength);
//uint8_t CacheWrite(Cache *cache, char *fileName, char *data, uint32_t dataLength);
