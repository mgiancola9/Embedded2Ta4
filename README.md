# Lab 01

# Introduction
This assignment will introduce you to the software and hardware development tools you will use during the semester to work with the STM32F429I-Discovery development board and the Cortex-M4 ARM Processor so that you may run programs using Keil Studio Cloud. The example code provided with this assignment shows you:

How to setup a project using Keil Studio
How to use timer interrupts to interface with the LEDs in the STM32F429-Discovery board using the "user push-button" on the board.


# Prelab
Reading: This lab combines elements from Chapters 1-6 of the course textbook. You should read and understand those chapters. Note that we will focus on C++ in this course, so you do not need to digest the sections on C or MicroPython. Pay specific attention to Chapter 5 (Interrupts and Power Management) and Chapter 6 (Timing Operations).

Hardware

STM32F429-Discovery
Mini-USB

The STM32F429-Discovery is powered via the mini-USB connection and no external power supply will be needed although this option is available. When using the mini-USB connection to power the board you must be very careful that any other peripherals that are using either the 3V or 5V output do not exceed 100mA otherwise you will burn the mini-USB interface on the board.

Setup Procedure

# Log in to Keil Studio Cloud

Connect your STM32F429-Discovery board to a USB port of your choice. Let Windows try to find the drivers. If the drivers are not found then the st-link usb drivers are missing and you will need assistance from the TAs.
Create new project called “MT2TA4-2024-Lab-01". Go to File -> New -> Mbed Project and select "empty Mbed OS project" from Example projects the dropdown. Name the project and uncheck "Initialize this project as a Git repository".
Set Active project to MT2TA4-2024-Lab-01
Set Build target to DISCO-F429ZI
Set Connected device to DISCO-F429ZI
Copy and paste the code from this lab's main.cpp file into main.cpp within your Keil Studio project.
Compile and Run the program using the run button: 


This will compile and download the project to the discovery board.
You should see LED4 (red) blink once per second. We call this phase 0 in the code. Pressing the user (blue) button will make LED4 (red) and LED3 (green) blink in a turn taking manner: LED4 on, LED4 off, LED3 on, LED3 off, and then repeat. This is phase 1. Pressing the user button a second time will switch to phase 2, which adds a 1-second period in which both LEDs are off between the blinking of LED4 and LED3. Additional button presses will switch between phase 1 and phase 2. As you can see from the code, this behaviour is achieved using a software-wait loop.

# Lab Requirements
Modify the demo code downloaded above to achieve the exact same behaviour using timer interrupts instead of software waits. The while loop in your main method should now be empty.

# Marking Scheme


40 pts A functional program that implements all the requirements correctly.

10 pts Motivate your design and implementation decisions to your TA and answer questions about your code.

3 pts main.cpp source file uploaded to your Avenue drop box for lab-01. In the event your project isn't fully functional, this may be used to justify partial marks.

Be prepared to demo the program you wrote to your TA in lab and also to defend your design and implementation decisions.

# Lab 02: Reaction Time Tester

# Introduction
In this lab you will be asked to implement a reaction time tester. The purpose is to see how fast you can push a button after an LED light turns on and to display that time on an LCD screen. You will interface the MCU with an external pushbutton as well as with the available user push button and LCD unit on the Discovery board.

# Prelab
Reading: This lab combines elements from Chapters 1-6, and 11 of the course textbook. You should read and understand those chapters. Note that we will focus on C++ in this course, so you do not need to digest the sections on C or MicroPython. Pay specific attention to Chapter 5 (Interrupts and Power Management), Chapter 6 (Timing Operations), and Chapter 11 (LCD, Touch Screen, and Graphical User Interface Formation). You should also read up on Finite State Machines (FSMs) in this IEEE Software Article and this paper on Finite State Machines in Embedded Applications.
Draw a Finite State Machine (FSM) to implement the behavior described in the Requirements section below. Consider using a table too to describe the system behavior for different push button inputs to make sure you have completely and consistently specified the behavior.
Upload the FSM (in a document or picture file) to Avenue. One copy per group please.

Hardware

