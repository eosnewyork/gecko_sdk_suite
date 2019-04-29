USB audio microphone device example.

This example implements a USB connected microphone device. The device enumerates
as a device supporting stereo 16 bit PCM encoded audio at a samplerate of
44.1 kHz (the standard audio CD samplerate).

The PCM samples are aquired using the Pulse Density Modulation (PDM) peripheral
of the microcontroller.

The USB device is modelled as the MT (Microphone Topology) described in
USB-IF document "Universal Serial Bus Audio Device Class Specification for
Basic Audio Devices", Release 1.0, November 24th, 2009.

The PDM clock is programmed to give a PCM samplerate of 44,110.28 kHz.
This is not exactly the specified samplerate, and in order to compensate for
this and for clock drift between device and host, additional audio samples
are occasionally inserted or removed from the data stream sent to host to ensure
that the data streams (USB and PCM) stay synchronized.

Board:  Silicon Labs SLTB009A_EFM32GG12 Thunderboard Kit
Device: EFM32GG12B810F1024GM64
