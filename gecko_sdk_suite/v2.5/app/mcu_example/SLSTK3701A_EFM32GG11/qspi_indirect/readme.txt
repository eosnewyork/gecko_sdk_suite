QSPI Indirect example.

This example demonstrates how to use the indirect mode of the QSPI to write to and 
read from an external flash device. The implementation adds all calls to indirect
reads and writes to two arrays such that all operations are executed and none are
rejected. The maximum number of indirect read and write operations are given by the
size of the two arrays in qspi.c. By uncommenting the various tests in main.c the 
user can verify that it works as expected.

Note: The sectors of the external flash to write to have to be erased before 
writing to them. The QSPI hardware will prioritize indirect write operations
before indirect read operations. All indirect read and writes should be a multiple
of 32 bits and be word aligned.

Board:  Silicon Labs EFM32GG11_SLSTK3701 Starter Kit
Device: EFM32GG11B820F2048GL192