STM32F429-Discovery
Mini-USB
Pushbutton
Breadboard
2 breadboard wires
The LCD screen mounted on the Discovery board is a 2.41", 262K color display with QVGA resolution (240 x 320 dots). The LCD is directly driven by the STM32F429ZIT6 MCU using the RGB protocol. The communication between the MCU and the LCD is done through the SPI interface (for more details, you can find the electrical schematic on p.33 in the STM32F429I Discovery Board User Guide). You will not use the SPI interface directly though, all these low level details are already implemented in the firmware. We will use a C++ library provided by the textbook authors (details in Setup Procedure below).


# Setup Procedure

Log in to Keil Studio Cloud

Connect your STM32F429-Discovery board to a USB port of your choice. Let Windows try to find the drivers. If the drivers are not found then the st-link usb drivers are missing and you will need assistance from the TAs.
Create new project called “MT2TA4-2024-Lab-02". Go to File -> New -> Mbed Project and select "empty Mbed OS project" from Example projects the dropdown. Name the project and uncheck "Initialize this project as a Git repository".
Set Active project to MT2TA4-2024-Lab-02
Set Build target to DISCO-F429ZI
Set Connected device to DISCO-F429ZI
Visit the BSP driver page and copy the link under the Import into Keil Studio dropdown. Then, in Keil Studio, got to File -> Add MBed Librry to Active Project. Enter the link in the URL field and click next. Select the default branch.
Repeat Step 7 to add the LCD library to your project.
Copy and paste the code from this lab's main.cpp file into main.cpp within your Keil Studio project.
Compile and Run the program using the run button: 


This will compile and download the project to the discovery board.
You should see "Hello World" and a running count of the number of seconds since reset printed on the LCD display.

External Pushbutton
The STM32F429I-Discovery board has only one available user button, so you will need to connect an external pushbutton to the GPIO ports. Generally we need to connect an external pull-up resistor (wikipedia reference) to the GPIO pin in order to avoid a floating value being read into the input GPIO pin, as shown in the picture below.

However, instead of an external pull-up resistor, we can make use of the internal pull-up resistor provided by the GPIO port. By doing so, an external push button can be directly connected to one of the pins on the development board (while the other side is connected directly to ground). Chapter 4 of the textbook (Section 4.3.3.2 C++ Language) discusses this in detail. For example, to configure PA6 as a digital interrupt input with internal pull-up use the following code:

InterruptIn external_button(PA_6, PullUp);



# Lab Requirements
Write a program to implement the following behaviour:

Upon a reset signal, flash the grenn LED on the development board at ~10 Hz and wait for a user button (on-board button) press.
At the user button press, turn the LED off for a period between 1 and 5 seconds. Figure out a way to make the waiting time random, so that it is harder to predict. You may use the rand function for this.
After the variable time delay, turn on the green LED, start a timer and wait for an onboard user button press. (your code should detect the cheating condition of a pressed button at t=0 and respond by returning to the flashing LED).
At the onboard user button press, compute the time between LED illumination and the button press. Display two reaction times on the LCD, one from the latest test, and one from the the fastest recorded test so far. Displayed time should be accurate to 1 mSec.
When the external pushbutton is pushed at any time/state, clear the LCD, reset the fastest time, and return to the flashing LED state (number 1 above).

All timing must be done with interrupt-driven hardware timers and not with software wait-loops.

# Marking Scheme


10 pts A Finite State Machine diagram for the behaviour outlined in the requirements.

40 pts A functional program that implements all the requirements correctly.

10 pts Motivate your design and implementation decisions to your TA and answer questions about your code.

3 pts main.cpp source file uploaded to your Avenue drop box for lab-02. In the event your project isn't fully functional, this may be used to justify partial marks.

Be prepared to demo the program you wrote to your TA in lab and also to defend your design and implementation decisions.

Useful Mbed OS 6 APIs
DigitalOut |
IterruptIn |
Ticker |
Timeout |
Timer |

# Lab 03: Serial Communication and Non-Volatile Memory

# Introduction
This lab will help you to understand how the I2C serial communication protocol works. You will interface the STM32F4 Discovery board and STM32F429 MCU with an external 24FC64F EEPROM chip that communicates over I2C, and create the corresponding I2C bus circuit on your breadboard. You will also use the internal real-time clock (RTC) of the STM32F429.

