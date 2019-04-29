LC sense example using LESENSE.

This example project uses the EFM32 CMSIS and demonstrates the use of 
the LESENSE peripheral on the board.

This demo has two modes. To change between them, press BTN0. In 
Single Mode (default) the EFM32 wakes up whenever a metal object is 
passed above the LC sensor in th bottom right hand corner of the STK. 
In 5x Mode, the EFM32 only wakes up every fifth time the metal object 
is passed over the sensor.

The VDAC is set up to drive the LC sensor voltage to half of VDD.
LESENSE is set up to periodically drive the low side of the LC sensor
low for a short time. This makes the LC sensor voltage oscillate
around the VDAC voltage. This oscillation dampens faster if there is
metal close to the sensor.

The ACMP is set up to compare the oscillating LC sensor voltage
against a threshold which is set just above the VDAC voltage. The
output from the ACMP will pulse high whenever the oscillations go
above the threshold voltage. When metal is close to the sensor, the
oscillation dampens faster and there will be fewer pulses on the ACMP
output. Each time after LESENSE drives the LC sensor pin low, it
counts how many pulses it receives from the ACMP and compares this
count against a threshold to determine if there is metal close to the
LC sensor or not.

LESENSE controls both the VDAC and the ACMP and turns these on only
when needed to measure the sensor to save power.

In Single Mode LESENSE is set up to give an interrupt whenever the
ACMP pulse count value is below the counter threshold.

In 5x Mode LESENSE does not wake the EFM32 up, but instead generates a
PRS pulse every time the counter threshold is breached. Each pulse on
this PRS channel will then cause the PCNT to increase its count value
until it reaches it's compare value (5) and wakes up the EFM32.

Board: Silicon Labs SLSTK3402A_EFM32PG12 Starter Kit
Device: EFM32PG12B500F1024GL125
