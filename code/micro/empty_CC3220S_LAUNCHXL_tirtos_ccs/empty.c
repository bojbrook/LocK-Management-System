/* Standard libraries */
#include <unistd.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>

/* TI-DRIVERS Header files */
#include <ti/drivers/GPIO.h>
#include <ti/drivers/SPI.h>
#include <ti/drivers/net/wifi/simplelink.h>
#include <ti/drivers/Power.h>
#include <ti/drivers/power/PowerCC32XX.h>
#include <ti/devices/cc32xx/driverlib/prcm.h>
#include <ti/devices/cc32xx/driverlib/utils.h>
#include "Board.h"

/* Our Files */
#include "PN532.h"
#include "empty.h"

/* Control block definition */
pthread_t  gSpawnThread = (pthread_t)NULL;
appControlBlock     app_CB;

void *mainThread(void *arg0)
{
    /* VARIABLES */
    ///////////////////////////////////////////////////////////////////////////////////
    int i;
    //PN532 Sensor Variables
    uint32_t retVal, firmwareVersion, returnPassiveTarget;
    uint8_t returnConfigureSAM = 0, ACKreturn = 0, test[1] = {GETFIRMWAREVERSION}, startIndex, timer;
    uint8_t zeroArray[25];                          //dummy buffer of zeros
    uint8_t rxData[25];                             //receive data array
    char cruzIDInputToCloud[25];                    //character buffer to send over TCP
    uint8_t NFCStates = DETECTPASSIVECARD;          //state machine byte


    //WiFi variables
    int32_t Sd;
    SlSockAddr_t *Sa;
    sockAddr_t  sAddr;
    struct SlTimeval_t TimeVal;
    TimeVal.tv_sec = 3;                             // Seconds
    TimeVal.tv_usec = 0;                            // Microseconds. 10000 microseconds resolution
    sAddr.in4.sin_family = SL_AF_INET;
    sAddr.in4.sin_port = sl_Htons((unsigned short)6001);
    sAddr.in4.sin_addr.s_addr = sl_Htonl(SL_IPV4_VAL(35,230,38,111));
    Sa = (SlSockAddr_t*)&sAddr.in4;

    /* INIT FUNCTIONS */
    ///////////////////////////////////////////////////////////////////////////////////
    Board_initGeneral();            //already has power_init();
    GPIO_init();
    SPI_init();
    InitSPI();
    InitTerm();                         //configures UART terminal
    retVal = initAppVariables();

    /* Configure the LED and button pins */
    ////////////////////////////////////////////////////////////////////////////////////
    GPIO_setConfig(MOTORA, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_LOW);                    //motor drive 1
    GPIO_setConfig(MOTORB, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_LOW);                    //motor drive 2
    GPIO_setConfig(SLAVESELECT, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_LOW);               //slave select
    GPIO_setConfig(GREENLIGHT, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_LOW);                //green light
    GPIO_setConfig(REDLIGHT, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_LOW);                  //red lights
    GPIO_write(MOTORA, 0);
    GPIO_write(MOTORB, 0);
    GPIO_write(SLAVESELECT, GPIOLEDOFF);            //1 is off for SS, 0 is on
    GPIO_write(GREENLIGHT, GPIOLEDOFF);
    GPIO_write(REDLIGHT, GPIOLEDOFF);

    //Starts WiFi and waits until it is connected before continuing
    ////////////////////////////////////////////////////////////////////////////////////
    #ifdef DEBUG_PRINT
    UART_PRINT("Starting Wifi\n");
    #endif
    StartWiFi();
    while(app_CB.Status != STATUS_BIT_IP_LEASED){
        usleep(500000);
    }

    //PN532 Sensor initialization
    ////////////////////////////////////////////////////////////////////////////////////
    ACKreturn = SendCommandAndACK(test, sizeof(test));
    returnConfigureSAM = ConfigureSAM();
    firmwareVersion = GetFirmwareVersion();
    if( (ACKreturn != SUCCESS)
            && (returnConfigureSAM != SUCCESS)
            && (firmwareVersion != PN532FIRMWAREVERSION) ){
        #ifdef DEBUG_PRINT
        UART_PRINT("Sensor initialization error\n");
        #endif
    }

    #ifdef DEBUG_PRINT
    UART_PRINT("Entering Infinite Loop\n");
    #endif
    while(1){
        switch(NFCStates){
        case DETECTPASSIVECARD:
            //returns the card ID which is a 8 bytes usually like 0x8888888
            returnPassiveTarget = DetectPassiveTarget(ISOIEC14443A);
            if(returnPassiveTarget > 1000){
                NFCStates = DETECTPHONE;
            }
            break;
        case DETECTPHONE:
            if(InDataExchange() == SUCCESS){
                NFCStates = CORRECTFRAME;
            }else{
                NFCStates = DETECTPASSIVECARD;
            }
            break;
        case CORRECTFRAME:
            timer = 1;
            while(timer++ < 10){              //loop only lasts usleep()*10 time
                zeroArray[0] = SPI_DATAREAD;
                ReadSPI(zeroArray, rxData, 25);
                startIndex = frameCheck(rxData, INDATAEXCHANGE + 0x01);
                if(startIndex != ERROR){        //if not an error, break from loop
                    break;
                }
                usleep(50000);
            }
            if(startIndex == ERROR){            //sensor is not ready for data
                #ifdef DEBUG_PRINT
                UART_PRINT("Framing Error\n");
                #endif
            }else{
                for(i = 0; i < 10; i++){
                    if(rxData[i+DATAEXCHANGEINDEX+startIndex] != '!'){          //cruzID is terminated by '!'
                        #ifdef DEBUG_PRINT
                            UART_PRINT("%c", rxData[i+DATAEXCHANGEINDEX+startIndex]);
                        #endif
                        cruzIDInputToCloud[i] = rxData[i+DATAEXCHANGEINDEX+startIndex];
                    }else if(rxData[i+DATAEXCHANGEINDEX+startIndex] == '!') {
                        #ifdef DEBUG_PRINT
                        UART_PRINT("\n");
                        #endif
                        cruzIDInputToCloud[i] = '\0';
                        break;
                    }
                }
                NFCStates = INTERNET;
                break;
            }
            NFCStates = DETECTPASSIVECARD;
            break;
        case INTERNET:
            Sd = sl_Socket(Sa->sa_family, SL_SOCK_STREAM, TCP_PROTOCOL_FLAGS);
            ASSERT_ON_ERROR(Sd, SL_SOCKET_ERROR);

            //Enable receive timeout
            retVal = sl_SetSockOpt(Sd,SL_SOL_SOCKET,SL_SO_RCVTIMEO, (_u8 *)&TimeVal, sizeof(TimeVal));
            if( retVal ){
                #ifdef DEBUG_PRINT
                UART_PRINT("ERROR: Socket timeout\n");
                #endif
            }

            //function to check for if user is allowed, calls Google Cloud
            retVal = IsAllowed(Sd,Sa,cruzIDInputToCloud);
            if(retVal == 1){
                #ifdef DEBUG_PRINT
                UART_PRINT("They are allowed in %s\n", ROOM_NUMBER);
                #endif
                GPIO_write(GREENLIGHT, GPIOLEDON);
                MotorControl(MOTORCCW);             //unlock door
                sleep(1);
                MotorControl(MOTORCW);              //lock door
            }else if(retVal == 0){
                #ifdef DEBUG_PRINT
                UART_PRINT("They are not allowed in %s\n", ROOM_NUMBER);
                #endif
                GPIO_write(REDLIGHT, GPIOLEDON);
            }else{
                #ifdef DEBUG_PRINT
                UART_PRINT("SOMETHING WENT REALLY WRONG\n");
                #endif
                GPIO_write(REDLIGHT, GPIOLEDON);
            }
            NFCStates = DETECTPASSIVECARD;
            break;
        }

        usleep(100000);
        GPIO_write(REDLIGHT, GPIOLEDOFF);
        GPIO_write(GREENLIGHT, GPIOLEDOFF);
        if(NFCStates == DETECTPASSIVECARD){
            Power_shutdown(0, 3000);    //hibernate
        }
    }
}

