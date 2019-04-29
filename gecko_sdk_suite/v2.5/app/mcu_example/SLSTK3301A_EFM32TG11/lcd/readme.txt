Segment LCD controller example.

This example project uses the EFM32 CMSIS and EMLIB peripheral library and
demonstrates the use of the segment LCD controller with EM2. The demo
stays in EM2 when the segment is not updated.
This example uses the EFM32 Segment LCD Display driver which is provided 
by the Silicon Labs MCU SDK. 

BTN0 is used to enable/disable dynamic charge redistribution.
Dynamic charge redistribution is enabled if the Gecko symbol
is displayed on the right upper corner of LCD. Dynamic
charge redistribution feature can be used to reduce current
consumption.

BTN1 is used to stop/continue the animation.

Board:  Silicon Labs SLSTK3301A_EFM32TG11 Starter Kit
Device: EFM32TG11B520F128GM80