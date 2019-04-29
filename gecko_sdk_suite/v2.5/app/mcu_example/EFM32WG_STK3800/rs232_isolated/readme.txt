RS232 Isolated Example

This is an example for the EFM32WG_STK3800 Wonder Gecko Starter Kit attached
to the Isolated-RS-EXP. It uses the UART port to communicate over isolation to
an RS232 tranciever. The buttons send characters and the screen shows 
receieved characters. Use a terminal program like putty at 115200 baud to send
and recieve characters. Lower case characters are converted to upper case before
being printed to the screen. Alternatively, use 2 Isolated-RS-EXPs to 
communicate between each other. On the Isolated-RS-EXP, all the jumpers on J1 
and J2 need to be populated except the SH jumper. If there is not a 2nd device 
or host to read the RS232 messages, populating JP1 and JP2 will cause the 
messages to return to the Starter Kit to be read.

Board:  Silicon Labs EFM32WG_STK3800 Starter Kit
Device: EFM32WG990F256