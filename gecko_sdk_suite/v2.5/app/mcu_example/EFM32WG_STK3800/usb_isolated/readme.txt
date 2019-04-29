RS232 Isolated Example

This is an example for the EFM32WG_STK3800 Wonder Gecko Starter Kit attached to
the Isolated-USB-EXP. It uses the LEUART port to communicate over isolation to 
a USB-UART bridge. Program the EFM32 WG STK with this demo. Unplug the STK and 
connect the Isolated-USB-EXP to the EFM32 WG STK.  Then, connect the USB cable 
to the EXP board, open a com port to it at 115200 baud. You can enter messages to 
the kit and after pressing "enter" the kit will return your message. On the 
Isolated-USB-EXP, all the jumpers on J2 and J3 need to be populated except the 
SH jumper. If there is no USB host to read the messages, pupolating JP2 will 
send the messages back to the Starter Kit.

Board:  Silicon Labs EFM32WG_STK3800 Starter Kit
Device: EFM32WG990F256