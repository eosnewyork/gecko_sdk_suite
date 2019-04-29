Wireless Whiteboard Demo: Micrium OS + WiFi + Graphics LCD

In this demo you get to draw something on your smart phone and watch the
drawing being displayed in the board's LCD. 

Hardware Requirements
---------------------

This demo requires the three boards illustrated in the following diagram:

 +---------------------------------------+  +----------------------------+
 | EFM32 Giant Gecko                     |  | Wireless Expansion Board   |
 | SLSTK3701A                            |--| PCB8016                    |
 | BRD2204A                              |--|           +--------------+ |
 |                                       |--|           | Wizard Gecko | |
 |                                       |--|           | WGM110       | |
 |                                       |--|           | PCB4320      | |
 |                                       |--|           |              | |
 |                                       |--|           |              | |
 |                                       |--|           +--------------+ |
 |    SWITCH                             |--|   SWITCH                   |
 |  BAT  [AEM]                           |  |  3V  [5V]                  |
 +---------------------------------------+  +----------------------------+
                                        20-pin
                                       Exp. Hdr.

Configuration
-------------

1. Connect the boards as illustrated in the diagram above.

2. Configure the board switches as illustrated in the previous diagram:

   a. AEM
   b. 5V

3. Enter your WiFi access point's SSID and Password in the file app_wifi_cfg.h

4. Build the project.

Execution
---------

1. Execute the application by launching a debug session and wait for the
   board to connect to the TCP server at demo.silabs.com

   The application will display a series of messages to show you the progress
   as follows:

   a. Connecting to [Your SSID]
   b. Connecting to [demo.silabs.com resolved IP address]
   c. MAC Address: [Your MAC address]

2. Open your favorite browser, go to the web page at http://demo.silabs.com
   and select the Wireless Whiteboard demo for the Giant Gecko.

3. Enter your board's MAC address in the web page and start drawing.

4. Press the buttons to switch between screens.

Tools
-----

This demo features two development tools; uC/Probe and SystemView.

To learn more about these tools take the following steps:

1. uC/Probe:

   Click the button uC/Probe on the toolbar of Simplicity Studio.

2. SystemView:

   a. Install SystemView from https://www.segger.com/systemview.html
   
   b. Enable kernel tracing by setting OS_CFG_TRACE_EN to DEF_ENABLED
      in os_cfg.h
   
   c. Rebuild the project.
 
   d. Launch a debugging session.
   
   e. Open SystemView and press the F5 key or select Target from the
      Options Menu and click Start Recording.

Board: Silicon Labs SLSTK3701A_EFM32GG11 Starter Kit
Device: EFM32GG11B820F2048GL192
Expansion Board: PCB8016
WiFi Module: Wizard Gecko WGM110 PCB4320
