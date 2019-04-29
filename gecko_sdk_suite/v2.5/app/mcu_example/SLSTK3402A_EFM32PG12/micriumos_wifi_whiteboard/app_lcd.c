/***************************************************************************//**
 * @file
 * @brief Wireless Whiteboard's LCD Application Module
 * WGM110 and GLIB demo for the SLSTK3402A running on MicOS.
 * This module besides initializing the SPI, LCD and the GLIB, it also
 * implements a series of predefined screens to display on the LCD.
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#define APP_LCD_MODULE

#include <string.h>

#include "em_usart.h"
#include "em_cmu.h"
#include "em_gpio.h"

#include "gpiointerrupt.h"
#include "display.h"
#include "dmd.h"
#include "glib.h"
#include "bspconfig.h"
#include "displayls013b7dh03config.h"

#include <kernel/include/os.h>
#include <common/include/rtos_utils.h>

#include <app_cfg.h>

#include "app_lcd.h"
#include "app_lcd_logo.h"
#include "app_wifi.h"

// -----------------------------------------------------------------------------
// Global Variables

/// LCD Task control block
static OS_TCB appLcdTaskTCB;

/// LCD Task stack
static CPU_STK appLcdTaskStk[APP_CFG_TASK_LCD_STK_SIZE];

/// GLIB LCD handle
static GLIB_Context_t appLcdGlibContext;

/// Screen to display
static APP_LCD_Screen_t appLcdScreen;

// -----------------------------------------------------------------------------
// Function Prototypes

static void AppLcdTask(void *pArg);
static void AppLcdGpioSetup(void);
static void AppLcdSpiSetup(void);
static void AppLcdShowApInfo(void);
static void AppLcdShowMacAddrInfo(void);
static void AppLcdShowServerInfo(void);
static void AppLcdShowWhiteboard(void);
static void AppLcdGpioCallback(uint8_t pin);

/**************************************************************************//**
 * @brief Initialize the LCD module.
 * @return Void.
 *****************************************************************************/
void APP_LCD_Init(void)
{
  RTOS_ERR   osErr;
  EMSTATUS status;

  // Configure the GPIO for the specific needs of the LCD
  AppLcdGpioSetup();

  // Configure the SPI for the specific needs of the LCD
  AppLcdSpiSetup();

  // Start by displaying the screen "Connecting to AP"
  appLcdScreen = APP_LCD_SCREEN_CONN_AP;

  // Initialize the display module.
  status = DISPLAY_Init();
  if (DISPLAY_EMSTATUS_OK != status) {
    APP_PRINTF(("Error: Unable to initialize the LCD."));
    while(1);
  }

  // Initialize the DMD module for the DISPLAY device driver.
  status = DMD_init(0);
  if (DMD_OK != status) {
    APP_PRINTF(("Error: Unable to initialize the DMD for the LCD."));
    while(1);
  }

  // Initialize the GLIB library
  status = GLIB_contextInit(&appLcdGlibContext);
  if (GLIB_OK != status) {
    APP_PRINTF(("Error: Unable to initialize the LCD library."));
    while(1);
  }

  // Setup display colors
  appLcdGlibContext.backgroundColor = White;
  appLcdGlibContext.foregroundColor = Black;

  // Create the LCD task
  OSTaskCreate(&appLcdTaskTCB,
              "LCD Task",
               AppLcdTask,
               0u,
               APP_CFG_TASK_LCD_PRIO,
              &appLcdTaskStk[0u],
               appLcdTaskStk[APP_CFG_TASK_LCD_STK_SIZE / 10u],
               APP_CFG_TASK_LCD_STK_SIZE,
               0u,
               0u,
               0u,
               (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
              &osErr);

  APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(osErr) == RTOS_ERR_NONE), 1);
}

/**************************************************************************//**
 * @brief LCD task handler.
 * This task waits for a signal to display something on the LCD.
 * @param p_arg Kernel task optional argument pointer.
 * @return Void.
 *****************************************************************************/
