Digi XBee Device Cloud example.

This example project uses a Digi XBee module to communicate via LTE-M. It
transmits temperature data to the Digi Device Cloud and allows a Cloud user to
modify LED's on the board.

In order to use the Digi Device Cloud, the user must create an account on the
Digi Remote Manager (see www.digi.com/products/cloud/digi-remote-manager).
Once the user has an account on Digi Remote Manager, he should set the compiler
symbols USER to his username and PWD to his password, respectively. Next, 
rebuild the example project. In Studio, these compiler symbols can be set at:

Project Properties->GNU ARM C Compiler->Symbols.

Note: Digi Remote Manager does not currently support controlling board LED's
remotely. In order to exercise this feature, use the included Python script
lte_xbee_device_cloud\scripts\rgb_uploader.py.

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

