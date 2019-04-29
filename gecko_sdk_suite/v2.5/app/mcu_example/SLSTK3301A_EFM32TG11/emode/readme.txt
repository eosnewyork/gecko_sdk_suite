Select a single energy mode, and stay there.

This example project uses the EFM32 CMSIS and demonstrates the use of
the LCD display, RTCC, GPIO and various Energy Modes (EM).


1. Use BTN1 to cycle through the available energy mode tests.

Display :
  upper row: frequency (kHz)
  lower row: Test Mode ==> 
    - Energy mode 
    - Clock source (X: HFXO/LFXO, R: HFRCO/LFRCO/ULFRCO) 
    - Operation (i.e. While loop, Prime, Coremark, RTCC, CRYO, None), with/without DC/DC converter
    
See the file lcd_setup.c for more information about the different modes.

2. Press BTN0 to start selected test.

This demo application has been made to give a quick demo of the
Energy Profiler from Silicon Labs, including EFM32 energy modes.

Board:  Silicon Labs EFM32TG11_STK3301A Starter Kit
Device: EFM32TG11B520F128GM80