static void AppLcdTask(void *pArg)
{
  RTOS_ERR osErr;
  CPU_TS ts;

  // Prevent 'variable unused' compiler warning.
  (void)&pArg;

  while (1) {
    // Wait for a signal to update the LCD
    OSTaskSemPend( 0,
                   OS_OPT_PEND_BLOCKING,
                  &ts,
                  &osErr);
    if (appLcdScreen == APP_LCD_SCREEN_NETWORK_INFO) {
      AppLcdShowMacAddrInfo();
    } else if (appLcdScreen == APP_LCD_SCREEN_WHITEBOARD) {
      AppLcdShowWhiteboard();
    } else if (appLcdScreen == APP_LCD_SCREEN_CONN_AP ||
              appLcdScreen == APP_LCD_SCREEN_CONN_AP_NOT_FOUND) {
      AppLcdShowApInfo();
    } else if (appLcdScreen == APP_LCD_SCREEN_CONN_SERVER) {
      AppLcdShowServerInfo();
    }
    appLcdScreen = APP_LCD_SCREEN_NONE;
  }
}

/**************************************************************************//**
 * @brief Show a predefined screen.
 * This function displays one of the predefined screens on the LCD.
 * @param screen The screen to display.
 * @return Void.
 *****************************************************************************/
void APP_LCD_ShowScreen(APP_LCD_Screen_t screen)
{
  RTOS_ERR osErr;

  appLcdScreen = screen;
  OSTaskSemPost(&appLcdTaskTCB,
                 OS_OPT_POST_NONE,
                &osErr);
}

/**************************************************************************//**
 * @brief Configure the GPIO.
 * Setup the GPIO interrupts for the push buttons.
 * @return Void.
 *****************************************************************************/
static void AppLcdGpioSetup(void)
{
  // Enable GPIO clock.
  CMU_ClockEnable(cmuClock_GPIO, true);

  // Initialize GPIO interrupt dispatcher.
  GPIOINT_Init();

  // Configure PB0 and PB1 as inputs and enable interrupts.
  GPIO_PinModeSet(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN, gpioModeInput, 0);
  GPIO_PinModeSet(BSP_GPIO_PB1_PORT, BSP_GPIO_PB1_PIN, gpioModeInput, 0);

  // Register callbacks before setting up and enabling interrupts.
  GPIOINT_CallbackRegister(BSP_GPIO_PB0_PIN, AppLcdGpioCallback);
  GPIOINT_CallbackRegister(BSP_GPIO_PB1_PIN, AppLcdGpioCallback);

  // Enable interrupts.
  GPIO_IntConfig(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN, false, true, true);
  GPIO_IntConfig(BSP_GPIO_PB1_PORT, BSP_GPIO_PB1_PIN, false, true, true);

  // Configure the interrupt handler
  NVIC_EnableIRQ(GPIO_EVEN_IRQn);

  NVIC_EnableIRQ(GPIO_ODD_IRQn);
}

/**************************************************************************//**
 * @brief Configure the SPI.
 * Setup the SPI to interface the MCU with the LCD.
 * @return Void.
 *****************************************************************************/
static void AppLcdSpiSetup(void)
{
  // Enable GPIO clock.
  CMU_ClockEnable(cmuClock_GPIO, true);

  // Configure PC6 as output (MOSI).
  GPIO_PinModeSet(LCD_PORT_SI,
                  LCD_PIN_SI,
                  gpioModePushPull,
                  1);

  // Configure PC8 as output (CLK).
  GPIO_PinModeSet(LCD_PORT_SCLK,
                  LCD_PIN_SCLK,
                  gpioModePushPull,
                  1);

  // Configure PD14 as output (CS).
  GPIO_PinModeSet(LCD_PORT_SCS,
                  LCD_PIN_SCS,
                  gpioModePushPull,
                  1);

  // Initialize USART for SPI transaction.
  CMU_ClockEnable(PAL_SPI_USART_CLOCK, true);

  // Base the configuration off the default and override it.
  USART_InitSync_TypeDef  usartInit = USART_INITSYNC_DEFAULT;

  usartInit.baudrate    = PAL_SPI_BAUDRATE;
  usartInit.databits    = usartDatabits8;
  usartInit.master      = 1;
  usartInit.msbf        = 1;
  usartInit.clockMode   = usartClockMode1;
  usartInit.prsRxEnable = 0;
  usartInit.prsRxCh     = usartPrsRxCh0;
  usartInit.autoTx      = 0;

  // Initialize the UART.
  USART_InitSync(PAL_SPI_USART_UNIT, &usartInit);

  // Enable pins and establish route locations
  PAL_SPI_USART_UNIT->ROUTEPEN = USART_ROUTEPEN_TXPEN
                                 | USART_ROUTEPEN_CLKPEN
                                 | USART_ROUTEPEN_RXPEN;

  PAL_SPI_USART_UNIT->ROUTELOC0 = (PAL_SPI_USART_UNIT->ROUTELOC0
                                   & ~( _USART_ROUTELOC0_TXLOC_MASK
                                        | _USART_ROUTELOC0_CLKLOC_MASK
                                        | _USART_ROUTELOC0_RXLOC_MASK
                                      )
                                  )
                                  | (PAL_SPI_USART_LOCATION_TX
                                     << _USART_ROUTELOC0_TXLOC_SHIFT)
                                  | (PAL_SPI_USART_LOCATION_SCLK
                                     << _USART_ROUTELOC0_CLKLOC_SHIFT)
                                  | (11 << _USART_ROUTELOC0_RXLOC_SHIFT)
                                  | (11 << _USART_ROUTELOC0_CSLOC_SHIFT);

  PAL_SPI_USART_UNIT->CTRL &= ~USART_CTRL_MSBF;

  // Initialize the PRS trigger.
  USART_PrsTriggerInit_TypeDef initprs = USART_INITPRSTRIGGER_DEFAULT;

  initprs.rxTriggerEnable   = 0;
  initprs.txTriggerEnable   = 0;
  initprs.prsTriggerChannel = usartPrsTriggerCh0;

  USART_InitPrsTrigger(PAL_SPI_USART_UNIT, &initprs);
}

