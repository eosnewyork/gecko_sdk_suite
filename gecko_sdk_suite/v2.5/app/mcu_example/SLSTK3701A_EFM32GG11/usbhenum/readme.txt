USB device enumerator example.

This example project uses the USB host stack to implement
a simple device enumerator.

Connect a terminal to VCOM and then connect any USB device to the USB port of
the kit.

Info about the VID/PID and some string descriptors will be output on
the serial port. The connected USB device will not be configured.

Note that USB disk drives may fail, as some of them draw too much current
when attached.

Board:  Silicon Labs SLSTK3701A Starter Kit
Device: EFM32GG11B820F2048GL192
