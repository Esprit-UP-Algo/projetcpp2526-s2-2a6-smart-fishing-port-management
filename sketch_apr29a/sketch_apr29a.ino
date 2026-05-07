#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>

LiquidCrystal_I2C lcd(0x3F, 16, 2); 

const int pirPin = 4;
const int servoPin = 6;

Servo monServo;
bool mouvementDejaEnvoye = false;

void setup() {
  Serial.begin(9600);
  pinMode(pirPin, INPUT);
  monServo.attach(servoPin);
  monServo.write(0);
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Systeme pret");
}

void loop() {
  int mouvement = digitalRead(pirPin);

  if (mouvement == HIGH && mouvementDejaEnvoye == false) {
    Serial.println("1");
    mouvementDejaEnvoye = true;
  }

  if (mouvement == LOW) {
    mouvementDejaEnvoye = false;
  }

  if (Serial.available() > 0) {
    char data = Serial.read();

    if (data == '1') {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Bien venue");

      monServo.write(90);
      delay(3000);
      monServo.write(0);
      
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Systeme pret");
    }  // ✅ ferme le if (data == '1')
  }    // ✅ ferme le if (Serial.available())
}      // ✅ ferme le loop()