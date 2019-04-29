Digi XBee SMS example.

This example project uses a Digi XBee module to receive and send SMS text
messages via LTE-M.

To run, build the example project, load it onto the SLSTK3701A board, and
follow the directions on the LCD.

Note: As of August, 2018, Verizon SIM cards do not support SMS via LTE-M.
If the project is built with XBEE_DEMO_CONFIG defined, the application will
configure the XBee to connect only to AT&T-compatible carriers.

Note: When using the Hologram SIM, the incoming and outgoing phone numbers
for the XBee will be different. The outgoing phone number may be a +88 number.
Carriers have been known to block incoming +88 numbers in some instances. If
you are not receiving responses from the XBee, this may be the issue.

At startup, the EFM32GG11 will attempt to communicate with the XBee module
via UART. The application assumes the XBee is configured with a baudrate of
115200 bps and its API mode set to API Mode Without Escapes. If this initial
communication fails, the behavior of the application can be controlled by
defining or not defining a symbol called XBEE_DEMO_CONFIG at the compiler
command line.

If XBEE_DEMO_CONFIG is defined, the application will reset the XBee and then
attempt to configure it to 115200 bps/API Mode Without Escapes.

If XBEE_DEMO_CONFIG is not defined, the application will halt execution in a
while(1) loop at the point of failure.

If XBEE_DEMO_HOLOHRAM_SIM is defined, the application will not try to read the
phone number from the SIM card, because Hologram SIM cards do not store the
phone number.

If XBEE_DEMO_HOLOGRAM_SIM is defined and XBEE_DEMO_CONFIG, the application will
configure the XBee to work with the Hologram SIM card. This configuration
involves disabling NB-IoT on the modem. This is a slow process that takes over
20 seconds. Once NB-IoT has been disabled on a given XBee, that setting is
saved in non-volatile memory. So, if the user is confident that his XBee is
configured correctly for use with the Hologram SIM card, he can remove this
configuration define in the project settings and the demo will still operate
correctly.

If XBEE_CHANGE_APN is defined, the application will configure the Access Point 
Name (APN) setting of the XBee. The default value in "-". In the case of the 
Hologram SIM card, it is recommended to set the APN to "hologram".

Board:  Silicon Labs SLSTK3701A Starter Kit attached to BRD8021A Expansion Board
Device: EFM32GG11B820F2048GL192

