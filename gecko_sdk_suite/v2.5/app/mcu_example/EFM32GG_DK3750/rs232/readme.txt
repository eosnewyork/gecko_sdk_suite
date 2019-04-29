RS232/UART output example.

This example project uses the EFM32 CMSIS and EMLIB, to demonstrates the
use of UART and LEUART communication on the EFM32GG-DK3750 development kit.

For the default UART communication, serial configuration 115200-8-N-1 is used.
If LEUART mode is configured by defining "RETARGET_LEUART1" as a build option,
terminal configuration is 9600-8-N-1.

Board:  Silicon Labs EFM32GG-DK3750 Development Kit
Device: EFM32GG990F1024
