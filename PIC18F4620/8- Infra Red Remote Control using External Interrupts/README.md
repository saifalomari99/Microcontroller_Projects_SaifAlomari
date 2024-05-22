# Infra Red Remote Control using External Interrupts - Saif Alomari

This C code is designed for a PIC18F4620 microcontroller and interfaces with an Infra-Red (IR) remote control 
using external interrupts. The lab demonstrates how to use external interrupts to detect IR signals, process 
them, and control an LCD screen and RGB LEDs based on the received IR commands.

System Loop: 

- IR Signal Processing: Continuously checks for IR signals. When a valid signal is detected, it identifies the button pressed on the remote.
- Button Identification: Uses predefined arrays (array1 and txt1) to match the received IR signal with the corresponding button text and action.
- LCD and RGB LED Control: Updates the LCD display with the button text and changes the RGB LED colors based on the button pressed.
- Buzzer Activation: Activates the buzzer for one second when a button press is detected.

The circuit: 

<img src='./images/circuit_08.jpg' width='400'>

The IR Remote interface: 

<img src='./images/graph_08.png' width='500'>

The schematics: 

<img src='./images/schematics_08.png' width='600'>
