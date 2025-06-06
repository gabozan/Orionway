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
