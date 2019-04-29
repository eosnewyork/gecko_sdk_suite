TFT Addressed map coloured blocks example.

This example project uses the EFM32 CMSIS including EMLIB and the
board support package support functions/drivers to demonstrate driving
the kit's TFT-display from the EFM32 Leopard Gecko.

This example is driving the display in what is called "address mapped
mode" (or mode 8080). Using this mode, the framebuffer resides in the
memory of the SSD2119 controller chip.

WARNING:
SD2119 driver and GLIB graphics library are not intended for production
purposes, and are included here to illustrate TFT display driving only.

These components are subject to changes in API/usage and there will be
no effort to keep compatibility, or to support this software in any way.

Board:  Silicon Labs EFM32LG-DK3650 Development Kit
Device: EFM32LG990F256
