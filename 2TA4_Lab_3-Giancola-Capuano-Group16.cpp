// MECHTRON 2TA4: LAB 3 //
//   Michael Giancola  //
//  Gianluca Capauno  //
#include "LCD_DISCO_F429ZI.h"
#include "mbed.h"
#include "stm32f429i_discovery_lcd.h"
#include <cstdint>
#include <time.h>
#include "DebouncedInterrupt.h"

// Defining size of screen
#define LCD_WIDTH 240
#define LCD_HEIGHT 320

// Defining EEPROM Pins
#define SDA_PIN PC_9
#define SCL_PIN PA_8
#define EEPROM_ADDR 0xA0

// Configuring Hardware
LCD_DISCO_F429ZI LCD;
I2C i2c(SDA_PIN, SCL_PIN);
InterruptIn userButton(BUTTON1);
DebouncedInterrupt externalButton1(PA_5);
DebouncedInterrupt externalButton2(PA_6);
DebouncedInterrupt externalButton3(PA_7);

// Declaring Variables
int display = 0;
int eprom = 0;
int powerUp = 1; 
bool increment = false;
unsigned int addr1 = 0;
unsigned int addr2 = 100;
int setTime = 0;
char first_time[20] = "none1";
char second_time[20] = "none2";

// Write EEPROM Function
void WriteEEPROM(int address, unsigned int eeaddress, char *data, int size) {
  char i2cBuffer[size + 2];
  i2cBuffer[0] = (unsigned char)(eeaddress >> 8);   // MSB
  i2cBuffer[1] = (unsigned char)(eeaddress & 0xFF); // LSB

  for (int i = 0; i < size; i++) {
    i2cBuffer[i + 2] = data[i];
  }

  int result = i2c.write(address, i2cBuffer, size + 2, false);
  thread_sleep_for(6);
}

// Read EEPROM Function
void ReadEEPROM(int address, unsigned int eeaddress, char *data, int size) {
  char i2cBuffer[2];
  i2cBuffer[0] = (unsigned char)(eeaddress >> 8);   // MSB
  i2cBuffer[1] = (unsigned char)(eeaddress & 0xFF); // LSB

  // Reset eeprom pointer address
  int result = i2c.write(address, i2cBuffer, 2, false);
  thread_sleep_for(6);

  // Read eeprom
  i2c.read(address, data, size);
  thread_sleep_for(6);
}

// ISR for updating EEPROM state -> for reading times to EEPROM
void UserButton(){
    eprom = 1;
}

// ISR for leaving powerUp state and updates display variable -> displays times
void DisplayButton1(){
    powerUp = 0;
    if(display==0){
        display = 1;
    }
    else{
    display = 0;
    }
    }

// Cycles time setting between hour,min,sec
void settingTime(){
    setTime++;
    LCD.Clear(LCD_COLOR_WHITE);
    }
// Changes value of hour, min, sec
void changeTime(){
    increment = true;
    }

