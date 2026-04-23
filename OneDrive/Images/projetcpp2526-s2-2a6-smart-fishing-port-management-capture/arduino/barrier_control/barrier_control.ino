

#include <Servo.h>
#include <NewPing.h>

#define TRIG_PIN 9
#define ECHO_PIN 10
#define MAX_DISTANCE 200  
#define DETECT_THRESHOLD 50  
#define SERVO_PIN 11
#define STATUS_LED 13

NewPing sonar(TRIG_PIN, ECHO_PIN, MAX_DISTANCE);
Servo barrierServo;

bool barrierOpen = false;
unsigned long lastPing = 0;
const unsigned long PING_INTERVAL = 5000;  
bool shipDetected = false;

void setup() {
  Serial.begin(9600);
  pinMode(STATUS_LED, OUTPUT);
  barrierServo.attach(SERVO_PIN);
  barrierServo.write(0);  
  digitalWrite(STATUS_LED, LOW);
  
  Serial.println("ARDUINO_READY");  
  delay(1000);
}

void loop() {

  if (millis() - lastPing > PING_INTERVAL) {
    if (Serial.available()) {
      handleSerial();
    }
    lastPing = millis();
  }

  
  delay(50);
  unsigned int distance = sonar.ping_cm();
  
  if (distance > 0 && distance < DETECT_THRESHOLD && !shipDetected) {
    shipDetected = true;
    Serial.println("SHIP_DETECTED");
    digitalWrite(STATUS_LED, HIGH);
  } else if (distance >= DETECT_THRESHOLD || distance == 0) {
    shipDetected = false;
    digitalWrite(STATUS_LED, LOW);
  }

  if (Serial.available()) {
    handleSerial();
  }
}

void handleSerial() {
  String cmd = Serial.readStringUntil('\n');
  cmd.trim().toUpperCase();
  
  if (cmd == "PING") {
    Serial.println("PONG");
  } else if (cmd == "OPEN_BARRIER") {
    barrierServo.write(90);  
    barrierOpen = true;
    Serial.println("BARRIER:OPEN");
  } else if (cmd == "CLOSE_BARRIER") {
    barrierServo.write(0); 
    barrierOpen = false;
    Serial.println("BARRIER:CLOSED");
  } else {
    Serial.println("UNKNOWN:" + cmd); 
  }
}

