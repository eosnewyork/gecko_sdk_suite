USB Host mass storage device example.

This example project uses the USB host and the MSD modules in the
drivers directory to implement support for Mass Storage Device's (MSD).

Connect a terminal to VCOM and then connect a memory stick the USB port of the
kit.

Info about the MSD device will be output on the VCOM port. When a MSD device is
connected and detected as a valid, you can issue commands at the VCOM console to
list all files on the device (ls), type the content of a file (cat) and append
text to a file (echo).

Note that USB disk drives may fail, as some of them draw too much current
when attached.

NOTE:
This example is too large to be built with IDEs with 32KB size limits.

Board:  Silicon Labs SLSTK3701A Starter Kit
Device: EFM32GG11B820F2048GL192
