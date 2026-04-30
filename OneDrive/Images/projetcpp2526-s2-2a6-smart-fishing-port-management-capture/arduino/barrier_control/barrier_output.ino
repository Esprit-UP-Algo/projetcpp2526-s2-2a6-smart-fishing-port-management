// Sketch Output: contrôle barrière via Servo (UNO)
// SERVO -> D11, LED statut -> D13
// Reçoit commandes série: OPEN, CLOSE, PING

#include <Servo.h>

const int SERVO_PIN = 11;
const int STATUS_LED = 13;
Servo barrierServo;

void setup() {
  Serial.begin(9600);
  barrierServo.attach(SERVO_PIN);
  barrierServo.write(0); // position fermée
  pinMode(STATUS_LED, OUTPUT);
  digitalWrite(STATUS_LED, LOW);
  delay(100);
  Serial.println("OUTPUT_SKETCH_READY");
}

void loop() {
  if (Serial.available()) {
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();
    cmd.toUpperCase();

    if (cmd == "OPEN") {
      barrierServo.write(90); // ouvrir
      digitalWrite(STATUS_LED, HIGH);
      Serial.println("BARRIER:OPEN");
    } else if (cmd == "CLOSE") {
      barrierServo.write(0); // fermer
      digitalWrite(STATUS_LED, LOW);
      Serial.println("BARRIER:CLOSED");
    } else if (cmd == "PING") {
      Serial.println("PONG");
    } else {
      Serial.print("UNKNOWN:");
      Serial.println(cmd);
    }
  }
}
