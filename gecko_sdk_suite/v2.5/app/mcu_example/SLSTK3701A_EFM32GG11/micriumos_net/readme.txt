MicriumOS Network example.

This example shows how to use the MicriumOS network stack with the ETH 
peripheral on the EFM32GG11B starter kit. This example will initialize 
the RMII interface to the external PHY and setup a 100 Mbit connection.

The example will output messages on the VCOM port and it will show status 
on the memory lcd display on the kit. The display will show the current 
link status along with the current IPv4 address and the currently 
configured IPv6 addresses.

Micrium OS Support SEGGER SystemView to view the runtime behavior or a system.
SystemView Trace is enabled by default and can be disabled by changing the 
OS_CFG_TRACE_EN configuration inside the os_cfg.h file. SystemView can be used
to inspect the runtime behaviour of this example, it will give a nice overview
of the tasks and interrupts in the application. SystemView can be downloaded 
from https://www.segger.com/systemview.html

Board:  Silicon Labs SLSTK3701A_EFM32GG11 Starter Kit
Device: EFM32GG11B820F2048GL192