void MotorControl(uint8_t direction){
    if(direction == MOTORCCW){
        GPIO_write(MOTORA, 1);
        GPIO_write(MOTORB, 0);
    }else if(direction == MOTORCW){
        GPIO_write(MOTORA, 0);
        GPIO_write(MOTORB, 1);
    }else{
        #ifdef DEBUG_PRINT
        UART_PRINT("Motor Direction Undefined\n");
        #endif
        return;
    }
    //turn and then stop
    sleep(1);
    GPIO_write(MOTORA, 0);
    GPIO_write(MOTORB, 0);
    return;
}

void StartWiFi(void){
    /*VARIABLES*/
    pthread_attr_t  pAttrs_spawn;
    struct sched_param  priParam;
    int32_t RetVal;


    /*PTHREAD*/
    pthread_attr_init(&pAttrs_spawn);
    priParam.sched_priority = SPAWN_TASK_PRIORITY;
    RetVal = pthread_attr_setschedparam(&pAttrs_spawn, &priParam);
    RetVal |= pthread_attr_setstacksize(&pAttrs_spawn, TASK_STACK_SIZE);
    RetVal = pthread_create(&gSpawnThread, &pAttrs_spawn, sl_Task, NULL);           //sl_task is first SL function that needs to be called
    if(RetVal){
        /* Handle Error */
        #ifdef DEBUG_PRINT
        UART_PRINT("Unable to create sl_Task thread \n");
        #endif
        while(1);
    }


    RetVal = ConfigureSimpleLinkToDefaultState();               //calls sl_Start()
    if(RetVal < 0)
    {
        /* Handle Error */
        #ifdef DEBUG_PRINT
        UART_PRINT("EMPTY project - Couldn't configure Network Processor\n");
        #endif
    }
}



