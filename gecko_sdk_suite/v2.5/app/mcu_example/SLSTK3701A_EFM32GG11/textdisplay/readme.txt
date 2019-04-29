Printf output on Sharp Memory LCD LS013B7DH06.

This example project uses the EFM32 CMSIS including EMLIB and the
board support package support functions/drivers to demonstrate driving
the Sharp Memory LCD display on the EFM32 Giant Gecko 11B. In this case the
display is used as output from a regular printf in the application, as
an alternative to using LEUART or UART for text output.

The user can customize the behaviour of the DISPLAY driver by changing
configuration macros in displayconfigapp.h.

Board:  Silicon Labs SLSTK3701A Starter Kit
Device: EFM32GG11B810F2048
