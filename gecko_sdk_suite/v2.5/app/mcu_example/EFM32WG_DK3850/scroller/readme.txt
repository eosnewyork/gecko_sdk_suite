Horizontal single pixel text scroller example.

This example demonstrate driving the EFM32WG_DK3850 kit's TFT-display
from the EFM32 Wonder Gecko.

This example is driving the display in "direct drive" (or mode generic in
SSD2119 terms). Using this mode, the framebuffer resides in the external
PSRAM memory of the development kit. This can be accessed directly to
modify the screen contents.

This demo demonstrates the use of frame buffer control, and implements
a horizontal scroller and shows the hardware assisted masking and
blending capabilities.

The horizontal scroller is implemented by using a large framebuffer,
which is shifted right one pixel for each horizontal scan line, using
the horizontal sync interrupt, and frame base sync trigger  capability
of the Wonder Gecko devices.

The masking and blending is hardware assisted. The geckos being drawn
on screen are using the same software procedure, only adding mask and
enable configurations.

Board:  Silicon Labs EFM32WG-DK3850 Development Kit
Device: EFM32WG990F256
