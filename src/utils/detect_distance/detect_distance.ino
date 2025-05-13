const int trigPins[3] = {2, 4, 6};  // TRIG para izq, mid, der
const int echoPins[3] = {3, 5, 7};  // ECHO para izq, mid, der

unsigned long duracion[3];
int distancia[3];

unsigned long tiempoAnterior = 0;
const unsigned long intervalo = 500000;  // 500 ms en microsegundos

void setup() {
  Serial.begin(9600);
  for (int i = 0; i < 3; i++) {
    pinMode(trigPins[i], OUTPUT);
    pinMode(echoPins[i], INPUT);
  }
}

void loop() {
  unsigned long tiempoActual = micros();

  if (tiempoActual - tiempoAnterior >= intervalo) {
    tiempoAnterior = tiempoActual;

    for (int i = 0; i < 3; i++) {
      digitalWrite(trigPins[i], LOW);
      delayMicroseconds(2);
      digitalWrite(trigPins[i], HIGH);
      delayMicroseconds(10);
      digitalWrite(trigPins[i], LOW);

      duracion[i] = pulseIn(echoPins[i], HIGH, 30000);
      if (duracion[i] > 0) {
        distancia[i] = duracion[i] * 0.0343 / 2;
      } else {
        distancia[i] = -1; // Valor para indicar "sin lectura"
      }
    }
    Serial.println(distancia[0]);
    Serial.println(distancia[1]);
    Serial.println(distancia[2]);


  }
}
