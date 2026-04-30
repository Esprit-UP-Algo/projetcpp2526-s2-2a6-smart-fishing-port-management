// Sketch Input: détection de bateau avec HC-SR04 (UNO)
// TRIG -> D9, ECHO -> D10, LED statut -> D13

const int TRIG_PIN = 9;
const int ECHO_PIN = 10;
const int STATUS_LED = 13;
const unsigned long SAMPLE_INTERVAL = 1000; // ms
const int DETECT_THRESHOLD_CM = 50; // distance seuil pour détection

unsigned long lastSample = 0;
unsigned long lastBlink = 0;
bool blinkMode = false;
bool solidMode = false;
int ledState = LOW;

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
  // Lecture des commandes envoyées par l'application Qt
  if (Serial.available() > 0) {
    char command = Serial.read();
    if (command == 'E') { // Mode Entrée : Clignotement
      blinkMode = true;
      solidMode = false;
    } else if (command == 'S') { // Mode Sortie : Fixe
      blinkMode = false;
      solidMode = true;
      digitalWrite(STATUS_LED, HIGH);
    } else if (command == 'C') { // Reset / Clear
      blinkMode = false;
      solidMode = false;
      digitalWrite(STATUS_LED, LOW);
    }
  }

  // Gestion du clignotement (500ms) si blinkMode est actif
  if (blinkMode) {
    if (millis() - lastBlink >= 500) {
      lastBlink = millis();
      ledState = (ledState == LOW) ? HIGH : LOW;
      digitalWrite(STATUS_LED, ledState);
    }
  }

  // Mesure de distance (on ne l'exécute que si aucun mode forcé n'est actif)
  if (millis() - lastSample < SAMPLE_INTERVAL) return;
  lastSample = millis();

  long dist = measureDistanceCm();

  if (!blinkMode && !solidMode) {
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
}
