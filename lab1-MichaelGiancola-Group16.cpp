#include "mbed.h"

//Initialize interrupt for user button on the pin Button1
InterruptIn userButton(BUTTON1);

//Initialize DigitalOut objects for LED's on the pins PG_13 and PG_14
DigitalOut led3(PG_13);
DigitalOut led4(PG_14);

//Initialize the Ticker objects for timers
Ticker phaseTicker;   // Timer for switching between phases
Ticker led4Ticker;    // Timer for blinking LED4
Ticker led3Ticker; 

//State Variables
int state = 0; // default state on startup
int activeLed = 3; // Start with LED 3

//Function to set the state based on button press
void SetState() {
    if (state != 1) {
        state = 1;
    } else {
        state = 2;
    }
}

//Function toggles the state of LED4
void BlinkLed4() {
    led4 = !led4;  // Toggle the state of LED4
}

//Function toggles the state of LED3
void BlinkLed3(){
    led3 = !led4;
}

//Function to switch between phases
void SwitchPhase() {
    // Switch between phases
    if (state == 0) {
        led4Ticker.attach(&BlinkLed4, 1000ms);  // Turn off LED4 timer in phase 0
    } else if (state == 1) {
        led4Ticker.attach(&BlinkLed4, 1000ms);  // Blink LED4 every second in phase 1
        
        led3Ticker.attach(&BlinkLed3, 1000ms); 
        
    } else {
        
        if (activeLed == 3) {
            led3Ticker.attach(&BlinkLed3, 1000ms);
            led4Ticker.detach(); // Detach LED4 ticker
            led4=0;
            activeLed = 4; // Update the active LED
        } else if (activeLed == 4) {
            led4Ticker.attach(&BlinkLed4, 1000ms);
            led3Ticker.detach(); // Detach LED3 ticker
            led3=0;
            activeLed = 3; // Update the active LED
        }
}
}

int main() {
    //Attach the SetState function to be called on falling edge of user button 
    userButton.fall(&SetState);

    //Attach SwitchPhase function to be called every 2 seconds
    phaseTicker.attach(&SwitchPhase, 2000ms);  // Switch phases every 2 seconds

    __enable_irq(); //Enables interrupts

    while (true) {
        // The while loop is now empty
        // All the work is done in the interrupt handlers
    }
}