Progamming and use of user flash page for data.

This example project uses the EFM32 CMSIS  and demonstrates the programming of
the user page in flash. The page can be used for data such as keys, serial
numbers and other information that should not be deleted by a device erase.

PB0 increases the main counter.
PB1 saves the main counter value to the user page and displays how many Flash writes
have been issued.

Board:  Silicon Labs EFM32-Gxxx-STK Development Kit
Device: EFM32G890F128
