#include <Wire.h>
#include "rgb_lcd.h"
#include <Servo.h>
#include "Ultrasonic.h"

#define interval1 200
#define interval2 800

unsigned long time1 = 0;
unsigned long time2 = 0;

rgb_lcd lcd;


Ultrasonic ultrasonic(8);

int stage = 0;

const int colorR = 255;
const int colorG = 255;
const int colorB = 255;

int sensorVal = 0;

Servo servo;
int servoPin = 7;
int servoAngle = 0;
int servoDelay = 15;

int buttonState1 = 0;
int lastState1 = HIGH; 
int currentState1;

int buttonState2 = 0;
int lastState2 = HIGH; 
int currentState2;

int loadingCounter = 0;
String loading[4] = {"", ".", "..", "..."};

int Li=16;
int Lii=0;
String Scroll_LCD_Left(String StrDisplay) {
   String result;
   String StrProcess = "                " + StrDisplay + "                ";
   result = StrProcess.substring(Li,Lii);
   Li++;
   Lii++;
   if (Li>StrProcess.length()) {
     Li=16;
     Lii=0;
   }
   return result;
}

void buttonPress(int stage1, int stage2) {
    if (lastState1 == HIGH && currentState1 == LOW) {
        lcd.clear();
        stage = stage1;
    } else if (lastState2 == HIGH && currentState2 == LOW) {
        lcd.clear();
        stage = stage2;
    }
}

void setup() {
    lcd.begin(16, 2);

    pinMode(2, INPUT_PULLUP);
    pinMode(4, INPUT_PULLUP);

    Serial.begin(9600);

    lcd.setRGB(colorR, colorG, colorB);

    servo.attach(servoPin);
    servo.write(90);

    delay(1000);
}

void loop() {
  long RangeInCentimeters;
  currentState1 = digitalRead(2);
  currentState2 = digitalRead(4);
  switch (stage) {
    case 0:   //initial stage
      if (millis() >= time1 + interval1) {
        time1 = millis();
        lcd.setCursor(0, 0);
        lcd.print(Scroll_LCD_Left("Do you have a ticket?"));
      }
      lcd.setCursor(0, 1);
      lcd.print("   Yes     No   ");
      buttonPress(1,2);
      break;
    case 1:   //scanning
      lcd.setCursor(0, 0);
      lcd.print("Please scan it");
      lcd.setCursor(0, 1);
      buttonPress(10,0);
      break;
    case 2:   //purchase confirmation
      if (millis() >= time1 + interval1) {
        time1 = millis();
        lcd.setCursor(0, 0);
        lcd.print(Scroll_LCD_Left("Buy a ticket? 1 day - 15eur"));
      }
      lcd.setCursor(0, 1);
      lcd.print("   Yes     No   ");
      buttonPress(10,0);
      break;
    case 3:   //opening gate
      lcd.setCursor(0, 0);
      lcd.write("Please wait");
      for(servoAngle = 90; servoAngle > 0; servoAngle--) {
        servo.write(servoAngle);
        delay(servoDelay);
      }
      lcd.setCursor(0, 0);
      lcd.write("Gate is open");
      delay(5000);
      stage = 4;
      break;
    case 4:   //closing gate
      RangeInCentimeters = ultrasonic.MeasureInCentimeters();
      if (RangeInCentimeters >= 10) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.write("Closing");
        for(servoAngle = 0; servoAngle < 90; servoAngle++) {
          servo.write(servoAngle);
          delay(servoDelay);
        }
        stage = 0;
      }
      break;
    case 10:    //veryfing purchase
      if (millis() >= time2 + interval2) {
        time2 = millis();
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Verifying");
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
  lastState1 = currentState1;
  lastState2 = currentState2;
}