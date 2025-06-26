void initDistancies()
{
  for (int i = 0; i < 3; i++)
  {
    pinMode(trigPins[i], OUTPUT);
    pinMode(echoPins[i], INPUT);
    digitalWrite(trigPins[i], LOW);
  }
}

void TractaDistancia(float dist, int i)
{
  objectesDetectats[i] = (dist > 0 && dist < thresholds[i]);
}

float MesuraDistancia(int trigPin, int echoPin)
{
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  unsigned long tempsInici = micros();
  while (digitalRead(echoPin) == LOW)
  {
    if (micros() - tempsInici > timeoutEcho) return 0;
  }

  unsigned long tempsHighStart = micros();
  while (digitalRead(echoPin) == HIGH)
  {
    if (micros() - tempsHighStart > timeoutEcho) return 0;
  }

  unsigned long durada = micros() - tempsHighStart;
  return durada * 0.0343 / 2.0;
}

void RevisaObstacles()
{
  for (int i = 0; i < 3; i++)
  {
    distancies[i] = MesuraDistancia(trigPins[i], echoPins[i]);
    TractaDistancia(distancies[i], i);
  }
}