/**************************************************************************//**
 * @brief Display the WiFi module's information.
 * @return Void.
 *****************************************************************************/
static void AppLcdShowMacAddrInfo(void)
{
  GLIB_clear(&appLcdGlibContext);

  GLIB_drawBitmap(&appLcdGlibContext,
                  0,
                  0,
                  APP_LCD_IMG_WIDTH,
                  APP_LCD_IMG_HEIGHT,
                 (uint8_t *)&APP_LCD_LOGO_PixValues[0]);

  GLIB_setFont(&appLcdGlibContext, (GLIB_Font_t *)&GLIB_FontNormal8x8);

  GLIB_drawString(&appLcdGlibContext,
                  "Go to:",
                  6,
                  APP_LCD_CENTER_X - (APP_LCD_FONT_WIDTH * 3),
                  72,
                  0);

  GLIB_drawString(&appLcdGlibContext,
                  "demo.silabs.com",
                  15,
                  4,
                  83,
                  0);

  GLIB_setFont(&appLcdGlibContext, (GLIB_Font_t *)&GLIB_FontNarrow6x8);

  GLIB_drawString(&appLcdGlibContext,
                  "(whiteboard demo)",
                  17,
                  14,
                  95,
                  0);

  GLIB_setFont(&appLcdGlibContext,
               (GLIB_Font_t *)&GLIB_FontNormal8x8);

  GLIB_drawString(&appLcdGlibContext,
                  "MAC Address:",
                  strlen(APP_WIFI_MacAddr),
                  APP_LCD_CENTER_X - ((APP_LCD_FONT_WIDTH
                                       * strlen(APP_WIFI_MacAddr))
                                       / 2),
                  109,
                  0);
  GLIB_drawString(&appLcdGlibContext,
                  APP_WIFI_MacAddr,
                  strlen(APP_WIFI_MacAddr),
                  APP_LCD_CENTER_X - ((APP_LCD_FONT_WIDTH
                                       * strlen(APP_WIFI_MacAddr))
                                       / 2),
                  119,
                  0);

  DMD_updateDisplay();
}

/**************************************************************************//**
 * @brief Display the Access Point information.
 * @return Void.
 *****************************************************************************/