int main() {
    //Set interupts
    userButton.fall(&UserButton);
    externalButton1.attach(&DisplayButton1,IRQ_FALL,100,false);
    externalButton2.attach(&settingTime,IRQ_FALL,100,false);
    externalButton3.attach(&changeTime,IRQ_FALL,100,false);

//Set initial time to 0:0:0
    struct tm t;
    t.tm_year = 124; 
    t.tm_mon = 0;
    t.tm_mday = 0;
    t.tm_hour = 0;
    t.tm_min = 0;
    t.tm_sec = 0;
    set_time(mktime(&t));

    //Main Loop
    while (1) {
        // Update the time every second if the program is idle
        time_t rawtime;
        time(&rawtime);
        char timebuf[20];
        strftime(timebuf, 200, "%H:%M:%S", localtime(&rawtime));
        LCD.Clear(LCD_COLOR_WHITE);
        LCD.SetFont(&Font24);
        LCD.SetBackColor(LCD_COLOR_ORANGE);  
        LCD.SetTextColor(LCD_COLOR_BLACK);
        LCD.DisplayStringAt(0, 40, (uint8_t *)"HH:MM:SS", CENTER_MODE);
        LCD.DisplayStringAt(0, 80, (uint8_t *)&timebuf, CENTER_MODE);
        
        // Enters in this state -> reads most recent times from the EEPROM upon repowering
        if(powerUp ==1){
        ReadEEPROM(EEPROM_ADDR, addr1, first_time, 20);
        ReadEEPROM(EEPROM_ADDR, addr2, second_time, 20);
        }

        // Enters on User Button Push
        if(eprom == 1){
        LCD.DisplayStringAt(0, 10, (uint8_t *)"Reading...", CENTER_MODE);
        // Reads EEPROM data from first address then shifts to second
        ReadEEPROM(EEPROM_ADDR, addr1, second_time, 20);
        WriteEEPROM(EEPROM_ADDR, addr2, second_time, 20);
        // Writes new time to EEPROM
        WriteEEPROM(EEPROM_ADDR, addr1, timebuf, 20);
        ReadEEPROM(EEPROM_ADDR, addr1, first_time, 20);
        thread_sleep_for(1000);
        eprom = 0;
        }

        // Enters on External Button Push
        if(display == 1){
            //Displays times stored in EEPROM
            LCD.DisplayStringAt(0, 20, (uint8_t *)"display", CENTER_MODE);
            LCD.Clear(LCD_COLOR_WHITE);
            LCD.DisplayStringAt(0, 50, (uint8_t *)"Most Recent", CENTER_MODE);
            LCD.DisplayStringAt(0, 80, (uint8_t *)&first_time, CENTER_MODE);
            LCD.DisplayStringAt(0, 140, (uint8_t *)"2nd Most", CENTER_MODE);
            LCD.DisplayStringAt(0, 170, (uint8_t *)"Recent", CENTER_MODE);
            LCD.DisplayStringAt(0, 200, (uint8_t *)&second_time, CENTER_MODE);
        }

        // Checking if set time is in the hours position, if so, begin time-setting procedure
        if(setTime % 4 == 1){
        // Clear the LCD screen and initialize variables
            LCD.Clear(LCD_COLOR_WHITE);
            time_t rawtime;
            time(&rawtime);
            t = *localtime(&rawtime);
            char hrs[4];            
            char mins[4];
            char secs[4];

            // Continue time-setting procedure until hours position is no longer active
        while((setTime % 4) != 0){
            // If hours, minutes, or seconds is active and an increment interrupt has been sent,
            // increase the corresponding value based on the Set_Time variable
            if(setTime % 4 == 1 && increment){
                t.tm_hour++;              
            // Check if hours have exceeded 12, reset to 0 and clear the LCD screen
                if(t.tm_hour > 12){
                    t.tm_hour = 0;
                    LCD.Clear(LCD_COLOR_WHITE);
                }
                increment = !increment;  // Toggle increment to avoid continuous increase
            }

            if(setTime % 4 == 2 && increment){
                t.tm_min++;
                // Check if minutes have exceeded 60, reset to 0 and clear the LCD screen
                if(t.tm_min > 60){
                  t.tm_min = 0;
                        LCD.Clear(LCD_COLOR_WHITE);
                }
                increment = !increment;  // Toggle increment to avoid continuous increase
            }

            if(setTime % 4 == 3 && increment){
                t.tm_sec++;
                // Check if seconds have exceeded 60, reset to 0 and clear the LCD screen
                if(t.tm_sec > 60){
                    t.tm_sec = 0;
                    LCD.Clear(LCD_COLOR_WHITE);
                }
                increment = !increment;  // Toggle increment to avoid continuous increase
            }

            // Reprint screen with updates
            sprintf(hrs, "%d", t.tm_hour);
            sprintf(mins, "%d", t.tm_min);
            sprintf(secs, "%d", t.tm_sec);
            LCD.DisplayStringAt(0, 40, (uint8_t *)"Hours", CENTER_MODE);
            LCD.DisplayStringAt(0, 70, (uint8_t *)&hrs, CENTER_MODE);
            LCD.DisplayStringAt(0, 130, (uint8_t *)"Minutes", CENTER_MODE);
            LCD.DisplayStringAt(0, 160, (uint8_t *)&mins, CENTER_MODE);
            LCD.DisplayStringAt(0, 220, (uint8_t *)"Seconds", CENTER_MODE);
            LCD.DisplayStringAt(0, 250, (uint8_t *)&secs, CENTER_MODE);
            LCD.DisplayStringAt(0, 10 + 90 * (setTime % 4), (uint8_t *)"*", CENTER_MODE);

            // Save new time set to the internal time
            set_time(mktime(&t));
            thread_sleep_for(50);  // Introduce a delay to control the loop frequency
                }
            }
            thread_sleep_for(1000); 
        }
    }
    
