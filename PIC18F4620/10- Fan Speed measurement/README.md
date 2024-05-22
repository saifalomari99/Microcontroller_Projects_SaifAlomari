# Fan Speed measurement - Saif Alomari

This project focuses on measuring and controlling the speed of a fan using PWM on a PIC18F4620 microcontroller. 
It integrates various functionalities such as interfacing with an LCD screen, handling IR remote control 
input, measuring temperature via I2C, and controlling fan speed. The lab provides practical experience with 
real-time control and monitoring of hardware components in an embedded system, highlighting the integration 
of multiple peripherals and communication protocols. 

Main Loop:
- Time and Temperature Update: Reads the current time and temperature every second, updates the LCD display, and calculates RPM (revolutions per minute) of the fan.
- IR Remote Control Handling: Checks for IR remote control input, identifies the button pressed, and performs corresponding actions such as setting the time, controlling the fan, and updating the display.
- Fan Speed Control: Adjusts the fan speed based on the button presses from the remote control.

Fan Control:
- Toggle_Fan: Function to toggle the fan on or off.
- Increase_Speed: Function to increase the fan speed.
- Decrease_Speed: Function to decrease the fan speed.



The circuit: 

<img src='./images/circuit_02.jpg' width='400'>

The schematics: 

<img src='./images/schematics_02.png' width='600'>
