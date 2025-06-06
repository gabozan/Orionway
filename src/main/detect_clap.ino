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
