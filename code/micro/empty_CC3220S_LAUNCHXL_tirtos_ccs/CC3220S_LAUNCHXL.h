
/** ============================================================================
 *  @file       C3220S_LAUNCHXL.h
 *
 *  @brief      CC3220 Board Specific APIs
 *
 *  The CC3220S_LAUNCHXL header file should be included in an application as
 *  follows:
 *  @code
 *  #include <CC3220S_LAUNCHXL.h>
 *  @endcode
 *
 *  ============================================================================
 */
#ifndef __CC3220S_LAUNCHXL_H
#define __CC3220S_LAUNCHXL_H

#ifdef __cplusplus
extern "C" {
#endif

#define CC3220S_LAUNCHXL_GPIO_LED_OFF (0)
#define CC3220S_LAUNCHXL_GPIO_LED_ON  (1)

/*!
 *  @def    CC3220S_LAUNCHXL_ADCName
 *  @brief  Enum of ADC names on the CC3220S_LAUNCHXL dev board
 */
typedef enum CC3220S_LAUNCHXL_ADCName {
    CC3220S_LAUNCHXL_ADC0 = 0,
    CC3220S_LAUNCHXL_ADC1,

    CC3220S_LAUNCHXL_ADCCOUNT
} CC3220S_LAUNCHXL_ADCName;

/*!
 *  @def    CC3220S_LAUNCHXL_CaptureName
 *  @brief  Enum of Capture names on the CC3220S_LAUNCHXL dev board
 */
typedef enum CC3220S_LAUNCHXL_CaptureName {
    CC3220S_LAUNCHXL_CAPTURE0 = 0,
    CC3220S_LAUNCHXL_CAPTURE1,

    CC3220S_LAUNCHXL_CAPTURECOUNT
} CC3220S_LAUNCHXL_CaptureName;

/*!
 *  @def    CC3220S_LAUNCHXL_CryptoName
 *  @brief  Enum of Crypto names on the CC3220S_LAUNCHXL dev board
 */
typedef enum CC3220S_LAUNCHXL_CryptoName {
    CC3220S_LAUNCHXL_CRYPTO0 = 0,

    CC3220S_LAUNCHXL_CRYPTOCOUNT
} CC3220S_LAUNCHXL_CryptoName;

/*!
 *  @def    CC3220S_LAUNCHXL_GPIOName
 *  @brief  Enum of GPIO names on the CC3220S_LAUNCHXL dev board
 */
typedef enum CC3220S_LAUNCHXL_GPIOName {
    CC3220S_LAUNCHXL_GPIO_SW2 = 0,
    CC3220S_LAUNCHXL_GPIO_SW3,
    CC3220S_LAUNCHXL_GPIO_LED_D7,
    CC3220S_LAUNCHXL_PIN_15,
    CC3220S_LAUNCHXL_PIN_18,
    CC3220S_LAUNCHXL_PIN_53,
    CC3220S_LAUNCHXL_PIN_02,
    CC3220S_LAUNCHXL_PIN_01,


    /*
     *  CC3220S_LAUNCHXL_GPIO_LED_D5 and CC3220S_LAUNCHXL_GPIO_LED_D6 are shared with the
     *  I2C and PWM peripherals. In order for those examples to work, these
     *  LEDs are  taken out of gpioPinCOnfig[]
     */
    /* CC3220S_LAUNCHXL_GPIO_LED_D6, */
    /* CC3220S_LAUNCHXL_GPIO_LED_D5, */

    CC3220S_LAUNCHXL_GPIOCOUNT
} CC3220S_LAUNCHXL_GPIOName;

/*!
 *  @def    CC3220S_LAUNCHXL_I2CName
 *  @brief  Enum of I2C names on the CC3220S_LAUNCHXL dev board
 */
typedef enum CC3220S_LAUNCHXL_I2CName {
    CC3220S_LAUNCHXL_I2C0 = 0,

    CC3220S_LAUNCHXL_I2CCOUNT
} CC3220S_LAUNCHXL_I2CName;

/*!
 *  @def    CC3220S_LAUNCHXL_I2SName
 *  @brief  Enum of I2S names on the CC3220S_LAUNCHXL dev board
 */
typedef enum CC3220S_LAUNCHXL_I2SName {
    CC3220S_LAUNCHXL_I2S0 = 0,

    CC3220S_LAUNCHXL_I2SCOUNT
} CC3220S_LAUNCHXL_I2SName;

/*!
 *  @def    CC3220S_LAUNCHXL_PWMName
 *  @brief  Enum of PWM names on the CC3220S_LAUNCHXL dev board
 */
typedef enum CC3220S_LAUNCHXL_PWMName {
    CC3220S_LAUNCHXL_PWM6 = 0,
    CC3220S_LAUNCHXL_PWM7,

    CC3220S_LAUNCHXL_PWMCOUNT
} CC3220S_LAUNCHXL_PWMName;

/*!
 *  @def    CC3220S_LAUNCHXL_SDFatFSName
 *  @brief  Enum of SDFatFS names on the CC3220S_LAUNCHXL dev board
 */
typedef enum CC3220S_LAUNCHXL_SDFatFSName {
    CC3220S_LAUNCHXL_SDFatFS0 = 0,

    CC3220S_LAUNCHXL_SDFatFSCOUNT
} CC3220S_LAUNCHXL_SDFatFSName;

/*!
 *  @def    CC3220S_LAUNCHXL_SDName
 *  @brief  Enum of SD names on the CC3220S_LAUNCHXL dev board
 */
typedef enum CC3220S_LAUNCHXL_SDName {
    CC3220S_LAUNCHXL_SD0 = 0,

    CC3220S_LAUNCHXL_SDCOUNT
} CC3220S_LAUNCHXL_SDName;

/*!
 *  @def    CC3220S_LAUNCHXL_SPIName
 *  @brief  Enum of SPI names on the CC3220S_LAUNCHXL dev board
 */
typedef enum CC3220S_LAUNCHXL_SPIName {
    CC3220S_LAUNCHXL_SPI0 = 0,
    CC3220S_LAUNCHXL_SPI1,

    CC3220S_LAUNCHXL_SPICOUNT
} CC3220S_LAUNCHXL_SPIName;

/*!
 *  @def    CC3220S_LAUNCHXL_TimerName
 *  @brief  Enum of Timer names on the CC3220S_LAUNCHXL dev board
 */
typedef enum CC3220S_LAUNCHXL_TimerName {
    CC3220S_LAUNCHXL_TIMER0 = 0,
    CC3220S_LAUNCHXL_TIMER1,
    CC3220S_LAUNCHXL_TIMER2,

    CC3220S_LAUNCHXL_TIMERCOUNT
} CC3220S_LAUNCHXL_TimerName;

/*!
 *  @def    CC3220S_LAUNCHXL_UARTName
 *  @brief  Enum of UARTs on the CC3220S_LAUNCHXL dev board
 */
typedef enum CC3220S_LAUNCHXL_UARTName {
    CC3220S_LAUNCHXL_UART0 = 0,
    CC3220S_LAUNCHXL_UART1,

    CC3220S_LAUNCHXL_UARTCOUNT
} CC3220S_LAUNCHXL_UARTName;

/*!
 *  @def    CC3220S_LAUNCHXL_WatchdogName
 *  @brief  Enum of Watchdogs on the CC3220S_LAUNCHXL dev board
 */
typedef enum CC3220S_LAUNCHXL_WatchdogName {
    CC3220S_LAUNCHXL_WATCHDOG0 = 0,

    CC3220S_LAUNCHXL_WATCHDOGCOUNT
} CC3220S_LAUNCHXL_WatchdogName;

/*!
 *  @brief  Initialize the general board specific settings
 *
 *  This function initializes the general board specific settings.
 */
extern void CC3220S_LAUNCHXL_initGeneral(void);

#ifdef __cplusplus
}
#endif

