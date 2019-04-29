USB HID keyboard example.

This example project use the EFM32 USB Device protocol stack
and implements an USB HID class keyboard device (one button !).

PB0 is the keyboard button. Pressing this button will succesively write
"Silicon Labs - ".
The RGB color of LED1 will reflect keyboard Num, Caps and Scroll lock status.

Board:  Silicon Labs SLSTK3701A Starter Kit
Device: EFM32GG11B820F2048GL192
