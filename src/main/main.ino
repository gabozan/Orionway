const int LED = 4;
const int BUTTON = 3;
bool actual = HIGH;
bool last = HIGH;

void setup()
{
  Serial.begin(115200);
  pinMode(LED, OUTPUT);
  pinMode(BUTTON, INPUT_PULLUP);
}

void loop()
{
  actual = digitalRead(BUTTON);

  if (last == HIGH && actual == LOW)
  {
    Serial.println(1);
  }

  last = actual;

  if (Serial.available())
  {
    int input = Serial.read() - '0';

    switch (input)
    {
    case 0:
      digitalWrite(LED, LOW);
      break;
    case 1:
      digitalWrite(LED, HIGH);
      break;
    }
  }
}