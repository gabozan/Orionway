const int soundThreshold = 800;
const unsigned long minTimeGap = 200;
const unsigned long maxTimeGap = 500;
int taps = 0;

unsigned long previousTapTime = 0;
unsigned long timeSinceLastTap = 0;

void setup() {
  Serial.begin(9600);
  Serial.println("Microfon_Casa\tMicrofon_Classe");
}

int readInput(int pin) {
  int minValue = 1024;
  int maxValue = 0;
  for (int i = 0; i < 100; ++i) {
    int value = analogRead(pin);
    minValue = min(minValue, value);
    maxValue = max(maxValue, value);
  }
  return maxValue - minValue;
}

void loop() {
  int variation1 = readInput(A0);
  int variation2 = readInput(A1);

  Serial.print(variation1);
  Serial.print("\t");
  Serial.println(0);

  if (variation1 > soundThreshold) {
    timeSinceLastTap = millis() - previousTapTime;
    if (timeSinceLastTap > minTimeGap && timeSinceLastTap < maxTimeGap) {
      taps++;
    }
    previousTapTime = millis();
  }

  delay(20);
}