# Prelab
This lab assumes you are familiar with the material required for Lab1 and Lab2. In particular, you should feel comfortable structuring your programs using Finite State Machines.
Reading: This lab combines elements from Chapters 1-6, 8, 11  of the course textbook. You should read and understand those chapters. Note that we will focus on C++ in this course, so you do not need to digest the sections on C or MicroPython. Pay specific attention to Chapter 8 (Digital Communication) to learn about I2C.
Review the datasheet of the 24FC64F for the I2C EEPROM chip. Focus on p.1-7 paying particular attention to the addressing info at the bottom of page 4.
Draw a FSM that implements the behavior described in the Requirements section below and upload your diagram to avenue before the beginning of your lab session.

# Hardware

STM32F429-Discovery
Mini-USB
Pushbutton switch (3)
Breadboard
Breadboard wires
24FC64F EEPROM
4.7k resistor (2)
100 ohm resistors (2)
10 uF capacitor


# Circuit
The schematic diagram for wiring the STM32F429-DISCO board to EEPROM is given below:
![image](https://github.com/mgiancola9/Embedded2Ta4/assets/97264168/9ac066a5-7f0e-4fc8-8384-94889f4d026f)

On the breadboard create the circuit shown above.
The Vcc power sources may be connected to the 3V output pin of the STM32F4 Discovery board. This is sufficient to power everything. As was the case in Lab 2, we will use internal pull-up resistors for the switches instead of external pull-ups. Make sure you have the internal pull-ups enabled! (Note the schematic shows a normally-closed button but we use normally-open buttons). The SCL and SDA lines require external pull-up resistors as shown in the schematic.

# Setup Procedure

Log in to Keil Studio Cloud

Connect your STM32F429-Discovery board to a USB port of your choice. Let Windows try to find the drivers. If the drivers are not found then the st-link usb drivers are missing and you will need assistance from the TAs.
Create new project called “MT2TA4-2024-Lab-03". Go to File -> New -> Mbed Project and select "empty Mbed OS project" from Example projects the dropdown. Name the project and uncheck "Initialize this project as a Git repository".
Set Active project to MT2TA4-2024-Lab-03
Set Build target to DISCO-F429ZI
Set Connected device to DISCO-F429ZI
Visit the BSP driver page and copy the link under the Import into Keil Studio dropdown. Then, in Keil Studio, got to File -> Add MBed Librry to Active Project. Enter the link in the URL field and click next. Select the default branch.
Repeat Step 7 to add the LCD library to your project.
Copy and paste the code from this lab's main.cpp file into main.cpp within your Keil Studio project.
Compile and Run the program using the run button: 


This will compile and download the project to the discovery board.
The demo code provides an example using the internal RTC and displaying a time on the LCD. It also reads and writes some data from/to the 24C64 EEPROM memory. Test your breadboard circuit with the demo code. Using the serial monitor in Keil studio, you should confirm that EEPROM is working as expected, that is, your stored values should persist even if you cycle the power to the Discovery Board and circuit.

# Lab Requirements
Write a program to implement the following behaviour:

Detect the time at which the user button on the Discovery board is pressed. Get the time of the button push from the RTC and store it in the 24FC64F EEPROM. You should be writing a log file in the 24FC64F containing the last 2 times the button is pressed.
When the circuit is idle, display the current time (HH:MM:SS) on the LCD.
When an external button is pressed, display on the LCD the last two times the user button was pressed as recorded in the 24FC64F EEPROM. The user should be able to exit this display mode by pressing the external button again. (Note that pressing this button after cycling the power should still display the last time the user button was pressed since EEPROM is non-volatile)
Use additional external push buttons to set the time/date. Hint: To minimize the number of external push buttons think of using the same strategy as an alarm clock does to set the time and date (i.e., one button to change the value and one button to choose the value to be changed).

All values displayed on the screen should have labels to improve Usability.
All timing must be done with interrupt-driven hardware timers and not with software wait-loops.

# Marking Scheme


10 pts A Finite State Machine diagram for the behaviour outlined in the requirements.

40 pts A functional program that implements all the requirements correctly.

10 pts Motivate your design and implementation decisions to your TA and answer questions about your code.

3 pts main.cpp source file uploaded to your Avenue drop box for lab-03. In the event your project isn't fully functional, this may be used to justify partial marks.

Be prepared to demo the program you wrote to your TA in lab and also to defend your design and implementation decisions.

