Spectrum Analyzer Example

This example demonstrates a real-time audio spectrum analyzer with a
waterfall spectrogram display. The example uses the STK's color LCD to display
frequency data obtained from either the ADC (connected to Pin PB11/Expansion
Header Pin 11) or the microphones present on the STK. Cortex-M4 and 
CMSIS DSP_Lib capabilities are demonstrated in this example.

The example will display the frequency content of any time varying signal
present at the input source. The top portion of the screen displays the most
recently calculated frequency response (around 15 milliseconds of time domain
data), and the bottom portion displays a waterfall spectrogram that provides
a running history of color-coded past frequency responses. The scale at the
top right of the screen shows how the color corresponds to magnitude. Each 
pixel width on the display corresponds to a 66Hz wide frequency bin.

The ADC input mode uses a 5V reference voltage, so the ideal range input would
be a +/-2.5V signal with a +2.5V DC offset. 

Also note that the sampling frequency is 34133 Hz, and content at frequencies
greater than ~17Khz will be aliased.

Board:  Silicon Labs SLSTK3701A Starter Kit
Device: EFM32GG11B820F2048GL192
Pin PB11 (Expansion Header Pin 11) - ADC0 input