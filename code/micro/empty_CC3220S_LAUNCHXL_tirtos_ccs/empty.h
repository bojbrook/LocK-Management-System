/*
 * empty.h
 *
 *  Created on: Feb 28, 2018
 *      Author: bowenbrooks
 */

#ifndef EMPTY_H_
#define EMPTY_H_

#include <ti/drivers/net/wifi/simplelink.h>

#ifndef WIN32
#include "semaphore.h"
#include "pthread.h"
#include "uart_term.h"
#include "unistd.h"
#include "time.h"
#else

#include <stdio.h>
#include <stdint.h>
#include <windows.h>
#include "sl_studioP.h"

#endif


#define CMD_BUFFER_LEN          (256)
#define MAX_CMD_NAME_LEN        (32)
#define APPLICATION_NAME        ("Lock-Management-System")
#define APPLICATION_VERSION     ("1.0.1.0")
#define TASK_STACK_SIZE         (2048)
#define SPAWN_TASK_PRIORITY     (9)
#define PASSWD_LEN_MAX          (63)
#define PASSWD_LEN_MIN          (8)
#define WLAN_SCAN_COUNT         (20)
#define MAX_FILE_NAME_LEN       (32)
#define MAX_TEXT_PAD_SIZE       (256)
#define MAX_FILE_LIST           (20)
#define MAX_BUF_SIZE            (1400)
#define SL_STOP_TIMEOUT         (200)           //200ms to finish transmitting
#define DEV_TYPE_LEN            (17)
#define IPV6_ADDR_LEN           (16)
#define IPV4_ADDR_LEN           (4)
#define DEVICE_ERROR            ("Device error, please refer \"DEVICE ERRORS CODES\" section in errors.h")
#define WLAN_ERROR              ("WLAN error, please refer \"WLAN ERRORS CODES\" section in errors.h")
#define BSD_SOCKET_ERROR        ("BSD Socket error, please refer \"BSD SOCKET ERRORS CODES\" section in errors.h")
#define SL_SOCKET_ERROR         ("Socket error, please refer \"SOCKET ERRORS CODES\" section in errors.h")
#define NETAPP_ERROR            ("Netapp error, please refer \"NETAPP ERRORS CODES\" section in errors.h")
#define OS_ERROR                ("OS error, please refer \"NETAPP ERRORS CODES\" section in errno.h")
#define CMD_ERROR               ("Invalid option/command.")
#define RECEIVE_TIMEOUT         (60)
#define TCP_PROTOCOL_FLAGS      0
#define ALLOWED                 ("230 Allowed")
#define DENIED                  ("430 Denied")
///*******HOTSPOT******************/
#define WIFI_SSID               ("iPhone")
#define WIFI_KEY                ("cxfb6fqnsv9bj")
/*******SAM"S HOUSE**************/
//#define WIFI_SSID               ("DarkMystery")
//#define WIFI_KEY                ("murderpuppet")
/*************BOWEN'S HOUSE****************/
//#define WIFI_SSID               ("TpLink")
//#define WIFI_KEY                ("suckmydick8")


#define ROOM_NUMBER             ("-BE340")              //must have space  before room number
#define RESET_CARD              0xA053EFA2

#define SHOW_WARNING(ret, errortype)        UART_PRINT("\n\r[line:%d, error code:%d] %s\n\r", __LINE__, ret, errortype);

/* LED LIGHTS and GPIO*/
#define GPIOLEDOFF          1
#define GPIOLEDON           0
#define GREENLIGHT          CC3220S_LAUNCHXL_PIN_18
#define REDLIGHT            CC3220S_LAUNCHXL_PIN_53
#define SLAVESELECT         CC3220S_LAUNCHXL_PIN_15
#define MOTORA              CC3220S_LAUNCHXL_PIN_02
#define MOTORB              CC3220S_LAUNCHXL_PIN_01
#define MOTORCCW            ((uint8_t) 0)
#define MOTORCW             ((uint8_t) 1)

#define MAXATTEMPS      2


