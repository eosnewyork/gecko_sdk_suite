VCOM communication example.

This example project uses the EFM32 CMSIS and EMLIB to demonstrates the
use of USART and LEUART communication on the SLSTK3701A Starter Kit 
using the virtual COM port.

Virtual COM port through USB is used in by default this example. Undefine 
"RETARGET_VCOM" in the project to use UART on EXP header pins. For EXP header 
UART communication, serial configuration 115200-8-N-1 is used on EXP pin 1 is GND, 
EXP pin 12 is USART0 Tx and EXP pin 14 is USART0 Rx.

If LEUART mode is configured by defining "RETARGET_LEUART0" as a build option,
terminal configuration is 9600-8-N-1 if "RETARGET_VCOM" is not defined, and
115200-8-N-1 if "RETARGET_VCOM" is defined.

Board:  Silicon Labs SLSTK3701A Starter Kit
Device: EFM32GG11B820F2048GL192