/*\brief WLAN Async event handler*/
void SimpleLinkWlanEventHandler(SlWlanEvent_t *slWlanEvent){
    if(!slWlanEvent){
        return;
    }
    switch(slWlanEvent->Id){
        case SL_WLAN_EVENT_CONNECT:{
            SET_STATUS_BIT(app_CB.Status, STATUS_BIT_CONNECTION);
            /* Copy new connection SSID and BSSID to global parameters */
            memcpy(app_CB.CON_CB.ConnectionSSID, slWlanEvent->Data.Connect.SsidName, slWlanEvent->Data.Connect.SsidLen);
            memcpy(app_CB.CON_CB.ConnectionBSSID, slWlanEvent->Data.Connect.Bssid, SL_WLAN_BSSID_LENGTH);

            #ifdef DEBUG_PRINT
            UART_PRINT("\n\r[WLAN EVENT] STA Connected to the AP: %s , "
                "BSSID: %x:%x:%x:%x:%x:%x\n\r",
                      app_CB.CON_CB.ConnectionSSID, app_CB.CON_CB.ConnectionBSSID[0],
                      app_CB.CON_CB.ConnectionBSSID[1],app_CB.CON_CB.ConnectionBSSID[2],
                      app_CB.CON_CB.ConnectionBSSID[3],app_CB.CON_CB.ConnectionBSSID[4],
                      app_CB.CON_CB.ConnectionBSSID[5]);
            #endif
            sem_post(&app_CB.CON_CB.connectEventSyncObj);
        }
        break;
        case SL_WLAN_EVENT_DISCONNECT:
               {
                   //SlWlanEventDisconnect_t  *pEventData = NULL;

                   CLR_STATUS_BIT(app_CB.Status, STATUS_BIT_CONNECTION);
                   CLR_STATUS_BIT(app_CB.Status, STATUS_BIT_IP_ACQUIRED);
                   CLR_STATUS_BIT(app_CB.Status, STATUS_BIT_IPV6_ACQUIRED);

                    //pEventData = &slWlanEvent->Data.Disconnect;
                    #ifdef DEBUG_PRINT
                   UART_PRINT("\n\r[WLAN ERROR] Device disconnected from the AP: %s,\n\r"
                   "BSSID: %x:%x:%x:%x:%x:%x\n\r",
                     app_CB.CON_CB.ConnectionSSID, app_CB.CON_CB.ConnectionBSSID[0],
                     app_CB.CON_CB.ConnectionBSSID[1],app_CB.CON_CB.ConnectionBSSID[2],
                     app_CB.CON_CB.ConnectionBSSID[3],app_CB.CON_CB.ConnectionBSSID[4],
                     app_CB.CON_CB.ConnectionBSSID[5]);
                   #endif

                   memset(&(app_CB.CON_CB.ConnectionSSID), 0x0, sizeof(app_CB.CON_CB.ConnectionSSID));
                   memset(&(app_CB.CON_CB.ConnectionBSSID), 0x0, sizeof(app_CB.CON_CB.ConnectionBSSID));
               }
               break;
        default:{
                #ifdef DEBUG_PRINT
                UART_PRINT("\n\r[WLAN EVENT] Unexpected event [0x%x]\n\r", slWlanEvent->Id);
                #endif
            }
            break;
    }
}



