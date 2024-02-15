/*
 * Lab 2
 * Michael Giancola (400372137)
 * Gianluca Capuano (400374041)
 */

// Importing libraries and header files
#include "mbed.h"
#include "LCD_DISCO_F429ZI.h"
#include "DebouncedInterrupt.h"
#include "stdlib.h"
#include <iostream>

// Objects for hardware components
LCD_DISCO_F429ZI LCD;
DebouncedInterrupt button(PA_6);
InterruptIn userButton(BUTTON1);
DigitalOut led3(PG_13);
Ticker greenTicker;
Timeout timeoutDelay;
Timer myTimer;

// Variables for reaction times and states
int latest = std::numeric_limits<int>::max();
int best = std::numeric_limits<int>::max();
int reactionState = 0;
bool reaction = false;
bool ifCheat = false;

// Function to update LCD display with reaction time information
void update_lcd() {
  LCD.Clear(LCD_COLOR_WHITE);
  uint8_t text[30];
  LCD.DisplayStringAt(0, 40, (uint8_t *)"Reaction Times", CENTER_MODE);

  // Display latest reaction time
  if (latest == std::numeric_limits<int>::max()) {
    sprintf((char *)text, "Latest: %s", "ms");
  } else {
    sprintf((char *)text, "Latest:%d%s", latest, "ms");
  }
  LCD.DisplayStringAt(0, 80, (uint8_t *)&text, LEFT_MODE);

  // Display best reaction time
  if (best == std::numeric_limits<int>::max()) {
    sprintf((char *)text, "Best: %s", "ms");
  } else {
    sprintf((char *)text, "Best:%d%s", best, "ms");
  }

  LCD.DisplayStringAt(0, 120, (uint8_t *)&text, LEFT_MODE);
}

// Function to start the reaction time measurement timer
void startTimer() {
    myTimer.start();
}

// Interrupt Service Routine for a green LED
void greenLed() {
    led3 = !led3; 
} 

// Function to get the elapsed time from the timer in milliseconds
int getTime() {
    myTimer.stop();
    auto elapsed = myTimer.elapsed_time();
    int milliseconds = chrono::duration_cast<chrono::milliseconds>(elapsed).count();
    return milliseconds;
}

// Function to reset signals and update LCD after a reaction or cheat
void resetSignal(){
    led3 = 0;
    ifCheat = false;
    reaction = false;
    myTimer.reset();
    greenTicker.attach(&greenLed, 100ms);
    update_lcd();
}

// Interrupt Service Routine for a reaction time trigger
void reactionTime() {
    led3 = 1;
    startTimer();
    reaction = true;
    ifCheat = false;
}

// Function to handle button presses on the board
void onBoardButton() {
    if (ifCheat) {
        timeoutDelay.detach();
        resetSignal();
        return;
    }

    if (reactionState == 1){
        led3 = 0;
        int randomTime = (rand() % 5 + 1 ) * 1000;
        std::chrono::milliseconds duration(randomTime); 
        greenTicker.detach();
        ifCheat = true;
        timeoutDelay.attach(&reactionTime, duration);
    }

    if (reaction == true) {
        int elapsed_time = getTime();
        if (elapsed_time > 0) { 
            latest = elapsed_time; 
        }
        if (latest < best) { 
            best = latest; 
        }
        timeoutDelay.detach();
        resetSignal();
    }
}

void resetLCD(){
  LCD.Clear(LCD_COLOR_WHITE);
  uint8_t text[30];
  LCD.DisplayStringAt(0, 40, (uint8_t *)"Click the blue", CENTER_MODE);
  LCD.DisplayStringAt(0, 80, (uint8_t *)"button to", CENTER_MODE);
  LCD.DisplayStringAt(0, 120, (uint8_t *)"play!!!", CENTER_MODE);
}

void startScreen(){
  LCD.Clear(LCD_COLOR_WHITE);
  uint8_t text[30];
  LCD.DisplayStringAt(0, 40, (uint8_t *)"Click the", CENTER_MODE);
  LCD.DisplayStringAt(0, 80, (uint8_t *)"External", CENTER_MODE);
  LCD.DisplayStringAt(0, 120, (uint8_t *)"Button", CENTER_MODE);
  LCD.DisplayStringAt(0, 160, (uint8_t *)"to Start", CENTER_MODE);
}

// Interrupt Service Routine for an external button
void externalButton() { 
    latest = std::numeric_limits<int>::max();
    best = std::numeric_limits<int>::max();
    resetSignal();
    resetLCD();
    reactionState = 1;
}

// Main function
int main() {
    
  LCD.SetFont(&Font24);
  LCD.SetTextColor(LCD_COLOR_DARKBLUE);
  startScreen();
  // Attach interrupts and set up LCD
  button.attach(&externalButton, IRQ_FALL, 100, false);
  userButton.fall(&onBoardButton);
  __enable_irq();
  // Main loop
  while (true) {
  }
}
