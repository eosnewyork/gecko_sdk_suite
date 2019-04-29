MicriumOS Blink example.

This example shows how to initialize and run MicriumOS on the kit. The 
application will start a single task which is blinking LED 0 at a regular 
interval. This examples is a good starting point for applications that want 
to use MicriumOS.

Micrium OS Support SEGGER SystemView to view the runtime behavior or a system.
SystemView Trace is enabled by default and can be disabled by changing the 
OS_CFG_TRACE_EN configuration inside the os_cfg.h file. SystemView can be used
to inspect the runtime behaviour of this example, it will give a nice overview
of the tasks and interrupts in the application. SystemView can be downloaded 
from https://www.segger.com/systemview.html

Board:  Silicon Labs SLSTK3701A Starter Kit
Device: EFM32GG11B820F2048GL192