int32_t ConfigureSimpleLinkToDefaultState(){
    int32_t  RetVal = -1;
    int32_t  Mode = -1;
    /* Turn NWP on */
    Mode = sl_Start(0, 0, 0);
    ASSERT_ON_ERROR(Mode, DEVICE_ERROR);

    if(Mode != ROLE_STA){
        /* Set NWP role as STA */
        Mode = sl_WlanSetMode(ROLE_STA);
        ASSERT_ON_ERROR(Mode, WLAN_ERROR);

        /* For changes to take affect, we restart the NWP */
        RetVal = sl_Stop(SL_STOP_TIMEOUT);
        ASSERT_ON_ERROR(RetVal, DEVICE_ERROR);

        Mode = sl_Start(0, 0, 0);
        ASSERT_ON_ERROR(Mode, DEVICE_ERROR);
    }

    if(Mode != ROLE_STA){
        #ifdef DEBUG_PRINT
        UART_PRINT("Failed to configure device to it's default state");
        #endif
        return -1;
    }else{
         SlWlanSecParams_t SecParams;
         SecParams.Type = SL_WLAN_SEC_TYPE_WPA_WPA2;
         SecParams.Key = WIFI_KEY;
         SecParams.KeyLen = strlen((const char*)SecParams.Key);
         sl_WlanConnect(WIFI_SSID,strlen(WIFI_SSID),0 ,&SecParams ,0);
         return 0;
    }

}

int32_t initAppVariables(void){
    int32_t ret = 0;
    app_CB.Status = 0 ;
    app_CB.Role = ROLE_RESERVED;
    app_CB.Exit = FALSE;


    memset(&app_CB.CmdBuffer, 0x0, CMD_BUFFER_LEN);
    memset(&app_CB.CON_CB, 0x0, sizeof(app_CB.CON_CB));

    ret = sem_init(&app_CB.CON_CB.connectEventSyncObj,    0, 0);
    if(ret != 0){
        SHOW_WARNING(ret, OS_ERROR);
        return -1;
    }

    ret = sem_init(&app_CB.CON_CB.eventCompletedSyncObj,  0, 0);
    if(ret != 0){
        SHOW_WARNING(ret, OS_ERROR);
        return -1;
    }

    ret = sem_init(&app_CB.CON_CB.ip4acquireEventSyncObj, 0, 0);
    if(ret != 0){
        SHOW_WARNING(ret, OS_ERROR);
        return -1;
    }

    ret = sem_init(&app_CB.CON_CB.ip6acquireEventSyncObj, 0, 0);
    if(ret != 0){
        SHOW_WARNING(ret, OS_ERROR);
        return -1;
    }

    ret = sem_init(&app_CB.WowlanSleepSem, 0, 0);
    if(ret != 0){
        SHOW_WARNING(ret, OS_ERROR);
        return -1;
    }
    return ret;
}


