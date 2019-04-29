Gterm - an asynchronous termianl example.

This example demonstrates how to use the display and a uart to make a simple asynchronous terminal
by utilizing the textdisplay and uartdrv drivers.

The uartdrv driver is configured to use vcom and stdout is configured to print to the display
through textdisplay.

Received data is displayed on the screen, while pressing B0 or B1 will transmit some predefined
strings.

Board:  Silicon Labs SLSTK3400A_EFM32HG Starter Kit
Device: EFM32HG322F64
