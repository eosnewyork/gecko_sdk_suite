CAN bus example. 

This example project use EFM32 EMLIB peripheral library to demonstrate 
the use of CAN bus. This demo requires CAN expansion board ISO-CAN-EXP REV 1.0
This example uses glib example to show the CAN actions on the display. 
GPIO interrupt are implemented in order to make use of the buttons. 
BTN0 is used to send a message using CAN0 and LED0 will be turned on. 
BTN1 is used to send a message using CAN1 and LED1 will be turned on.
On the display number of receptions of the messages and number of
transmissions are shown for each CAN peripherals. As well as the id of the
arrived message and the content. To test this example properly, the kit
needs to be connected with the CAN tranceivers and another CAN devices.
This example is intended as a skeleton for new projects. 

Board: Silicon Labs EFM32GG11_STK3701 Starter Kit

Device: EFM32GG11B820F2048GL192, ISO-CAN-EXP REV 1.0