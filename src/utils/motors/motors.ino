#define ENA 6
#define IN1 8
#define IN2 7
#define ENB 3
#define IN3 4
#define IN4 5

const float linealSpeed = 100; // cm/s (v)
const float turningSpeed = 30; // rpm (ω)

const float K_v = 100/12; // rpm/V : Speed constant
const float V_bat = 12; // V : Bateria
const float r = 4; // cm : Radi de les rodes
const float d = 5; // cm : Distància entre el centre i una roda

const uint8_t linealPWM = 255 * linealSpeed/(V_bat*K_v*r);
const uint8_t turningPWM = 255 * (d*turningSpeed)/(V_bat*K_v*r);

void initMotors(){
  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
}

void setLeft(uint8_t pwm, bool dir)  { analogWrite(ENA, pwm); digitalWrite(IN1, dir);  digitalWrite(IN2, !dir); }
void setRight(uint8_t pwm, bool dir) { analogWrite(ENB, pwm); digitalWrite(IN3, dir);  digitalWrite(IN4, !dir); }

void moveForward()
{
  setLeft(linealPWM, true);
  setRight(linealPWM, true);
}

void stopMotors()
{
  setLeft(0, false);
  setRight(0, false);
}

void rotate(float angle)
{
  setLeft(turningPWM, angle > 0);
  setRight(turningPWM, angle <= 0);
  float absAngle = angle > 0 ? angle : -angle;
  unsigned long rotateDuration = (unsigned long)(absAngle / (2.0 * PI / 60.0 * turningSpeed) * 1000.0);
  delay(rotateDuration);
  stopMotors();
}
