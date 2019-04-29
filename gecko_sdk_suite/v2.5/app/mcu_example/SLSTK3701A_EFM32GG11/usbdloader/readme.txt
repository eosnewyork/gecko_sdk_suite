USB Device Mass Storage Device GBL (Gecko Bootloader) loader example.

This example project use the USB device protocol stack
to implement a Mass Storage Class device (MSD). 
Once the STK is connected to a PC via USB, it appears 
as a MSD with a FAT12 formatted disk. MSD is used to
store a gbl file used for bootloader upgrade. When a 
valid gbl file is dragged and dropped into the disk,
the application waits for the file to be completely transferred. 
As soon as the transfer is completed, the flash memory allocated
for the bootloader storage is reprogrammed with the gbl file. 
Right after the bootloader storage is reprogrammed, 
the bootloader upgrade procedure starts and a software reset is performed.

A combined first stage + main bootloader image called 
bootloader-storage-internal-single-combined.s37 is included in the example, 
which can be flushed to the STK using Simplicity Commander.

GBL files can be created using Simplicity Commander by simply running the
command:
"commander gbl create myupgrade.gbl --app myapp.s37 --bootloader mybootloader.s37".
This is described in more details in UG266.

Board:  Silicon Labs SLSTK3701A_EFM32GG11 Starter Kit
Device: EFM32GG11B820F2048GL192