const int LED = 2;
String input;

void setup() {
  Serial.begin(9600);
  pinMode(LED, OUTPUT);
}

void loop() {
  if (Serial.available()) {
    input = Serial.readStringUntil('\n');
    input.trim();

    if (input == "on") {
      digitalWrite(LED, HIGH);
    } else if (input == "off") {
      digitalWrite(LED, LOW);
    }
  }
}
