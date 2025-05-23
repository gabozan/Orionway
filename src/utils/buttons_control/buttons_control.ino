// Todos los tiempos se encuentran en milisegundos
const int buttonPin = 2;
int estatActual = HIGH;
int estatAnterior = HIGH;
unsigned long lastPressTime = 0;
int clickCount = 0;
unsigned long lastReleaseTime = 0;
const long debounceDelay = 50; 
const long doubleClickInterval = 300; // Intervalo máximo entre clics para considerar doble clic
const long holdTimeThreshold = 3000; // Tiempo para considerar pulsación larga

// Estado para la detección de clics:
// 0: Esperando el primer clic
// 1: Primer clic detectado, esperando posible segundo clic
// 2: Botón pulsado durante más de un loop (posible pulsación larga con función para botón central)
int clickState = 0;
unsigned long holdStartTime = 0; // Tiempo en que comenzó la pulsación larga

void setup() {
  pinMode(buttonPin, INPUT_PULLUP);
  Serial.begin(9600);
}

void loop() {
  estatActual = digitalRead(buttonPin);
  unsigned long currentTime = millis();

  // Detección del flanco de bajada (pulsar)
  if (estatActual == LOW && estatAnterior == HIGH && (currentTime - lastPressTime) > debounceDelay) {
    lastPressTime = currentTime;
    clickCount++;
    clickState = 1; // El botón ahora está pulsado
    holdStartTime = currentTime; // Registrar el inicio de la pulsación para posible pulsación larga posterior
  }
  
  // Si está dos iteraciones del bucle el botón pulsado se considerará que hay posibilidad de una pulsación larga
  if (estatActual == LOW && estatAnterior == LOW)
  {
    clickState = 2;
  }

  // Detección del flanco de subida (soltar)
  if (estatActual == HIGH && estatAnterior == LOW) {
    lastReleaseTime = currentTime;
    if (clickState != 2) { // Si no venimos de una pulsación larga
      clickState = 1; // Se soltó después de un posible clic
    } else {
      clickState = 0; // Se soltó después de una pulsación larga
    }
  }

  // Lógica para detectar pulsación larga
  if (clickState == 2 && (currentTime - holdStartTime) >= holdTimeThreshold) {
    Serial.println("Pulsación larga (3 segundos)");
    clickState = 0; // Volver al estado de espera
    clickCount = 0; // Reiniciar el contador de clics
    holdStartTime = currentTime; // Reiniciar el tiempo de botón pulsado para evitar llamar en numerosas ocasiones a la función del botón central
  }

  // Lógica para determinar el tipo de clic después de un intervalo
  if (clickState !=2 && (currentTime - lastReleaseTime) > doubleClickInterval && lastReleaseTime > 0) {
    if (clickCount == 1) {
      Serial.println("Un click");
    } else if (clickCount == 2) {
      Serial.println("Dos clicks");
    }
    clickCount = 0; 
    clickState = 0; // Volver al estado de espera del primer clic
    lastReleaseTime = 0; // Reiniciar el tiempo de liberación
  }

  estatAnterior = estatActual;
}