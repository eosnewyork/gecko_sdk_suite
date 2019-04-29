Micrium OS Dynamic Tick example.

This example shows how to initialize and run MicriumOS on the kit. The 
application will start a single task which is blinking LED 0 at a regular 
interval. This example is using the dynamic tick feature of Micrium OS 
to be able to enter EM2 whenever the application is idle. This examples 
is a good starting point for applications that want to use Micrium OS 
while using EM2.

Micrium OS Support SEGGER SystemView to view the runtime behavior or a system.
SystemView Trace is disabled by default in this example, but can be enabled by 
changing the OS_CFG_TRACE_EN configuration inside the os_cfg.h file. 
SystemView can be used to inspect the runtime behaviour of this example, it 
will give a nice overview of the tasks and interrupts in the application. 
SystemView can be downloaded from https://www.segger.com/systemview.html

Board:  Silicon Labs SLSTK3401A Starter Kit
Device: EFM32PG1B200F256