static void AppLcdShowApInfo(void)
{
  GLIB_clear(&appLcdGlibContext);

  GLIB_drawBitmap(&appLcdGlibContext,
                  0,
                  0,
                  APP_LCD_IMG_WIDTH,
                  APP_LCD_IMG_HEIGHT,
                  (uint8_t *)&APP_LCD_LOGO_PixValues[0]);

  GLIB_setFont(&appLcdGlibContext,
               (GLIB_Font_t *)&GLIB_FontNormal8x8);

  GLIB_drawString(&appLcdGlibContext,
                  "Wireless",
                  8,
                  APP_LCD_CENTER_X - (APP_LCD_FONT_WIDTH * 4),
                  80,
                  0);

  GLIB_drawString(&appLcdGlibContext,
                  "Whiteboard",
                  10,
                  APP_LCD_CENTER_X - (APP_LCD_FONT_WIDTH * 5),
                  90,
                  0);

  if (appLcdScreen == APP_LCD_SCREEN_CONN_AP) {
    GLIB_drawString(&appLcdGlibContext,
                    "Connecting to:",
                    14,
                    APP_LCD_CENTER_X - (APP_LCD_FONT_WIDTH * 7),
                    109,
                    0);

    GLIB_drawString(&appLcdGlibContext,
                    APP_WIFI_ApSsid,
                    strlen(APP_WIFI_ApSsid),
                    APP_LCD_CENTER_X - (APP_LCD_FONT_WIDTH
                                        * ((strlen(APP_WIFI_ApSsid)
                                            / 2) + 1)),
                    119,
                    0);
  } else {
    GLIB_drawString(&appLcdGlibContext,
                    "SSID not found:",
                    14,
                    APP_LCD_CENTER_X - (APP_LCD_FONT_WIDTH * 7),
                    109,
                    0);
  }

  DMD_updateDisplay();
}

/**************************************************************************//**
 * @brief Display the TCP server information.
 * @return Void.
 *****************************************************************************/
static void AppLcdShowServerInfo(void)
{
  GLIB_clear(&appLcdGlibContext);

  GLIB_drawBitmap(&appLcdGlibContext,
                  0,
                  0,
                  APP_LCD_IMG_WIDTH,
                  APP_LCD_IMG_HEIGHT,
                  (uint8_t *)&APP_LCD_LOGO_PixValues[0]);

  GLIB_setFont(&appLcdGlibContext,
               (GLIB_Font_t *)&GLIB_FontNormal8x8);

  GLIB_drawString(&appLcdGlibContext,
                  "Wireless",
                  8,
                  APP_LCD_CENTER_X - (APP_LCD_FONT_WIDTH * 4),
                  80,
                  0);

  GLIB_drawString(&appLcdGlibContext,
                  "Whiteboard",
                  10,
                  APP_LCD_CENTER_X - (APP_LCD_FONT_WIDTH * 5),
                  90,
                  0);

  GLIB_drawString(&appLcdGlibContext,
                  "Connecting to:",
                  14,
                  APP_LCD_CENTER_X - (APP_LCD_FONT_WIDTH * 7),
                  109,
                  0);

  GLIB_drawString(&appLcdGlibContext,
                  APP_WIFI_TcpServerIpAddr,
                  strlen(APP_WIFI_TcpServerIpAddr),
                  APP_LCD_CENTER_X - (APP_LCD_FONT_WIDTH
                                      * ((strlen(APP_WIFI_TcpServerIpAddr)
                                          / 2) + 1)),
                  119,
                  0);

  DMD_updateDisplay();
}

/**************************************************************************//**
 * @brief Display the Whiteboard.
 * @return Void.
 *****************************************************************************/
static void AppLcdShowWhiteboard(void)
{
  GLIB_drawBitmap(&appLcdGlibContext,
                  0,
                  0,
                  APP_LCD_IMG_WIDTH,
                  APP_LCD_IMG_HEIGHT,
                  &APP_LCD_PixValues[0]);
  DMD_updateDisplay();
}

/**************************************************************************//**
* @brief GPIO callback function.
* This function handles the push buttons interrupts as follows:
* PB0 Display the Whiteboard.
* PB1 Display Network Information.
* @details This function is called by the gpio interrupt driver
*          in interrupt context.
*****************************************************************************/
static void AppLcdGpioCallback(uint8_t pin)
{
  CPU_SR_ALLOC();

  // Make the kernel aware of the start of the interrupt
  CPU_CRITICAL_ENTER();
  OSIntEnter();
  CPU_CRITICAL_EXIT();

  // Act on interrupts.
  if (pin == BSP_GPIO_PB0_PIN) {
    // BTN0: Display the Whiteboard.
    APP_LCD_ShowScreen(APP_LCD_SCREEN_WHITEBOARD);
  }

  if (pin == BSP_GPIO_PB1_PIN) {
    // BTN1: Display Network Information.
    APP_LCD_ShowScreen(APP_LCD_SCREEN_NETWORK_INFO);
  }

  // Make the kernel aware of the end of the interrupt
  OSIntExit();
}