#endif /* __CC3220S_LAUNCHXL_H */

//
//#ifndef __CC3220S_LAUNCHXL_H
//#define __CC3220S_LAUNCHXL_H
//
//#ifdef __cplusplus
//extern "C" {
//#endif
//
//#define CC3220S_LAUNCHXL_GPIO_LED_OFF (0)
//#define CC3220S_LAUNCHXL_GPIO_LED_ON  (1)
//
///*!
// *  @def    CC3220S_LAUNCHXL_ADCName
// *  @brief  Enum of ADC names on the CC3220S_LAUNCHXL dev board
// */
//typedef enum CC3220S_LAUNCHXL_ADCName {
//    CC3220S_LAUNCHXL_ADC0 = 0,
//    CC3220S_LAUNCHXL_ADC1,
//
//    CC3220S_LAUNCHXL_ADCCOUNT
//} CC3220S_LAUNCHXL_ADCName;
//
///*!
// *  @def    CC3220S_LAUNCHXL_CaptureName
// *  @brief  Enum of Capture names on the CC3220S_LAUNCHXL dev board
// */
//typedef enum CC3220S_LAUNCHXL_CaptureName {
//    CC3220S_LAUNCHXL_CAPTURE0 = 0,
//    CC3220S_LAUNCHXL_CAPTURE1,
//
//    CC3220S_LAUNCHXL_CAPTURECOUNT
//} CC3220S_LAUNCHXL_CaptureName;
//
///*!
// *  @def    CC3220S_LAUNCHXL_CryptoName
// *  @brief  Enum of Crypto names on the CC3220S_LAUNCHXL dev board
// */
//typedef enum CC3220S_LAUNCHXL_CryptoName {
//    CC3220S_LAUNCHXL_CRYPTO0 = 0,
//
//    CC3220S_LAUNCHXL_CRYPTOCOUNT
//} CC3220S_LAUNCHXL_CryptoName;
//
///*!
// *  @def    CC3220S_LAUNCHXL_GPIOName
// *  @brief  Enum of GPIO names on the CC3220S_LAUNCHXL dev board
// */
//typedef enum CC3220S_LAUNCHXL_GPIOName {
//    CC3220S_LAUNCHXL_GPIO_SW2 = 0,
//    CC3220S_LAUNCHXL_GPIO_SW3,
//    CC3220S_LAUNCHXL_GPIO_LED_D7,
//    CC3220S_LAUNCHXL_GPIO_LED_RED,
//    CC3220S_LAUNCHXL_GPIO_LED_GREEN,
//
//    /*
//     *  CC3220S_LAUNCHXL_GPIO_LED_D5 and CC3220S_LAUNCHXL_GPIO_LED_D6 are shared with the
//     *  I2C and PWM peripherals. In order for those examples to work, these
//     *  LEDs are  taken out of gpioPinCOnfig[]
//     */
//    /* CC3220S_LAUNCHXL_GPIO_LED_D6, */
//    /* CC3220S_LAUNCHXL_GPIO_LED_D5, */
//
//    CC3220S_LAUNCHXL_GPIOCOUNT
//} CC3220S_LAUNCHXL_GPIOName;
//
///*!
// *  @def    CC3220S_LAUNCHXL_I2CName
// *  @brief  Enum of I2C names on the CC3220S_LAUNCHXL dev board
// */
//typedef enum CC3220S_LAUNCHXL_I2CName {
//    CC3220S_LAUNCHXL_I2C0 = 0,
//
//    CC3220S_LAUNCHXL_I2CCOUNT
//} CC3220S_LAUNCHXL_I2CName;
//
///*!
// *  @def    CC3220S_LAUNCHXL_I2SName
// *  @brief  Enum of I2S names on the CC3220S_LAUNCHXL dev board
// */
//typedef enum CC3220S_LAUNCHXL_I2SName {
//    CC3220S_LAUNCHXL_I2S0 = 0,
//
//    CC3220S_LAUNCHXL_I2SCOUNT
//} CC3220S_LAUNCHXL_I2SName;
//
///*!
// *  @def    CC3220S_LAUNCHXL_PWMName
// *  @brief  Enum of PWM names on the CC3220S_LAUNCHXL dev board
// */
//typedef enum CC3220S_LAUNCHXL_PWMName {
//    CC3220S_LAUNCHXL_PWM6 = 0,
//    CC3220S_LAUNCHXL_PWM7,
//
//    CC3220S_LAUNCHXL_PWMCOUNT
//} CC3220S_LAUNCHXL_PWMName;
//
///*!
// *  @def    CC3220S_LAUNCHXL_SDFatFSName
// *  @brief  Enum of SDFatFS names on the CC3220S_LAUNCHXL dev board
// */
//typedef enum CC3220S_LAUNCHXL_SDFatFSName {
//    CC3220S_LAUNCHXL_SDFatFS0 = 0,
//
//    CC3220S_LAUNCHXL_SDFatFSCOUNT
//} CC3220S_LAUNCHXL_SDFatFSName;
//
///*!
// *  @def    CC3220S_LAUNCHXL_SDName
// *  @brief  Enum of SD names on the CC3220S_LAUNCHXL dev board
// */
//typedef enum CC3220S_LAUNCHXL_SDName {
//    CC3220S_LAUNCHXL_SD0 = 0,
//
//    CC3220S_LAUNCHXL_SDCOUNT
//} CC3220S_LAUNCHXL_SDName;
//
///*!
// *  @def    CC3220S_LAUNCHXL_SPIName
// *  @brief  Enum of SPI names on the CC3220S_LAUNCHXL dev board
// */
//typedef enum CC3220S_LAUNCHXL_SPIName {
//    CC3220S_LAUNCHXL_SPI0 = 0,
//    CC3220S_LAUNCHXL_SPI1,
//
//    CC3220S_LAUNCHXL_SPICOUNT
//} CC3220S_LAUNCHXL_SPIName;
//
///*!
// *  @def    CC3220S_LAUNCHXL_TimerName
// *  @brief  Enum of Timer names on the CC3220S_LAUNCHXL dev board
// */
//typedef enum CC3220S_LAUNCHXL_TimerName {
//    CC3220S_LAUNCHXL_TIMER0 = 0,
//    CC3220S_LAUNCHXL_TIMER1,
//    CC3220S_LAUNCHXL_TIMER2,
//
//    CC3220S_LAUNCHXL_TIMERCOUNT
//} CC3220S_LAUNCHXL_TimerName;
//
///*!
// *  @def    CC3220S_LAUNCHXL_UARTName
// *  @brief  Enum of UARTs on the CC3220S_LAUNCHXL dev board
// */
//typedef enum CC3220S_LAUNCHXL_UARTName {
//    CC3220S_LAUNCHXL_UART0 = 0,
//    CC3220S_LAUNCHXL_UART1,
//
//    CC3220S_LAUNCHXL_UARTCOUNT
//} CC3220S_LAUNCHXL_UARTName;
//
///*!
// *  @def    CC3220S_LAUNCHXL_WatchdogName
// *  @brief  Enum of Watchdogs on the CC3220S_LAUNCHXL dev board
// */
//typedef enum CC3220S_LAUNCHXL_WatchdogName {
//    CC3220S_LAUNCHXL_WATCHDOG0 = 0,
//
//    CC3220S_LAUNCHXL_WATCHDOGCOUNT
//} CC3220S_LAUNCHXL_WatchdogName;
//
///*!
// *  @brief  Initialize the general board specific settings
// *
// *  This function initializes the general board specific settings.
// */
//extern void CC3220S_LAUNCHXL_initGeneral(void);
//
//#ifdef __cplusplus
//}
//#endif
//
//#endif /* __CC3220S_LAUNCHXL_H */
