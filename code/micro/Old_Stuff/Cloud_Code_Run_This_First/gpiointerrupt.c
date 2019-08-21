#include <unistd.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
/* Driver Header files */
#include <ti/drivers/GPIO.h>
#include <ti/drivers/SPI.h>
#include <ti/drivers/UART.h>
#include <ti/drivers/net/wifi/simplelink.h>

/* Example/Board Header files */
#include "Board.h"
#include "PN532.h"



void gpioButtonFxn0(uint_least8_t index){
    GPIO_toggle(Board_GPIO_LED0);
    GPIO_toggle(CC3220S_LAUNCHXL_PIN_15);
    GPIO_toggle(CC3220S_LAUNCHXL_PIN_18);
}

// void *mainThread(void *arg0){
int main(void){
    /* Call driver init functions */
    Board_initGeneral();
    GPIO_init();
    SPI_init();
    UART_init();
    
    /* Variables */
    int i;
    UART_Handle uart;
    UART_Params uartParams;

    /*Initialize UART*/
    UART_Params_init(&uartParams);
    uartParams.writeMode = UART_MODE_CALLBACK;
    uartParams.readMode = UART_MODE_CALLBACK;
    uartParams.writeDataMode = UART_DATA_TEXT;
    uartParams.readDataMode = UART_DATA_TEXT;
    uartParams.readReturnMode = UART_RETURN_FULL;
    uartParams.readEcho = UART_ECHO_OFF;
    uartParams.baudRate = 115200;
    uart = UART_open(Board_UART0, &uartParams);
    if (uart == NULL) {
        while (1);
    }

    /* Configure the LED and button pins */
    GPIO_setConfig(Board_GPIO_LED0, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_LOW);
    GPIO_setConfig(Board_GPIO_LED1, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_LOW);
    GPIO_setConfig(Board_GPIO_BUTTON0, GPIO_CFG_IN_PU | GPIO_CFG_IN_INT_FALLING);
    GPIO_write(Board_GPIO_LED0, Board_GPIO_LED_ON);
    GPIO_setCallback(Board_GPIO_BUTTON0, gpioButtonFxn0);
    GPIO_enableInt(Board_GPIO_BUTTON0);
    GPIO_setConfig(CC3220S_LAUNCHXL_PIN_15, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_LOW);
    GPIO_write(CC3220S_LAUNCHXL_PIN_15, 1);
    // GPIO_setConfig(CC3220S_LAUNCHXL_PIN_18, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_LOW);
    // GPIO_write(CC3220S_LAUNCHXL_PIN_18, 0);
    
    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    char string[50];
    
    uint32_t firmwareVersion = 0;
    uint8_t returnConfigureSAM = 0;
    uint32_t cardVersion = 0;
    
    // UART_write(uart, "what\n", 6);
    InitSPI();
    
    
    uint8_t test[1] = {GETFIRMWAREVERSION};
    uint8_t ACKreturn = 0;
    ACKreturn = SendCommandAndACK(test, sizeof(test));
    firmwareVersion = GetFirmwareVersion();
    returnConfigureSAM = ConfigureSAM();
    
    char stringTest[14];
    uint32_t previousCardVersion = 0;
    uint8_t flag = 0;
    while(1){
        Delay(1000000);
        cardVersion = DetectPassiveTarget(ISOIEC14443A);
        if(cardVersion != TIMEOUT){
            if(cardVersion == TIMEOUT && cardVersion == previousCardVersion){
                break;
            }
            sprintf(stringTest, "ID: x%x \r\n", cardVersion);
            UART_write(uart, stringTest, sizeof(stringTest));
            UART_close (uart);
            uart = UART_open(Board_UART0, &uartParams);
            previousCardVersion = cardVersion;
        }
    }

    return (NULL);
}