#define READBIT(A, B)       (((A) >> (B)) & 0b01)


// Used for debugging and print statements
/**************************************************/
#define DEBUG_PRINT         1
/**************************************************/




enum NFCStates{
    DETECTPASSIVECARD = 0,
    DETECTPHONE,
    CORRECTFRAME,
    INTERNET
};

typedef union{
    SlSockAddrIn6_t     in6;   /* Socket info for Ipv6 */
    SlSockAddrIn_t      in4;   /* Socket info for Ipv4 */
}sockAddr_t;


typedef union
{
    uint8_t                    nwData[MAX_BUF_SIZE];
    int8_t                     textPad[MAX_TEXT_PAD_SIZE];
    SlWlanNetworkEntry_t       netEntries[WLAN_SCAN_COUNT];
}gDataBuffer_t;

typedef struct connectionControlBlock_t
{
    sem_t     connectEventSyncObj;
    sem_t     ip4acquireEventSyncObj;
    sem_t     ip6acquireEventSyncObj;
    sem_t     eventCompletedSyncObj;
    uint32_t GatewayIP;
    uint8_t  ConnectionSSID[SL_WLAN_SSID_MAX_LENGTH +1];
    uint8_t  ConnectionBSSID[SL_WLAN_BSSID_LENGTH];
    uint32_t DestinationIp;
    uint32_t IpAddr;
    uint32_t StaIp;
    uint32_t Ipv6Addr[4];
}connection_CB;

typedef struct appControlBlock_t
{
    /* Status Variables */
    uint32_t        Status;             /* This bit-wise status variable shows the state of the NWP */
    uint32_t        Role;               /* This field keeps the device's role (STA, P2P or AP) */
    uint32_t        Exit;               /* This flag lets the application to exit */
    int16_t     socket;
    SlSockAddrIn_t remoteAddr;

    /* Data & Network entry Union */
    //gDataBuffer_t     gDataBuffer;

    /* STA/AP mode CB */
    connection_CB    CON_CB;

    /* Cmd Prompt buffer */
    uint8_t         CmdBuffer[CMD_BUFFER_LEN];

    /* WoWLAN semaphore */
    sem_t            WowlanSleepSem;

}appControlBlock;

extern appControlBlock    app_CB;


typedef enum{

    STATUS_BIT_NWP_INIT = 0,          /* This bit is set: Network Processor is powered up */


    STATUS_BIT_CONNECTION,            /* This bit is set: the device is connected
                                         to the AP or client is connected to device (AP) */

    STATUS_BIT_IP_LEASED,             /* This bit is set: the device has leased IP to
                                         any connected client */

    STATUS_BIT_IP_ACQUIRED,           /* This bit is set: the device has acquired an IP */


    STATUS_BIT_P2P_DEV_FOUND,         /* If this bit is set: the device (P2P mode)
                                         found any p2p-device in scan */

    STATUS_BIT_P2P_REQ_RECEIVED,      /* If this bit is set: the device (P2P mode)
                                         found any p2p-negotiation request */

    STATUS_BIT_CONNECTION_FAILED,     /* If this bit is set: the device(P2P mode)
                                         connection to client(or reverse way) is failed */

    STATUS_BIT_PING_STARTED,          /* This bit is set: device is undergoing ping operation */


    STATUS_BIT_SCAN_RUNNING,          /* This bit is set: Scan is running is background */


    STATUS_BIT_IPV6_ACQUIRED,         /* If this bit is set: the device has acquired
                                         an IPv6 address */

    STATUS_BIT_IPV6_GLOBAL_ACQUIRED,  /* If this bit is set: the device has acquired
                                         an IPv6 address */

    STATUS_BIT_IPV6_LOCAL_ACQUIRED,   /* If this bit is set: the device has acquired
                                        an IPv6 address */

    STATUS_BIT_AUTHENTICATION_FAILED, /* If this bit is set: Authentication with ENT AP failed. */


    STATUS_BIT_RESET_REQUIRED,


    STATUS_BIT_TX_STARED

}e_StatusBits;



