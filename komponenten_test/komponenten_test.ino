#include <Servo.h>
#include <Wire.h>
#include "rgb_lcd.h"



// Definieren der Pins und Objekte
rgb_lcd lcd;
const int buttonPin = 8;          //Hier vor ort gucken welche Pins wir belegen müssen
const int ledPin = 4;            //Hier vor ort gucken welche Pins wir belegen müssen 
const int potPin = A0;            //Hier vor ort gucken welche Pins wir belegen müssen
const int tempSensorPin = A1;     //Hier vor ort gucken welche Pins wir belegen müssen
Servo myServo;  

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

void setup() {
    // set up the LCD's number of columns and rows:
    lcd.begin(16, 2, LCD_5x8DOTS, Wire1);

    lcd.createChar(0, degreeSymbol);  // Erstelle das benutzerdefinierte Zeichen

    // Initialisieren der seriellen Kommunikation
    Serial.begin(9600);

    // Konfigurieren der Pins
    pinMode(buttonPin, INPUT_PULLUP);
    pinMode(ledPin, OUTPUT);

    // Initialisieren des Servos
    myServo.attach(3);  // Servo am Pin 9
}

void loop() {
/************** LCD-Display *******************/
    // set the cursor to (0,0):
    lcd.setCursor(0, 0);
    // print from 0 to 9:
    for (int thisChar = 0; thisChar < 10; thisChar++)
    {
        lcd.print(thisChar);
        delay(100);
    }

    // set the cursor to (16,1):
    lcd.setCursor(16,1);
    // set the display to automatically scroll:
    lcd.autoscroll();
    // print from 0 to 9:
    for (int thisChar = 0; thisChar < 10; thisChar++)
    {
        lcd.print(thisChar);
        delay(100);
    }
    // turn off automatic scrolling
    lcd.noAutoscroll();
    lcd.clear();

    int j=0;
    lcd.setCursor(0, 0);
    char red[] = "RED ";
    for (int i = 0; i < 255; i++){
      lcd.setRGB(i, 0, 0);
      delay(5);

      if((i%(254/sizeof(red))) == 0 && j < sizeof(red) - 1){
        lcd.print(red[j]);
        j++;
      }
    }

    //lcd.print("+ BLUE ");
    j = 0;
    char blue[] = "+ BLUE";
    for (int i = 0; i < 255; i++){
      lcd.setRGB(254, 0, i);
      delay(5);

      if((i%(254/sizeof(blue))) == 0 && j < sizeof(blue) - 1){
        lcd.print(blue[j]);
        j++;
      }
    }

    char scroll[] = "RED + BLUE";
    for(int i = 0; i < sizeof(scroll)-1; i++){
      lcd.scrollDisplayLeft();
      delay(100);
    }

    //lcd.print("+ GREEN ");
    j = 0;
    char green[] = "+ GREEN ";
    for (int i = 0; i < 255; i++){
      lcd.setRGB(254, i, 254);
      delay(5);

      if((i%(254/sizeof(green))) == 0 && j < sizeof(green) - 1){
        lcd.print(green[j]);
        j++;
      }
    }
    lcd.print("= WHITE");
    lcd.noAutoscroll();
    delay(500);

    // clear screen for the next loop:
    lcd.clear();
    for(int i = 254; i > 100; i--){
      lcd.setRGB(i, i, i);
      delay(5);
    }

/************** LED *******************/
    digitalWrite(ledPin, HIGH);  
    delay(500);              
    digitalWrite(ledPin, LOW);   
    delay(500);


 /************** Taster *******************/
    if (digitalRead(buttonPin) == HIGH) {  
      Serial.println("Button pressed");
    }


 /************** Potentiometer *******************/
    int potValue = analogRead(potPin);
    // float voltage = potValue * (5.0 / 1023.0);   //zum Probieren was dann angezeigt wird
    Serial.print("Potentiometer Value: ");
    Serial.println(potValue);


 /************** Servo *******************/  
    myServo.write(90);  // Setzt den Servo auf die Mittelposition
    delay(1000);
    myServo.write(15);   // Setzt den Servo auf 0 Grad
    delay(1000);
    myServo.write(165); // Setzt den Servo auf 180 Grad
    delay(1000);


/************** Temperatursensor *******************/

  int rawValue = analogRead(tempSensorPin);
  float R = (1023.0 - rawValue)*10000/rawValue;
  float temperature = 1/(log(R/10000)/3975+1/298.15);
  temperature -= 273.15;  // Umrechnung von Kelvin in Grad Celsius
  Serial.print("Temperatur: ");
  Serial.print(temperature);
  Serial.println(" °C");

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temperatur: ");

  lcd.setCursor(0, 1);
  lcd.print(temperature);
  lcd.setCursor(5, 1);
  lcd.write((unsigned char)0);  // Zeige das benutzerdefinierte Grad-Symbol an
  lcd.print("C");
  
  delay(1000);
  lcd.clear();


}
