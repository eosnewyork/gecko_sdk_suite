/***************************************************************************//**
 * @file
 * @brief MPU example for EFM32_G8xx_STK.
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

#include <string.h>
#include "em_device.h"
#include "em_chip.h"
#include "em_rtc.h"
#include "em_cmu.h"
#include "em_gpio.h"
#include "segmentlcd.h"
#include "rtcdriver.h"
#include "bsp_trace.h"

/***************************************************************************//**
 *
 *  This example project demonstrate use of the memory protection unit (MPU).
 *  The cpu will normally run in Priviledged state, but when a pushbutton
 *  is pressed on the STK it will switch into User state and intentionally
 *  generate access violation exceptions.
 *
 *  The violation forced by hitting pushbutton PB0 is made by accessing
 *  internal SRAM. Hitting PB1 forces exception by accessing the LCD
 *  peripheral.
 *
 *  To be able to execute in User state at all, User state access must be
 *  granted to flash and SRAM memory. We use two MPU regions for this.
 *  Additionally one MPU section is used to deny User state access to a small
 *  part of SRAM. This section overlaps the section used to grant general
 *  general SRAM access, but it will take precedence because its section number
 *  is higher.
 *  A fourth MPU section is used to deny User state access to LCD registers.
 *
 *  Entry into User state is performed by writing to the CONTROL cpu core
 *  register. Before exiting the memory protection fault handler the cpu
 *  is switched back to Priviledged state. (It is impossible to switch from
 *  User state to Priviledged state in Thread mode).
 *
 ******************************************************************************/

/** Pushbutton macro's */
#define PB0() (GPIO->P[1].DIN & (1 << 9) )
#define PB1() (GPIO->P[1].DIN & (1 << 10) )
#define PB0_PUSHED() (!PB0() )
#define PB1_PUSHED() (!PB1() )

// This table contains 4 MPU region definitions.
static const ARM_MPU_Region_t mpuTable[] = {
  // *INDENT-OFF*
  //            RegionNo    BaseAddr
  //            DisableExec AccessPermission TypeExtField IsShareable IsCacheable IsBufferable SubRegionDisable Size
  // Flash memory
  { ARM_MPU_RBAR(0UL,       FLASH_MEM_BASE),
    ARM_MPU_RASR(0UL,       ARM_MPU_AP_FULL, 0UL,         0UL,        1UL,        0UL,         0x00UL,          ARM_MPU_REGION_SIZE_1MB) },

  // SRAM
  { ARM_MPU_RBAR(1UL,       RAM_MEM_BASE),
    ARM_MPU_RASR(0UL,       ARM_MPU_AP_FULL, 0UL,         1UL,        1UL,        0UL,         0x00UL,          ARM_MPU_REGION_SIZE_128KB) },

  // SRAM, a 4k part with priviledged only access, this regions settings will override those of the previous region
  { ARM_MPU_RBAR(2UL,       RAM_MEM_BASE + 0x2000),
    ARM_MPU_RASR(0UL,       ARM_MPU_AP_PRIV, 0UL,         1UL,        1UL,        0UL,         0x00UL,          ARM_MPU_REGION_SIZE_4KB) },

  // LCD, priviledged only access
  { ARM_MPU_RBAR(3UL,       LCD_BASE),
    ARM_MPU_RASR(1UL,       ARM_MPU_AP_PRIV, 0UL,         1UL,        0UL,        1UL,         0x00UL,          ARM_MPU_REGION_SIZE_128B) }
  // *INDENT-ON*
};

/***************************************************************************//**
 * @brief LCD scrolls a text over the display, sort of "polled printf".
 ******************************************************************************/
static void ScrollText(char *scrolltext)
{
  int  i, len;
  char buffer[8];

  buffer[7] = 0x00;
  len       = strlen(scrolltext);
  if (len < 7) {
    return;
  }
  for (i = 0; i < (len - 7); i++) {
    memcpy(buffer, scrolltext + i, 7);
    SegmentLCD_Write(buffer);
    RTCDRV_Delay(300);
  }
}

/***************************************************************************//**
 * @brief  Fire energy mode segments on the LCD.
 ******************************************************************************/
static void BullsEye(int mode)
{
  if ( mode ) {
    SegmentLCD_EnergyMode(0, 1);
    RTCDRV_Delay(100);
    SegmentLCD_EnergyMode(1, 1);
    RTCDRV_Delay(100);
    SegmentLCD_EnergyMode(2, 1);
    RTCDRV_Delay(100);
    SegmentLCD_EnergyMode(3, 1);
    RTCDRV_Delay(100);
    SegmentLCD_EnergyMode(4, 1);
    RTCDRV_Delay(100);
  } else {
    SegmentLCD_EnergyMode(0, 0);
    SegmentLCD_EnergyMode(1, 0);
    SegmentLCD_EnergyMode(2, 0);
    SegmentLCD_EnergyMode(3, 0);
    SegmentLCD_EnergyMode(4, 0);
  }
}

