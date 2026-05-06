 #include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

const int pirPin = 4;
const int servoPin = 6;

Servo monServo;
bool mouvementDejaEnvoye = false;

void setup() {
  Serial.begin(9600);
  pinMode(pirPin, INPUT);
  monServo.attach(servoPin);
  monServo.write(0); // Position initiale
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
    char data = Serial.read(); // ✅ Syntaxe corrigée

    if (data == '1') {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Bien venue");

      monServo.write(90);       // ✅ Ouvrir à 90°
      delay(3000);              // ✅ Attendre 3 secondes
      monServo.write(0);        // ✅ Retourner à 0°

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Systeme pret");
    }
    else if (data == '0') {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Complet");
      monServo.write(0);        // ✅ S'assurer que le servo est à 0°
    }
  }
}
