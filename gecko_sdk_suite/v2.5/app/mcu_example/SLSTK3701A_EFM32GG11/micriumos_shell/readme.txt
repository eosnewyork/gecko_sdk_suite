Micrium OS Shell example.

This example shows how to use Micrium OS Shell. The application will use 
the virtual com port on the kit (VCOM) for input and output. Input 
is parsed and executed by Micrium OS Shell. The commands are implemented
inside the cmd_*.c file in the example.

These are the available commands

  help          Prints available commands and usage help.

  init          Various initialization
    -chip       Init of chip, energy mode, dcdc, etc.
    -hfrco FREQ Set HFRCO as core clock and set frequency to FREQ
    -hfxo       Set HFXO as core clock    
    -help       Print init command help text

  selftest      Checks interrupt vector table against memory bounds.
    -help       Print sefltest command help text

  sleep         This command is used to enter various energy modes
    -em MODE    Enter the energy mode specified by MODE. Supported modes 
                are 1, 2, 3, 4H and 4S
    -s N        Wake-up after N seconds
    -help       Print sleep command help text

Example usage

Connect the board and flash it with this program.
Start a VCOM serial terminal connected to the board.
When presented with a prompt, type 'init -chip'.
Now you can run commands such as 'sleep -em 2 -s 3'.

Board:  Silicon Labs SLSTK3701A Starter Kit
Device: EFM32GG11B820F2048GL192
