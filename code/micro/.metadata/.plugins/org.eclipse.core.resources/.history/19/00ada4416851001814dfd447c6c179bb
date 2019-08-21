#include "cache.h"

Cache *CacheInit(char *filename){
    int i, j;
    Cache *cache = malloc(sizeof(cache));
    cache->token[0] = 0;
    cache->fileHandle = -1;
    cache->fileSize = 0;
    cache->readFileStatus = 0;
    strcpy(cache->fileName, filename);
//    UART_PRINT("filename: %s, cache->filename: %snext:", filename, cache->fileName);

//    for(i = 0; i < MAXFILENAMESIZE; i++){
//        cache->fileName[i] = filename[i];
//        UART_PRINT("\n%c", cache->fileName[i]);
//    }
//    UART_PRINT("\nfilename: %s, cache->filename: %s\n", filename, cache->fileName);
//
    for(i = 0; i < LINES; i++){                     //fill everything with string terminator, last byte is newline
        for(j = 0; j < MAXCRUZIDLENGTH; j++){
            if(j == (MAXCRUZIDLENGTH - 1)){
                cache->fileData[i][j] = '\n';
            }else{
                cache->fileData[i][j] = '\0';
            }
        }
    }
    cache->lineIndex = 0;

//    UART_PRINT("Successful CACHE INIT\n");
    return cache;
}

void fileRead(Cache *cache){
    uint8_t fileOverwriteFlag = 0;
//    cache->fileHandle = sl_FsOpen(cache->fileName, SL_FS_READ, &(cache->token));

    cache->fileHandle = sl_FsOpen("cache.txt", SL_FS_READ, cache->token);
    if(cache->fileHandle < 0){             //file opening failed, either doesn't exist or just failed for some reason
        UART_PRINT("File opening failed... Creating a new file\n");
        cache->fileHandle = sl_FsOpen("cache.txt", SL_FS_CREATE|SL_FS_OVERWRITE|SL_FS_CREATE_MAX_SIZE(MAXCACHESIZE), cache->token);
//      cache->fileHandle = sl_FsOpen("cache.txt", SL_FS_CREATE|SL_FS_OVERWRITE, NULL);

        if(cache->fileHandle < 0){
            UART_PRINT("Creating new file failed... Quitting\n");
        }
        fileOverwriteFlag = 1;
    }

    if(fileOverwriteFlag){
        UART_PRINT("Flage = 1\n");
        sl_FsClose(cache->fileHandle, NULL, NULL , 0);
    }else{
        uint8_t i;
        for(i = 0; i < LINES; i++){
//            if(sl_FsRead(cache->fileName, i*MAXCRUZIDLENGTH, cache->fileData[i], MAXCRUZIDLENGTH) < 0){
            if(sl_FsRead("cache.txt", i*MAXCRUZIDLENGTH, cache->fileData[i], MAXCRUZIDLENGTH) < 0){
                UART_PRINT("Error reading from file at line %d\n", i);
            }
        }
    }
    sl_FsClose(cache->fileHandle, NULL, NULL , 0);


//    UART_PRINT("No errors in read\n");
    return;
}



void filePrepend(Cache *cache, char *inputData){            //input data should be cruzId with max length MAXCRUZIDLENGTH
    int i;
    for(i = 0; i < LINES; i++){
        if(strcmp(inputData, cache->fileData[i]) == 0){
            UART_PRINT("User is already in cache\n");
            return;
        }
    }


    if(cache->lineIndex < LINES){       //set the index appropriately
        //find the next empty part of array and write new inputData
        strcpy(cache->fileData[cache->lineIndex], inputData);
        (cache->lineIndex)++;         //increment counter
    }else{
        cache->lineIndex = 0;
        strcpy(cache->fileData[cache->lineIndex], inputData);
        (cache->lineIndex)++;
    }

    //reopen file to add new inputData
//    cache->fileHandle = sl_FsOpen(cache->fileName, SL_FS_WRITE, &(cache->token));
    cache->fileHandle = sl_FsOpen("cache.txt", SL_FS_WRITE, &(cache->token));
    for(i = 0; i < LINES; i++){
        if(sl_FsWrite(cache->fileName, i*LINES, cache->fileData[i], MAXCRUZIDLENGTH) < 0){
            UART_PRINT("Error writing to file at line %d\n", i);
        }
    }

    sl_FsClose(cache->fileHandle, NULL, NULL , 0);
//    UART_PRINT("No errors in prepend\n");
    return;
}


void cacheClose(Cache *cache){
    free(cache);
    return;
}





//Cache *CacheInit(char* fileName){
//    Cache *cache = malloc(sizeof(cache));
//    cache->fileSize = 0;
//    cache->readFileStatus = 0;
//    cache->token = 0;
//    cache->fileHandle = -1;
//    //OPENS FILE, CREATES ONE IF IT DOESN'T EXIST
//    cache->fileHandle = sl_FsOpen(fileName, SL_FS_READ, &(cache->token));
//    if(cache->fileHandle < 0){             //file opening failed, either doesn't exist or just failed for some reason
//        UART_PRINT("File opening failed... Creating a new file\n");
////        uint8_t signature = 'A';
////        sl_FsClose(fileHandle, NULL, &signature, 1);
//        sl_FsClose(cache->fileHandle, NULL, NULL , 0);
//        cache->fileHandle = sl_FsOpen(fileName, SL_FS_CREATE|SL_FS_OVERWRITE|SL_FS_CREATE_MAX_SIZE(USERDEFINEDMAXFILESIZE), &(cache->token));
//        if(cache->fileHandle < 0){
//            UART_PRINT("Creating new file failed... Quitting\n");
//            sl_FsClose(cache->fileHandle, NULL, NULL , 0);
//        }
//    }
//    return cache;
//}
//
//
//uint32_t CacheInfo(Cache *cache, char* fileName){
////    FINDS THE FILE SIZE
//    SlFsFileInfo_t *fileInfo = malloc(sizeof(SlFsFileInfo_t));
//    cache->readFileStatus = sl_FsGetInfo(fileName, 0, fileInfo);
//    if(fileInfo->Len <= 0){
//        UART_PRINT("File is empty\n");
//    }
//    uint32_t fileLength = fileInfo->Len;
//    free(fileInfo);
//    return fileLength;
//}
//
//void CacheClose(Cache *cache){
//    if(sl_FsClose(cache->fileHandle,0,0,0) < 0){
//        UART_PRINT("Failure Closing Cache\n");
//    }else{
//        //Cache closed successfully
//    }
//    free(cache);
//}
//
//
//uint8_t *CacheRead(Cache *cache, uint32_t dataLength){
//    uint8_t *currentData = malloc(sizeof(USERDEFINEDMAXFILESIZE));
//    sl_FsRead(cache->fileHandle, 0, currentData, dataLength);           //2nd argument is offset
////    CacheClose(cache);
//
//    return currentData;
//}
//
//uint8_t CacheWrite(Cache *cache, char *fileName, char *data, uint32_t dataLength){
//    //have to copy current data because old data is deleted when writing to a file
//    uint32_t fileLength = CacheInfo(cache, fileName);
//    uint8_t *currentData = CacheRead(cache, fileLength);
//
//    sl_FsWrite(cache->fileHandle, 0, data, dataLength);                                                 //write new stuff
//    sl_FsWrite(cache->fileHandle, dataLength, currentData, USERDEFINEDMAXFILESIZE-dataLength);          //write old stuff
//    free(currentData);
//
////    CacheClose(cache);
//}