/***************************************************************************//**
 * @brief  Init GPIO to read pushbutton inputs.
 ******************************************************************************/
static void GpioInit(void)
{
  /* Enable GPIO clock */
  CMU_ClockEnable(cmuClock_GPIO, true);

  /* Configure PB10 as input */
  GPIO_PinModeSet(gpioPortB, 10, gpioModeInput, 0);

  /* Configure PB9 as input */
  GPIO_PinModeSet(gpioPortB, 9, gpioModeInput, 0);
}

/***************************************************************************//**
 * @brief  Convert a hex nibble to ASCII.
 ******************************************************************************/
static char Hex2Char(uint32_t value)
{
  value &= 0xF;

  if ( value < 10 ) {
    return '0' + value;
  }

  return 'A' + value - 10;
}

/***************************************************************************//**
 * @brief  Hard fault exception handler.
 ******************************************************************************/
void HardFault_Handler(void)      /* We should never end up here !           */
{
  for (;; ) {
    SegmentLCD_Write(" HARD  ");
    RTCDRV_Delay(800);
    SegmentLCD_Write(" FAULT ");
    RTCDRV_Delay(800);
  }
}

/***************************************************************************//**
 * @brief  Memory protection fault first level exception handler.
 ******************************************************************************/
#ifdef __CC_ARM  /* MDK-ARM compiler */

__asm void MemManage_Handler(void)
{
  EXTERN MemManage_HandlerC
  TST   LR, #4
  ITE   EQ
  MRSEQ R0, MSP
  MRSNE R0, PSP
  B MemManage_HandlerC
}
#else

#if defined(__GNUC__)
void MemManage_Handler(void) __attribute__ ((naked));
#endif
void MemManage_Handler(void)
{
  /*
   * Get the appropriate stack pointer, depending on our mode,
   * and use it as a parameter to a C handler.
   */
  __asm("TST   LR, #4         \n"
        "ITE   EQ             \n"
        "MRSEQ R0, MSP        \n"
        "MRSNE R0, PSP        \n"
        "B MemManage_HandlerC \n");
}
#endif

/***************************************************************************//**
 * @brief  Memory protection fault second level exception handler.
 ******************************************************************************/
void MemManage_HandlerC(uint32_t *stack)
{
  static char text[80], s[5];
  uint32_t pc;

  pc = stack[6];                  /* Get stacked return address              */

  strcpy(text, "    MPU FAULT AT PC 0x");
  s[0] = Hex2Char(pc >> 12);
  s[1] = Hex2Char(pc >> 8);
  s[2] = Hex2Char(pc >> 4);
  s[3] = Hex2Char(pc);
  s[4] = '\0';
  strcat(text, s);
  strcat(text, "        ");
  ScrollText(text);

  SCB->CFSR |= 0xFF;              /* Clear all status bits in the            */
                                  /* MMFSR part of CFSR                      */
  __set_CONTROL(0);               /* Enter Priviledged state before exit     */
}

/***************************************************************************//**
 * @brief  Main function.
 ******************************************************************************/
int main(void)
{
  int i;

  /* Chip alignment */
  CHIP_Init();

  /* If first word of user data page is non-zero, enable Energy Profiler trace */
  BSP_TraceProfilerSetup();

  /* Enable LCD without voltage boost */
  SegmentLCD_Init(false);
  SegmentLCD_AllOff();
  SegmentLCD_Symbol(LCD_SYMBOL_GECKO, 1);
  SegmentLCD_Symbol(LCD_SYMBOL_EFM32, 1);

  GpioInit();

  /* Initialize RTC timer. */
  RTCDRV_Init();

  ScrollText("        MPU DEMO  PRESS Pb0 OR Pb1 "
             "TO GENERATE MPU EXCEPTIONS         ");

  /* Set up the MPU. */
  ARM_MPU_Disable();
  ARM_MPU_Load(mpuTable, 4);        /* Load all MPU settings from the table */
  ARM_MPU_Enable(MPU_CTRL_PRIVDEFENA_Msk); /* Full access to default memory */
                                           /* map in priviledged state      */

  i = 0;
  while ( 1 ) {
    SegmentLCD_Number(i);            /* Count on numeric diplay */
    i = (i + 1) % 101;
    RTCDRV_Delay(150);

    if ( PB0_PUSHED() ) {
      BullsEye(1);

      /* Generate an access violation in internal SRAM          */
      __set_CONTROL(1);     /* Enter User (unpriviledged) state */
      *(volatile uint32_t *)(RAM_MEM_BASE + 0x2000) = 1;

      BullsEye(0);
    }

    if ( PB1_PUSHED() ) {
      BullsEye(1);

      /* Generate an access violation in LCD peripheral         */
      __set_CONTROL(1);     /* Enter User (unpriviledged) state */

      BullsEye(0);
    }
  }
}
