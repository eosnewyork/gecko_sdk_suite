USB HID keyboard example.

This example project use the EFM32 USB Device protocol stack
and implements an USB HID class keyboard device with one button.

BTN0 is the keyboard button. Pressing this button will succesively write
"Silicon Labs - ".
The red led of RGB LED0 toggles each time the keypad (BTN0) is polled and will
be permanently on when BTN0 is pressed.
The blue led of LED0 shows the USB device configured status.

Keyboard NumLock, CapsLock and ScrollLock status is reflected by the color
of RGB led LED1.

Board:  Silicon Labs SLTB009A_EFM32GG12 Thunderboard Kit
Device: EFM32GG12B810F1024GM64
