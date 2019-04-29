Example usage of microphones and MicriumOS HTTP server

This example shows how to sample data from the microphone and also how to
stream that data on a web server using uC/HTTPs.

The audio is sampled from on-kit microphones by using LDMA.
The sampled audio waveform is displayed on LCD. 
Sampling and encoding is performed in the main os task.
Configuration can be found in common_declarations.h and config.h.
For info on encoding, see IETFs and xiphs rfc6716, rfc7845 and rfc3533.

The encoded data is then streamed through a simple no-file-system HTTP server.
Transmission of data packets is handled by an HTTP server task by means
of unicast and chunked transfer encoding.

To listen to the audio: connect the board via ethernet, wait for the IP to be
shown on the display and type that IP into any browser capable of
streaming opus. Because of real-time constraints, there's an upper limit of 5 clients.

Micrium OS Support SEGGER SystemView to view the runtime behavior or a system.
SystemView Trace is enabled by default and can be disabled by changing the 
OS_CFG_TRACE_EN configuration inside the os_cfg.h file. SystemView can be used
to inspect the runtime behaviour of this example, it will give a nice overview
of the tasks and interrupts in the application. SystemView can be downloaded 
from https://www.segger.com/systemview.html

Board:  Silicon Labs SLSTK3701A_EFM32GG11 Starter Kit
Device: EFM32GG11B820F2048GL192
