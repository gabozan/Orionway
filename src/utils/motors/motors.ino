#define ENA 6
#define IN1 8
#define IN2 7
#define ENB 3
#define IN3 5
#define IN4 4

int velocidad = 30;
bool sentido = false;

void setup() {
  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  analogWrite(ENA, velocidad);
}

void loop() {
  if(sentido){
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
  }
  else
  {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
  }
    delay(2000);
    sentido = !sentido;
}
