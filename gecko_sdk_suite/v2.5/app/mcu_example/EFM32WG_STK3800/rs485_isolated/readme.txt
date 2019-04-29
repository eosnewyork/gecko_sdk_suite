RS485 Isolated Example

This is an example for the EFM32WG_STK3800 Wonder Gecko Starter Kit attached 
to the Isolated-RS-EXP. It uses the LEUART port to communicate over isolation
to an RS485 tranciever. The push buttons send characters over the isolation 
barrier.  The screen will update with received characters. Lower case 
characters are converted to upper case before being printed on the screen. 
Connect the EXP boards to an RS485 host at 9600 baud. Alternatively connect 2 
Isolated-RS-EXPs together and communicate between the two. On the 
Isolated-RS-EXP, all the jumpers on J3 and J4 need to be populated except the 
SH jumper.

Board:  Silicon Labs EFM32WG_STK3800 Starter Kit
Device: EFM32WG990F256