int32_t IsAllowed(int Sd, SlSockAddr_t * Sa , char * cruzID){
    int32_t Status = -1;
    int32_t addrSize = sizeof(SlSockAddrIn6_t);
    int32_t returnValue = -1;
    char sending_buffer[25];
    char RecvBuf[50];
    _i16 nb = 1; //nonblocking call
    int attempts = 0; //counter for connect

    sprintf(sending_buffer,"\\login-%s%s",cruzID,ROOM_NUMBER);
    while(Status < 0){
       /* Calling 'sl_Connect' followed by server's
        * 'sl_Accept' would start session with
        * the TCP server. */
        if(attempts > MAXATTEMPS){
           sl_Close(Sd);
           return -1;
        }
       attempts++;
       Status = sl_Connect(Sd, Sa, addrSize);
       if((Status == SL_ERROR_BSD_EALREADY)&& (nb)){
            #ifdef DEBUG_PRINT
           UART_PRINT("Status == SLERROR, Status = %d\n", Status);
           #endif
           sleep(1);
           continue;
       }
       break;
    }
    attempts = 0;
    Status = sl_Send(Sd, sending_buffer, strlen(sending_buffer), 0);

    while(Status < 0){
        #ifdef DEBUG_PRINT
        UART_PRINT("RESENDING...\n");
        #endif
        Status = sl_Send(Sd, sending_buffer, strlen(sending_buffer), 0);
        if(attempts > MAXATTEMPS){
            sl_Close(Sd);
            return -1;
        }
        attempts++;
    }

    Status = sl_Recv(Sd, RecvBuf, 20, 0);
    if(Status < 0){
        #ifdef DEBUG_PRINT
        UART_PRINT("[line:%d, error:%d] %s\n\r", __LINE__, Status, BSD_SOCKET_ERROR);
        #endif
        sl_Close(Sd);
        return -1;
    }else{
        sl_Close(Sd);
        RecvBuf[Status] = 0;
        if(strcmp(RecvBuf,ALLOWED) == 0){
            #ifdef DEBUG_PRINT
            UART_PRINT("Return 1\n");
            #endif
            returnValue = 1;
        }else if (strcmp(RecvBuf,DENIED) == 0){
            #ifdef DEBUG_PRINT
            UART_PRINT("Return 0\n");
            #endif
            returnValue = 0;
        }
        else{
            #ifdef DEBUG_PRINT
            UART_PRINT("Return -1\n");
            #endif
        }
    }
    return returnValue;
}

/*
 *  ======== asynchronous events for the SimpleLink library ========
 */

/*brief      Fatal Error async event for inspecting fatal error events.
                This event handles events/errors reported from the device/host driver*/
void SimpleLinkFatalErrorEventHandler(SlDeviceFatal_t *slFatalErrorEvent){

}
/*\brief      General async event for inspecting general events.
                This event handles events/errors reported from the device/host driver*/
void SimpleLinkGeneralEventHandler(SlDeviceEvent_t *slDeviceEvent){

}

/*\brief Socket Async event handler*/
void SimpleLinkSockEventHandler(SlSockEvent_t *slSockEvent){

}

/* \brief HTTP server async event*/
void SimpleLinkHttpServerEventHandler(SlNetAppHttpServerEvent_t *slHttpServerEvent,SlNetAppHttpServerResponse_t *slHttpServerResponse){

}
/* \brief NETAPP Async event handler*/
void SimpleLinkNetAppEventHandler(SlNetAppEvent_t *slNetAppEvent){

}

/*\brief          A handler for handling Netapp requests.
                    Netapp request types:
                    For HTTP server: GET / POST (future: PUT / DELETE)*/
void SimpleLinkNetAppRequestEventHandler(SlNetAppRequest_t *slNetAppRequest,SlNetAppResponse_t *slNetAppResponse){

}

/*\brief          A handler for freeing the memory of the NetApp response.*/
void SimpleLinkNetAppRequestMemFreeEventHandler(unsigned char *memFree){

}
