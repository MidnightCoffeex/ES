#include <math.h>
#include <Servo.h>
#include <Wire.h>
#include "rgb_lcd.h"
#include <TaskScheduler.h>

#define DEBUG
// green board
#if defined(ARDUINO_ARCH_SAM)
//#define MAX_VOLTAGE 3300
#define WireMaster Wire1
#define SETRES(x) analogReadResolution(x)
// red board
#elif defined(ARDUINO_ARCH_AVR)
#define ADC_RESOLUTION 10
//#define MAX_VOLTAGE 5000
#define WireMaster Wire
#define SETRES(x) \
  while (false) {}
// untested
#else
#error "Wrong Hardware"
#endif


const unsigned long ADCMax = (1 << ADC_RESOLUTION) - 1;
const int MAX_VOLTAGE = 5;
//definieren der Pins
const int ButtonPin = 8;
const int PotPin = A0;
Servo myServo;

bool LED_STATUS = HIGH;
const int TempPin = A1;

rgb_lcd lcd;
// Grad-Zeichen erstellen
byte degreeSymbol[8] = {
  0b00110,
  0b01001,
  0b01001,
  0b00110,
  0b00000,
  0b00000,
  0b00000,
  0b00000
};

void Timing();
void Servo();
void Temperature();
void Scroll();

Task task1(1500, TASK_FOREVER, &Timing);
Task task2(20, TASK_FOREVER, &Servo);
Task task3(2000, TASK_FOREVER, &Temperature);
Task task4(200, TASK_FOREVER, &Scroll);

Scheduler runner;

bool BUTTON_STATUS = false;
void handleInterrupt() {
  BUTTON_STATUS = !BUTTON_STATUS;
}

void setup() {
  Serial.begin(115200);

  pinMode(13, OUTPUT);
  pinMode(ButtonPin, INPUT);

  myServo.attach(3);

  lcd.begin(2, 16, LCD_5x8DOTS, WireMaster);
  lcd.setCursor(0, 0);
  lcd.print("Temperature:");
  lcd.createChar(0, degreeSymbol);

  runner.init();

  runner.addTask(task1);
  runner.addTask(task2);
  runner.addTask(task3);
  runner.addTask(task4);

  task1.enable();
  task2.enable();
  task3.enable();
  task4.enable();

  attachInterrupt(digitalPinToInterrupt(ButtonPin), handleInterrupt, RISING);
}


void loop() {

  runner.execute();

  Serial.print(millis());
  Serial.print(" --> in Sekunden = ");
  float timer = millis();
  Serial.print(timer / 1000);
  Serial.println("s");
}

/************************************
*             Funktionen            *
************************************/

void Timing() {
  digitalWrite(13, LED_STATUS);
  LED_STATUS = !LED_STATUS;
}

void Servo() {
  if (BUTTON_STATUS == HIGH) {
    int ValuePotentiometer = analogRead(PotPin);
    int servoDegree = map(ValuePotentiometer, 0, 1023, 1, 179);
    myServo.write(servoDegree);
  }
}

void Temperature() {
  if (BUTTON_STATUS == HIGH) {
    //https://wiki.seeedstudio.com/Grove-Temperature_Sensor_V1.2/
    float R0 = 100000.0;
    float B = 4250.0;

    int rawValue = analogRead(TempPin);
    if (MAX_VOLTAGE == 5) {
      rawValue = (float)(rawValue / 5) * 3.3;
    }
    float resistance = (float)(1023.0 - rawValue) * R0 / rawValue;
    float temperature = 1 / (log(resistance / R0) / B + 1 / 298.15);
    temperature = temperature - 273.15;  // Umrechnung von Kelvin in Grad Celsius

    //Ausgabe auf dem LCD Display
    lcd.setCursor(16, 0);
    lcd.print("Temperature: ");
    if (temperature > 25.0) {
      lcd.setRGB(255, 165, 0);
    }
    if (temperature < 25.0) {
      lcd.setColorWhite();
    }
    lcd.print(temperature);
    Serial.println(temperature);
    lcd.write((unsigned char)0);
    lcd.print("C");
  } else {
    lcd.clear();
    lcd.setRGB(0, 0, 0);
  }
}

void Scroll(){
  lcd.scrollDisplayLeft();
}
