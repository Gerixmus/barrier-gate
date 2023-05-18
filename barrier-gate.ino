#include <Wire.h>
#include "rgb_lcd.h"
#include <Servo.h>
#include "Ultrasonic.h"

#define interval1 200
#define interval2 800

unsigned long time = 0;

Ultrasonic ultrasonic(8);

int stage = 0;

//LCD config (background color for LCD)
rgb_lcd lcd;
const int colorR = 255;
const int colorG = 255;
const int colorB = 255;

//servo config
Servo servo;
int servoPin = 7;
int servoAngle = 90;
int servoDelay = 15;

//button1 config
int buttonState1 = 0;
int lastState1 = HIGH; 
int currentState1;

//button2 config
int buttonState2 = 0;
int lastState2 = HIGH; 
int currentState2;

//code for loading screens
int loadingCounter = 0;
String loading[4] = {"", ".", "..", "..."};

// function for scrolling only one line on LCD
// https://www.youtube.com/watch?v=tbGuXnqwYWU&ab_channel=UtehStr
int position=16;
int lenght=0;
String Scroll_LCD_Left(String StrDisplay) {
   String result;
   String StrProcess = "                " + StrDisplay + "                ";
   result = StrProcess.substring(position,lenght);
   position++;
   lenght++;
   if (position>StrProcess.length()) {
     position=16;
     lenght=0;
   }
   return result;
}

// function to select next stage
// button 1 - stage1, button 2 - stage2
void buttonPress(int stage1, int stage2) {
    if (lastState1 == HIGH && currentState1 == LOW) {
        lcd.clear();
        stage = stage1;
    } else if (lastState2 == HIGH && currentState2 == LOW) {
        lcd.clear();
        stage = stage2;
    }
}

/* function to print message on display
pos - position, row - row, msg - message, scroll - 0 is static and 1 is scrolling */
void printAtCursor(int pos, int row, String msg, int scroll = 0) {
  lcd.setCursor(pos, row);
  if (scroll == 1) {
    lcd.print(Scroll_LCD_Left(msg));
  } else {
    lcd.print(msg);
  }
}

void setup() {
    lcd.begin(16, 2);
    pinMode(2, INPUT_PULLUP);
    pinMode(4, INPUT_PULLUP);
    lcd.setRGB(colorR, colorG, colorB);
    servo.attach(servoPin);
    servo.write(servoAngle);
    delay(1000);
}

void loop() {
  // saving current state of buttons
  currentState1 = digitalRead(2);
  currentState2 = digitalRead(4);
  switch (stage) {
    case 0:   //initial stage
      servo.detach();
      if (millis() >= time + interval1) {
        time = millis();
        printAtCursor(0, 0, "Do you have a ticket?", 1);
      }
      printAtCursor(0, 1, "   Yes     No   ");
      buttonPress(1,2);
      break;
    case 1:   //scanning
      printAtCursor(0, 0, "Please scan it");
      buttonPress(10,0);
      break;
    case 2:   //purchase confirmation
      if (millis() >= time + interval1) {
        time = millis();
        printAtCursor(0, 0, "Buy a ticket? 1 day - 15eur", 1);
      }
      printAtCursor(0, 1, "   Yes     No   ");
      buttonPress(10,0);
      break;
    case 3:   //opening gate
      servo.attach(servoPin);
      printAtCursor(0, 0, "Please wait");
      for(servoAngle; servoAngle > 0; servoAngle--) {
        servo.write(servoAngle);
        delay(servoDelay);
      }
      printAtCursor(0, 0, "Gate is open");
      delay(5000);
      stage = 4;
      break;
    case 4:   //closing gate
      if (ultrasonic.MeasureInCentimeters() >= 10) {
        lcd.clear();
        printAtCursor(0, 0, "Closing");
        for(servoAngle; servoAngle < 90; servoAngle++) {
          servo.write(servoAngle);
          delay(servoDelay);
        }
        stage = 0;
      }
      break;
    case 10:    //veryfing purchase
      if (millis() >= time + interval2) {
        time = millis();
        lcd.clear();
        printAtCursor(0, 0, "Verifying");
        lcd.print(loading[loadingCounter]);
        if (loadingCounter < 3) {
          loadingCounter++;
        } else {
          loadingCounter = 0;
        }
      }
      buttonPress(3,0);
      break;
  }
  // saving last state for button debounce
  lastState1 = currentState1;
  lastState2 = currentState2;
}