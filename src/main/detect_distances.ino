const int trigPins[3] = {2, 4, 6};
const int echoPins[3] = {3, 5, 7};

const int thresholds[3] = {40, 40, 40};

float distancies[3];

// Màquina d’estats per sensor
enum EstatSensor { IDLE, TRIGGER, ESPERANT_HIGH, ESPERANT_LOW, CALCULAT };
EstatSensor estat[3] = {IDLE, IDLE, IDLE};

unsigned long tempsInici[3];
unsigned long tempsHighStart[3];
unsigned long duracions[3];

const unsigned long timeoutEcho = 30000; 
const unsigned long intervalLectura = 60000;
unsigned long tempsAnteriorLectura = 0;


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
    switch (estat[i])
    {
      case IDLE:
        if (micros() - tempsAnteriorLectura > intervalLectura)
        {
          digitalWrite(trigPins[i], LOW);
          estat[i] = TRIGGER;
        }
        break;

      case TRIGGER:
        digitalWrite(trigPins[i], HIGH);
        delayMicroseconds(10);
        digitalWrite(trigPins[i], LOW);
        tempsInici[i] = micros();
        estat[i] = ESPERANT_HIGH;
        break;

      case ESPERANT_HIGH:
        if (digitalRead(echoPins[i]) == HIGH)
        {
          tempsHighStart[i] = micros();
          estat[i] = ESPERANT_LOW;
        }
        else if (micros() - tempsInici[i] > timeoutEcho)
        {
          distancies[i] = 0;
          estat[i] = CALCULAT;
        }
        break;

      case ESPERANT_LOW:
        if (digitalRead(echoPins[i]) == LOW)
        {
          duracions[i] = micros() - tempsHighStart[i];
          distancies[i] = duracions[i] * 0.0343 / 2.0;
          estat[i] = CALCULAT;
        }
        else if (micros() - tempsHighStart[i] > timeoutEcho)
        {
          distancies[i] = 0;
          estat[i] = CALCULAT;
        }
        break;

      case CALCULAT:
        estat[i] = IDLE;
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
