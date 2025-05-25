#define ENA 6
#define IN1 8
#define IN2 7
#define ENB 3
#define IN3 4
#define IN4 5

const float linealSpeed = 100; // cm/s (v)
const float turningSpeed = 30; // rpm (ω)

const float K_vL = 100.0 / 12.0; // rpm/V per al motor esquerre
const float K_vR = 100.0 / 12.0;  // rpm/V per al motor dret

const float V_bat = 12; // V : Bateria
const float r = 4; // cm : Radi de les rodes
const float d = 5; // cm : Distància entre el centre i una roda

// PWM per moviment lineal
const uint8_t linealPWML = 255 * linealSpeed / (V_bat * K_vL * r);
const uint8_t linealPWMR = 255 * linealSpeed / (V_bat * K_vR * r);

// PWM per gir
const uint8_t turningPWML = 255 * (d * turningSpeed) / (V_bat * K_vL * r);
const uint8_t turningPWMR = 255 * (d * turningSpeed) / (V_bat * K_vR * r);

void initMotors() {
  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
}

void setLeft(uint8_t pwm, bool dir) {
  analogWrite(ENA, pwm);
  digitalWrite(IN1, dir);
  digitalWrite(IN2, !dir);
}

void setRight(uint8_t pwm, bool dir) {
  analogWrite(ENB, pwm);
  digitalWrite(IN3, dir);
  digitalWrite(IN4, !dir);
}

void moveForward() {
  setLeft(linealPWML, true);
  setRight(linealPWMR, true);
}

void stopMotors() {
  setLeft(0, false);
  setRight(0, false);
}

void rotate(float angle) {
  bool dirL = angle > 0;
  bool dirR = angle <= 0;
  setLeft(turningPWML, dirL);
  setRight(turningPWMR, dirR);
  float absAngle = abs(angle);
  unsigned long rotateDuration = (unsigned long)(absAngle / (2.0 * PI / 60.0 * turningSpeed) * 1000.0);
  delay(rotateDuration);
  stopMotors();
}
