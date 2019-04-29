QSPI Direct example.

This example demonstrates how to use QSPI direct mode for talking to an 
external QSPI flash device. The flash device used is an MX25R3235F. 
For highest throughput PHY mode is used at a clockrate of 32 MHz.
The flash is conected to the MCU at "slow" location.

To use the example, connect the kit to a computer. Find the 
virtual COM port and access it (for example with PuTTY).
configure the COM for 8N1, 115200 baudrate. Press reset on the bord. 

When the example starts it erases the external flash and writes
a test pattern to the whole flash. This process may take a couple of
minutes. After this pattern is written to the flash the example will
continously run through a set of test functions to measure performance
of various ways of reading from the QSPI flash.

Board:  Silicon Labs EFM32GG11_SLSTK3701 Starter Kit
Device: EFM32GG11B820F2048GL192
