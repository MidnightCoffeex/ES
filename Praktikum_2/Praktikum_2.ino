#include <math.h>
#include <Servo.h>
#include <Wire.h>
#include "rgb_lcd.h"


#define DEBUG
// green board
#if defined(ARDUINO_ARCH_SAM)
#define MAX_VOLTAGE 3300
#define WireMaster Wire1
#define SETRES(x) analogReadResolution(x)
// red board
#elif defined(ARDUINO_ARCH_AVR)
#define ADC_RESOLUTION 10
#define MAX_VOLTAGE 5000
#define WireMaster Wire
#define SETRES(x) while(false){}
// untested
#else
#error "Wrong Hardware"
#endif
const unsigned long ADCMax = (1 << ADC_RESOLUTION) - 1;

//definieren der Pins
const int ButtonPin = 2;
const int PotPin = A0;
Servo myServo;
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


int delay_time = 0;
bool ButtonPinpressed = false;
int minute = 10000;
int millis_merker = 0;

void handleInterrupt() {
  if (ButtonPinpressed == false) {
    ButtonPinpressed = true;
    millis_merker = millis();
  }
  Serial.println("Button pressed");
}

void setup() {
  Serial.begin(115200);

  pinMode(13, OUTPUT);
  pinMode(ButtonPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(ButtonPin), handleInterrupt, RISING);

  myServo.attach(3);

  lcd.begin(2, 16, LCD_5x8DOTS, WireMaster);
  lcd.setCursor(0, 0);
  lcd.print("Temperature:");
  lcd.createChar(0, degreeSymbol);
}


void loop() {

  Serial.print("Button state: ");
  Serial.println(digitalRead(ButtonPin));
  
  Timing();
  Servo();
  Temperature();

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
  if (ButtonPinpressed == false) {
    delay_time = 1500;
  }

  else if (ButtonPinpressed == true) {
    delay_time = 250;
  }

  if ((millis() - millis_merker) > minute) {
    ButtonPinpressed = false;
  }

  digitalWrite(13, HIGH);
  delay(delay_time);

  digitalWrite(13, LOW);
  delay(delay_time);
}

void Servo() {
  int ValuePotentiometer = analogRead(PotPin);
  int servoDegree = map(ValuePotentiometer, 0, 1023, 1, 179);
  myServo.write(servoDegree);
}

void Temperature() {
  //https://wiki.seeedstudio.com/Grove-Temperature_Sensor_V1.2/
  float R0 = 100000.0;
  float B = 4250.0;

  int rawValue = analogRead(TempPin);
  if (MAX_VOLTAGE == 5){
    rawValue =(float) (rawValue/5) * 3.3;
  }
  float resistance = (float)(1023.0 - rawValue) * R0 / rawValue;
  float temperature = 1 / (log(resistance / R0) / B + 1 / 298.15);
  temperature = temperature - 273.15;  // Umrechnung von Kelvin in Grad Celsius

  //Ausgabe auf dem LCD Display
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temperature: ");
  lcd.setCursor(0, 1);
  lcd.print(temperature);
  Serial.println(temperature);
  lcd.write((unsigned char) 0);
  lcd.print("C");
}

