CAN bus example. 

This example project use EFM32 EMLIB peripheral library to demonstrate 
the use of CAN bus. This demo requires CAN expansion board ISO-CAN-EXP REV 1.0 
This example uses SegmentLCD to show the CAN actions on the display.
GPIO interrupt are implemented in order to make use of the buttons. 
BTN0 and BTN1 are used to send a message using CAN bus and LEDs will be toggled. 
On the display number of receptions of the messages and number of
transmissions are shown. To test this example properly, the kit
needs to be connected with the CAN tranceivers and another CAN devices.
This example is intended as a skeleton for new projects. 

Board: Silicon Labs SLSTK3301A_EFM32TG11 Starter Kit

Device: EFM32TG11B520F128GM80, ISO-CAN-EXP REV 1.0
