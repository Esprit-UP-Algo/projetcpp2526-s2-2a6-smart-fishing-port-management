// Sketch Input: détection de bateau avec HC-SR04 (UNO)
// TRIG -> D9, ECHO -> D10, LED statut -> D13

const int TRIG_PIN = 9;
const int ECHO_PIN = 10;
const int STATUS_LED = 13;
const unsigned long SAMPLE_INTERVAL = 1000; // ms
const int DETECT_THRESHOLD_CM = 50; // distance seuil pour détection

unsigned long lastSample = 0;

void setup() {
  Serial.begin(9600);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(STATUS_LED, OUTPUT);
  digitalWrite(STATUS_LED, LOW);
  delay(100);
  Serial.println("INPUT_SKETCH_READY");
}

long measureDistanceCm() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 30000); // timeout 30ms
  if (duration == 0) return -1; // pas de retour
  long distanceCm = duration / 29 / 2;
  return distanceCm;
}

void loop() {
  if (millis() - lastSample < SAMPLE_INTERVAL) return;
  lastSample = millis();

  long dist = measureDistanceCm();
  if (dist > 0 && dist <= DETECT_THRESHOLD_CM) {
    digitalWrite(STATUS_LED, HIGH);
    Serial.print("SHIP_DETECTED:");
    Serial.println(dist);
  } else {
    digitalWrite(STATUS_LED, LOW);
    Serial.print("NO_SHIP:");
    if (dist == -1) Serial.println("TIMEOUT"); else Serial.println(dist);
  }
}
