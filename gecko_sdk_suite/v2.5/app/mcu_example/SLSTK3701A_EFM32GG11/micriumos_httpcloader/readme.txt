MicriumOS HTTP Client Gecko Bootloader example.

This example shows how to use the MicriumOS network stack with the ETH 
peripheral on the EFM32GG11B starter kit. This example will initialize
the RMII interface to the external PHY and set up a 100 Mbit connection.

Once connected to the network, the example will send an HTTP/HTTPS GET request
to the server given by the user. If the loaded file is not a (valid) gbl, or
anything goes wrong with the HTTP protocol, the bootloader storage space will
be cleaned and a new GET request will be sent to the given server.
LED 0 will toggle each time a new GET request is sent.

The example will output messages on the VCOM port. The messages will give
some useful information about the reprogramming process.

Micrium OS Support SEGGER SystemView to view the runtime behavior or a system.
SystemView Trace is enabled by default and can be disabled by changing the 
OS_CFG_TRACE_EN configuration inside the os_cfg.h file. SystemView can be used
to inspect the runtime behaviour of this example, it will give a nice overview
of the tasks and interrupts in the application. SystemView can be downloaded 
from https://www.segger.com/systemview.html.

A combined first stage + main bootloader image called 
bootloader-storage-internal-single-combined.s37 is included in the example, 
which can be flashed to the STK using Simplicity Commander.

GBL files can be created using Simplicity Commander by simply running the
command:
"commander gbl create myupgrade.gbl --app myapp.s37 --bootloader mybootloader.s37".
This is described in more details in UG266.

Note:
  - The URL of a web server needs to be added to httpclient.c
  - If using HTTPS connection, the trusted root CA needs to be added to ssl_certificates.c

Board:  Silicon Labs SLSTK3701A_EFM32GG11 Starter Kit
Device: EFM32GG11B820F2048GL192