/* Status keeping MACROS */

#define SET_STATUS_BIT(status_variable, bit) status_variable |= (1<<(bit))

#define CLR_STATUS_BIT(status_variable, bit) status_variable &= ~(1<<(bit))

#define GET_STATUS_BIT(status_variable, bit)    \
                                (0 != (status_variable & (1<<(bit))))

#define IS_NW_PROCSR_ON(status_variable)    \
                GET_STATUS_BIT(status_variable, STATUS_BIT_NWP_INIT)

#define IS_CONNECTED(status_variable)       \
                GET_STATUS_BIT(status_variable, STATUS_BIT_CONNECTION)

#define IS_IP_LEASED(status_variable)       \
                GET_STATUS_BIT(status_variable, STATUS_BIT_IP_LEASED)

#define IS_IP_ACQUIRED(status_variable)     \
                GET_STATUS_BIT(status_variable, STATUS_BIT_IP_ACQUIRED)

#define IS_IP6_ACQUIRED(status_variable)     \
        GET_STATUS_BIT(status_variable, (STATUS_BIT_IPV6_LOCAL_ACQUIRED | STATUS_BIT_IPV6_GLOBAL_ACQUIRED))

#define IS_IPV6L_ACQUIRED(status_variable)  \
                GET_STATUS_BIT(status_variable, STATUS_BIT_IPV6_LOCAL_ACQUIRED)

#define IS_IPV6G_ACQUIRED(status_variable)  \
                GET_STATUS_BIT(status_variable, STATUS_BIT_IPV6_GLOBAL_ACQUIRED)

#define IS_PING_RUNNING(status_variable)    \
                GET_STATUS_BIT(status_variable, STATUS_BIT_PING_STARTED)

#define    IS_TX_ON(status_variable)    \
            GET_STATUS_BIT(status_variable, STATUS_BIT_TX_STARED)

#define ASSERT_ON_ERROR(ret, errortype)\
        {\
            if(ret < 0)\
            {\
                SHOW_WARNING(ret, errortype);\
                return -1;\
            }\
        }

#define ASSERT_AND_CLEAN_CONNECT(ret, errortype, ConnectParams)\
        {\
            if(ret < 0)\
            {\
                SHOW_WARNING(ret, errortype);\
                FreeConnectCmd(ConnectParams);\
                return -1;\
            }\
        }

#define ASSERT_AND_CLEAN_STARTAP(ret, errortype, StartApParams)\
        {\
            if(ret < 0)\
            {\
                SHOW_WARNING(ret, errortype);\
                FreeStartApCmd(StartApParams);\
                return -1;\
            }\
        }

#define ASSERT_AND_CLEAN_PING(ret, errortype, pingParams)\
        {\
            if(ret < 0)\
            {\
                SHOW_WARNING(ret, errortype);\
                FreePingCmd(pingParams);\
                return -1;\
            }\
        }

#define ASSERT_AND_CLEAN_MDNS_ADV(ret, errortype, mDNSAdvertiseParams)\
        {\
            if(ret < 0)\
            {\
                SHOW_WARNING(ret, errortype);\
                FreemDNSAdvertiseCmd(mDNSAdvertiseParams);\
                return -1;\
            }\
        }

#define ASSERT_AND_CLEAN_RECV(ret, errortype, RecvCmdParams)\
        {\
            if(ret < 0)\
            {\
                SHOW_WARNING(ret, errortype);\
                FreeRecvCmd(RecvCmdParams);\
                return -1;\
            }\
        }


void MotorControl(uint8_t direction);
void StartWiFi(void);
int32_t ConfigureSimpleLinkToDefaultState(void);
int32_t initAppVariables(void);
int32_t createSocket(sockAddr_t*  sAddr);
void *LockSystem_connectProcess(void *pvParameters);
int32_t IsAllowed(int socket,SlSockAddr_t * Sa, char *cruzID);
int8_t ResetRoom(int Sd, SlSockAddr_t * Sa);

#endif /* EMPTY_H_ */
