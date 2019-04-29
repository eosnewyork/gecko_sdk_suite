This project demonstrates use of NVM3 and mbedTLS Random Number Generator.

This example project uses the mbedTLS Random Number Generator API (TRNG accelerated)
to generate random object data stored in NVM3. The random data is read back from NVM3
and displayed every 2 sec. Use the AEM/BAT switch or reset button on the STK to
interrupt the application. The application should boot up and display the most recent and
successfully stored data from NVM3. Both LEDs will be set to on if an error is detected.

Board:  Silicon Labs SLSTK3701A Starter Kit
Device: EFM32GG11B820F2048GL192
