Digi XBee Time Server example.

This example project uses a Digi XBee module to receive the current time from
an online timer server and displays the time on the LCD.

To run, build the example project, load it onto the SLSTK3701A board, and
look at the LCD to see the time.

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

