GLIB example using the Sharp Memory LCD on the SLSTK3701A.

This example project uses the GLIB library and the DISPLAY device driver
together with EFM32 CMSIS including EMLIB and board support package support
functions/drivers to demonstrate driving the 8 color Sharp Memory LCD display on
the SLSTK3701A kit.

Included with the example is also a python script img2rgb3header.py that converts
RGB image files (PNG, BMP, JPG) to 3-bit RGB values stored in a byte array in a C 
header file.

The user can switch trough different GLIB example screens by pushing the BTN0
(forwards) or BTN1 (backwards) buttons.

Board: Silicon Labs SLSTK3701A Starter Kit
Device: EFM32GG11B820F2048GL192
