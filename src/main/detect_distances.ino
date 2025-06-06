void initDistancies()
{
  Serial.begin(9600);
  for (int i = 0; i < 3; i++)
  {
    pinMode(trigPins[i], OUTPUT);
    pinMode(echoPins[i], INPUT);
    digitalWrite(trigPins[i], LOW);
  }
}


void DetectaDistancies()
{
  for (int i = 0; i < 3; i++)
  {
    switch (estat_distances[i])
    {
      case IDLE:
        if (micros() - tempsAnteriorLectura > intervalLectura)
        {
          digitalWrite(trigPins[i], LOW);
          estat_distances[i] = TRIGGER;
        }
        break;

      case TRIGGER:
        digitalWrite(trigPins[i], HIGH);
        delayMicroseconds(10);
        digitalWrite(trigPins[i], LOW);
        tempsInici[i] = micros();
        estat_distances[i] = ESPERANT_HIGH;
        break;

      case ESPERANT_HIGH:
        if (digitalRead(echoPins[i]) == HIGH)
        {
          tempsHighStart[i] = micros();
          estat_distances[i] = ESPERANT_LOW;
        }
        else if (micros() - tempsInici[i] > timeoutEcho)
        {
          distancies[i] = 0;
          estat_distances[i] = CALCULAT;
        }
        break;

      case ESPERANT_LOW:
        if (digitalRead(echoPins[i]) == LOW)
        {
          duracions[i] = micros() - tempsHighStart[i];
          distancies[i] = duracions[i] * 0.0343 / 2.0;
          estat_distances[i] = CALCULAT;
        }
        else if (micros() - tempsHighStart[i] > timeoutEcho)
        {
          distancies[i] = 0;
          estat_distances[i] = CALCULAT;
        }
        break;

      case CALCULAT:
        estat_distances[i] = IDLE;
        if (i == 2) tempsAnteriorLectura = micros(); // últim sensor → reiniciar cicle
        break;
    }
  }
}

void TractaDistancies(bool (&objectesDetectats)[3])
{
  for (int i = 0; i < 3; i++)
  {
    objectesDetectats[i] = (distancies[i] > 0 && distancies[i] < thresholds[i]);
  }
}


void RevisaObstacles(bool (&objectesDetectats)[3])
{
  DetectaDistancies();
  TractaDistancies(objectesDetectats);
}
