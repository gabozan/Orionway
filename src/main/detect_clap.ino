const int micPin = A0;
const int soundThreshold = 800;
const unsigned long minTimeGap = 200;
const unsigned long maxTimeGap = 500;
unsigned long previousTapTime = 0;
unsigned long timeSinceLastTap = 0;


int readInput() {
  int minValue = 1024;
  int maxValue = 0;
  for (int i = 0; i < 100; ++i) {
    int value = analogRead(micPin);
    minValue = min(minValue, value);
    maxValue = max(maxValue, value);
  }
  return maxValue - minValue;
}

bool getTaps() {
  int delta = readInput();
  if (delta > soundThreshold) {
    timeSinceLastTap = millis() - previousTapTime;
    if (timeSinceLastTap > minTimeGap && timeSinceLastTap < maxTimeGap) {
      return true;
    }
    previousTapTime = millis();
  }
  return false;
}
