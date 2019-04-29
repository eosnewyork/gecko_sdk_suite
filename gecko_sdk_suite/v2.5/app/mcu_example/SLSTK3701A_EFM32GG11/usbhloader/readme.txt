USB Host Mass Storage Device GBL (Gecko Bootloader) loader example.

This example project uses the USB host and the MSD modules in the
drivers directory to implement support for Mass Storage Device's (MSD). 
Info about the MSD device will be output on the VCOM port. When a MSD device is
connected and detected as a valid, this application will start looking for
a valid gbl file. Once a valid gbl file is found, the flash memory allocated
for the bootloader storage is reprogrammed with the gbl file. 
Right after the bootloader storage is reprogrammed, 
the bootloader upgrade procedure starts and a software reset is performed.

A combined first stage + main bootloader image called 
bootloader-storage-internal-single-combined.s37 is included in the example, 
which can be flashed to the STK using Simplicity Commander.

GBL files can be created using Simplicity Commander by simply running the
command:
"commander gbl create myupgrade.gbl --app myapp.s37 --bootloader mybootloader.s37".
This is described in more details in UG266.

NOTE:
  - USB disk drives may fail, as some of them draw too much current when attached.  

Board:  Silicon Labs SLSTK3701A Starter Kit
Device: EFM32GG11B820F2048GL192
