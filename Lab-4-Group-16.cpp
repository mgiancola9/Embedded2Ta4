// Lab 4: Fan Control and Temperature Sensor
// Michael Giancola and Gianluca Capuano

#include "mbed.h"
#include "LCD_DISCO_F429ZI.h"
#include "TS_DISCO_F429ZI.h"
#include <cstdint>
#include <limits>
#include <time.h>
#include "DebouncedInterrupt.h"
#include <cmath>

#define LCD_WIDTH 240
#define LCD_HEIGHT 320

//Initiating Variables and Objects
LCD_DISCO_F429ZI LCD;
TS_DISCO_F429ZI TS;
TS_StateTypeDef tsState;

uint16_t tsX, tsY;
AnalogIn aIn(PA_0);
PwmOut fan(PD_14);

float volt;
float sensorTemp;
float threshTemp;
int counter;

//function for display of LCD screen
void display_LCD(){
  LCD.SetFont(&Font24);
  LCD.Clear(LCD_COLOR_WHITE);
  LCD.SetTextColor(LCD_COLOR_DARKBLUE);
  uint8_t text1[30];
  uint8_t text2[30];
  sprintf((char *)text1, "Temp:%.2f", sensorTemp); //display sensor temperature
  sprintf((char *)text2, "Thresh:%.2f", threshTemp); //display threshold temperature
  LCD.DisplayStringAt(0, 40, (uint8_t *)&text1, CENTER_MODE);
  LCD.DisplayStringAt(0, 80, (uint8_t *)&text2, CENTER_MODE);
  LCD.FillRect(155, 280, 50, 10); //display minus sign
  LCD.FillRect(35,280,50,10); //display plus sign
  LCD.FillRect(55,260, 10,50);
  }

void set_Threshold(){
    TS.GetState(&tsState);
    if (tsState.TouchDetected) {
        tsX = tsState.X;
        tsY = 320 - tsState.Y;
      if ((tsX<120 && tsX>0) && (tsY>260 && tsY<320)) { //border for plus sign
        LCD.SetTextColor(LCD_COLOR_RED); //shows plus was pressed
        LCD.FillRect(0,260,120,60);
        threshTemp= 0.5 + threshTemp; //increment threshold
        thread_sleep_for(500); //sleep prevents spamming
      }
      if ((tsX>120 && tsX<240) && (tsY>260 && tsY<320)) { //border for minus sign 
        LCD.SetTextColor(LCD_COLOR_RED); //shows minus was pressed
        LCD.FillRect(120,260,120,60);
        threshTemp = threshTemp-0.5; //decrement threshold
        thread_sleep_for(500); //sleep prevents spamming
      }
    }
}

void fan_Control() {
    static float dutyCycle = 0.0f; // PWM duty cycle starts at 0 (fan off)
    if (sensorTemp > threshTemp) {
        // Turns fan on and gradually increases speed if below max speed
        if (dutyCycle < 1.0f) {
            dutyCycle += 0.01f; // Increase speed gradually
            thread_sleep_for(250);
            if (dutyCycle > 1.0f) {
                dutyCycle = 1.0f; // Ensure duty cycle does not exceed 1.0
            }
        }
        fan.write(dutyCycle); // Update fan speed
    } else {
        // Turns fan off and resets duty cycle for next time
        dutyCycle = 0.0f; // Reset duty cycle to turn fan off
        fan.write(dutyCycle); // Turn fan off
    }
}

int main() {
  aIn.set_reference_voltage(3);
  volt = aIn.read_voltage();
  sensorTemp = 3*volt*100/(1024*10e-3);
  threshTemp = sensorTemp + 1;
  counter = 0;
  while (true) {
    display_LCD();
    set_Threshold();
    fan_Control();
    //this loop ensures the temperature updates every one second
    if (counter == 20){
        aIn.set_reference_voltage(3);
        volt = aIn.read_voltage();
        sensorTemp = 3*volt*100/(1024*10e-3);
        counter = 0;
    }
    ++counter; 
    thread_sleep_for(50);
    }
}
