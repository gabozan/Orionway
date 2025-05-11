const int microphonePin = A0;
const int soundThreshold = 500;
const unsigned long minTimeGap = 200;
const unsigned long maxTimeGap = 500;

unsigned long previousTapTime = 0;
unsigned long timeSinceLastTap = 0;

void setup() {
  Serial.begin(9600);
}

int readInput() {
  int minValue = 1024;
  int maxValue = 0;
  for (int i = 0; i < 100; ++i) {
    int value = analogRead(microphonePin);
    minValue = min(minValue, value);
    maxValue = max(maxValue, value);
  }
  return maxValue - minValue;
}

void loop() { 
  int variation = readInput();
  if (variation > soundThreshold) {
    timeSinceLastTap = millis() - previousTapTime;
    if (timeSinceLastTap > minTimeGap && timeSinceLastTap < maxTimeGap) {
      Serial.println("Tap!");
    }
    previousTapTime = millis();
  }
}
